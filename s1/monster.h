
#pragma once

#include "creature.h"
#include <string>

class Hero;
class Map;
class Monster : public Creature
{
public:
    Monster();
    virtual ~Monster();

    virtual bool turn(int turncount, bool& again);

private:
    int  moving;
    void step(void);
};


