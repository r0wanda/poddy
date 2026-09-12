#pragma once
#include <regex>
#include <string>
#include <iomanip>
#include <sstream>

// https://gist.github.com/arthurafarias/56fec2cd49a32f374c02d1df2b6c350f

std::string encodeURIComponent(std::string decoded) {
    std::ostringstream oss;
    std::regex r("[!'\\(\\)*-.0-9A-Za-z_~]");

    for (char &c : decoded)
    {
        if (std::regex_match((std::string){c}, r))
        {
            oss << c;
        }
        else
        {
            oss << "%" << std::uppercase << std::hex << (0xff & c);
        }
    }
    return oss.str();
}
