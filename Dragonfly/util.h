#pragma once

#include <sys/stat.h>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>
#include <ctime>
#include "time.h"

using namespace std;

inline bool FileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

inline std::time_t IntDateToTime(int int_date) {
    std::tm time_in = { 0, 0, 0, // second, minute, hour
                        int_date % 100, int_date / 100 % 100 - 1, int_date / 10000 - 1900
                      }; // 1-based day, 0-based month, year since 1900
    std::time_t temp = std::mktime(&time_in);
    return temp;
}

inline std::string StructToHexString(const unsigned char* data, int size) {
    std::ostringstream op;
    // Format flags
    std::ostream::fmtflags old_flags = op.flags();

    // Fill characters
    char old_fill = op.fill();
    op << hex << setfill('0');

    for (int i = 0; i < size; i++) {
        // Give space between two hex values
        if (i > 0) {
            //op << ' ';
        }

        // force output to use hex version of ascii code
        op << "" << setw(2) << static_cast<int>(data[i]);
    }

    op.flags(old_flags);
    op.fill(old_fill);
    //return std::string("0x") + op.str();
    return op.str();
}
inline uint32_t OneHexStringToUInt32(std::string str) {
    uint32_t u;
    istringstream ip_convert(str);
    ip_convert >> hex;
    ip_convert >> u;
    return u;
}
inline void HexStringToStruct(std::string str, unsigned char* data, int size) {
    int index = 0;
    for (int i = 0; i < str.size(); i += 2) {
        std::string hexstring = str.substr(i, 2);
        uint32_t u = OneHexStringToUInt32(hexstring);
        data[index++] = static_cast<unsigned char>(u);
    }
}