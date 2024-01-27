#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <dirent.h>

std::vector<int> getPidsByName(const std::string &processName) {
    std::vector<int> pids;

    DIR *procDir = opendir("/proc");

    if (procDir == nullptr) {
        std::cerr << "Failed to open /proc directory" << std::endl;
        return pids;
    }

    // Iterate over all entries in the /proc directory
    struct dirent *entry;

    while ((entry = readdir(procDir)) != nullptr) {
        int pid = atoi(entry->d_name);

        if (pid > 0) {
            std::string cmdPath = "/proc/" + std::string(entry->d_name) + "/comm";
            std::ifstream cmdFile(cmdPath);
            std::string cmdName;
            if (cmdFile >> cmdName) {
                if (cmdName == processName) {
                    pids.push_back(pid);
                }
            }
        }
    }

    closedir(procDir);

    return pids;
}

pid_t findPid(const std::string &programName) {
    std::vector<int> pids = getPidsByName(programName);

    if (pids.empty()) {
        std::cout << "Target program not running." << std::endl;
        return 0;
    }

    if (pids.size() > 1) {
        std::cout << "Multiple instances of target program found. Using the first instance." << std::endl;
    }

    return pids[0];
}
