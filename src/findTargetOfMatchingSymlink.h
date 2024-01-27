#ifndef STOCK_KEYS_FINDTARGETOFMATCHINGSYMLINK_H
#define STOCK_KEYS_FINDTARGETOFMATCHINGSYMLINK_H

#include <string>
#include <regex>

std::string findTargetOfMatchingSymlink(const std::string &directory, const std::regex &pattern);

#endif //STOCK_KEYS_FINDTARGETOFMATCHINGSYMLINK_H
