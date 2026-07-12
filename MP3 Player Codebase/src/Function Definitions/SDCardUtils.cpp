#include <MusicPlayer.h>

//TODO: Test that these functions work
//TODO: Remove the usage of the SD card to print these
void MusicPlayer::printCurrentDirectory() {
    for (int i = 0; i < currentDirList.size(); i++) {
        if (i == selectedItemIndex) Serial.print("• ");

        Serial.println(currentDirList[i]);
    }
}

//TODO: Confirm this works with other folders
void MusicPlayer::populateCurrentDirList() {
    for (const char* name : currentDirList) delete[] name;
    currentDirList.clear();

    JsonDocument filter;
    filter[String(currentDir)] = true;

    File foldersFile = LittleFS.open(foldersFilePath, "r");
    DeserializationError err = deserializeJson(foldersDoc, foldersFile);
    foldersFile.close();

    parentDir = foldersDoc[String(currentDir)]["parent_dir"].as<const char*>();
    
    if (strcmp(parentDir, "") != 0) currentDirList.push_back("../");

    JsonArray items = foldersDoc[String(currentDir)]["items"];

    for (JsonVariant item: items) {
        currentDirList.push_back(item.as<const char *>());
    }

    formatCurrentDirList();

    foldersDoc.clear();
}

void MusicPlayer::formatCurrentDirList() {
    stable_partition(currentDirList.begin(), currentDirList.end(), [](const char* a) {
        size_t len = strlen(a);
        return len > 0 && a[len - 1] == '/';
    }); 
}

void MusicPlayer::addFoldertoJSON(FsFile dir, const char* parentDir) { 
    // Get the name of the current directory, concat "/" at the end
    char directoryName[MAX_NAME_SIZE];
    dir.getName(directoryName, sizeof(directoryName));
    snprintf(directoryName, sizeof(directoryName), "%s%s", directoryName, "/");

    // Make a nested object with the first key being the directory name and the second key being "parent_dir"
    // to keep the parent directory of the folder (empty string for root)
    foldersDoc[String(directoryName)]["parent_dir"] = String(parentDir); 

    // Add an array called items to store the items of the current directory
    JsonArray items = foldersDoc[String(directoryName)]["items"].to<JsonArray>();

    // Loop that goes through all items of the current directory
    while (true) {
        // Open an item in the current directory and break if there are no more items
        FsFile item = dir.openNextFile();
        if (!item) break;

        // Get its name
        char name[MAX_NAME_SIZE];
        item.getName(name, sizeof(name));

        // If an item is a directory, concat "/" at the end and add it as a folder to the folders array
        if (item.isDir()) {
            snprintf(name, sizeof(name), "%s%s", name, "/");
            addFoldertoJSON(item, directoryName);
        }

        // Add the name of the item to the array
        items.add(name);

        item.close();
    }

}

void MusicPlayer::saveJSON() {
    File pathsJSONFile = LittleFS.open(foldersFilePath, "w");
    if (!pathsJSONFile) return;
    serializeJson(foldersDoc, pathsJSONFile);
    pathsJSONFile.close();
    foldersDoc.clear();
}

void MusicPlayer::testJSON() {
    File pathsJSONFile = LittleFS.open("/paths.json", "r");

    if (!pathsJSONFile) {
        Serial.println("failed to open file");
        return;
    }

    DeserializationError err = deserializeJson(foldersDoc, pathsJSONFile);
    pathsJSONFile.close();

    if (err) {
        Serial.print("Parse failed: ");
        Serial.println(err.c_str());
    }

    serializeJsonPretty(foldersDoc, Serial);
    Serial.println();
}

