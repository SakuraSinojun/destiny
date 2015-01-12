
#include "race.h"

Race::Race(Race::Type t)
    : type(t)
{
    if (type == Race_Human) {
        name = "Human";
    } else if (type == Race_Elf) {
        name = "Elf";
        attrmod.dex_ = 2;
        attrmod.con_ = -2;
    } else {
        name = "NONE";
    }
}

Race::~Race()
{
}


