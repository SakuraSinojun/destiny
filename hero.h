
#pragma once

#include "creature.h"

class Hero : public Creature
{
public:
    virtual ~Hero();
    static Hero* Get();

    virtual bool turn(int turncount, bool& again);
private:
    Hero();

};

