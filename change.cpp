#include <iostream>
#include <unistd.h>
#include <dirent.h>
#include <string>
#include <cstdio>
#include <cctype>

bool hasExtension(const std::string& filename, const std::string& ext) {
    if (filename.length() >= ext.length()) {
        return filename.compare(filename.length() - ext.length(), ext.length(), ext) == 0;
    }
    return false;
}

std::string toLowerFirstChar(const std::string& filename) {
    std::string result = filename;
    if (!result.empty() && std::isupper(result[0])) {
        result[0] = std::tolower(result[0]);
    }
    return result;
}

int main() {
    const char* directory_path = "src";
    
    DIR* dir = opendir(directory_path);
    if (dir == NULL) {
        std::cerr << "Error: Could not open directory " << directory_path << std::endl;
        return 1;
    }
    
    struct dirent* entry;
    std::cout << "Renaming .cpp files in src/ directory:" << std::endl;
    
    while ((entry = readdir(dir)) != NULL) {
        std::string filename(entry->d_name);
        
        // Skip . and .. entries
        if (filename == "." || filename == "..") {
            continue;
        }
        
        // Only process .cpp files
        if (!hasExtension(filename, ".cpp")) {
            continue;
        }
        
        // Check if first character is uppercase
        if (filename.empty() || !std::isupper(filename[0])) {
            std::cout << "- " << filename << " (no change needed)" << std::endl;
            continue;
        }
        
        // Create new filename with lowercase first letter
        std::string newFilename = toLowerFirstChar(filename);
        
        // Create full paths
        std::string oldPath = std::string(directory_path) + "/" + filename;
        std::string newPath = std::string(directory_path) + "/" + newFilename;
        
        // Rename the file
        if (rename(oldPath.c_str(), newPath.c_str()) == 0) {
            std::cout << "- Renamed: " << filename << " -> " << newFilename << std::endl;
        } else {
            std::cerr << "- Error renaming: " << filename << std::endl;
        }
    }
    
    closedir(dir);
    
    return 0;
}