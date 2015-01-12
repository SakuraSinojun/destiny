
#include "string_util.h"

namespace tools {

std::string trim(std::string input, const char * trim_chars)
{
    size_t  pos1 = input.find_first_not_of(trim_chars);
    size_t  pos2 = input.find_last_not_of(trim_chars);
    if (input.empty() || pos1 == std::string::npos || pos2 == std::string::npos)
        return std::string("");
    return input.substr(pos1, pos2 - pos1 + 1);
}

}


