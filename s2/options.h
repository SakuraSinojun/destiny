
#pragma once

#include <string>

class options
{
public:
    typedef enum {
        OPT_NULL = 0,
    } option_key;

    std::string get(option_key key);
    std::string operator[](option_key key) { return get(key); }
};



