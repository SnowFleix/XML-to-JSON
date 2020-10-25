#pragma once

#include <string>
#include <cctype>

// checks if the passed string is empty or just full of whitespace
bool isOnlyWhitespace(const std::string& str)
{
    auto it = str.begin();
    do {
        if (it == str.end()) {
            return true;
        }
    } while (*it >= 0 && *it < str.size() && std::isspace(*(it++)));
    return false;
}