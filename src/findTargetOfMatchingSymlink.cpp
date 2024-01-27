#include <filesystem>
#include <string>
#include <regex>

std::string findTargetOfMatchingSymlink(const std::string &directory, const std::regex &pattern) {
    for (const auto &entry: std::filesystem::directory_iterator(directory)) {
        if (std::filesystem::is_symlink(entry.path())) {
            std::string filename = entry.path().filename().string();
            if (std::regex_match(filename, pattern)) {
                std::string fullPath = directory + '/' + filename;
                return fullPath;
            }
        }
    }

    return "";
}
