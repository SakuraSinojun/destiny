
#pragma once

#include <string>
#include "attr.h"

class Race {
public:
    typedef enum {
        Race_Random,
        Race_Human,
        Race_Elf,
    } Type;

    Race(Type type);
    virtual ~Race();

    std::string name;
    Attr        attrmod;
    Type    type;
};

