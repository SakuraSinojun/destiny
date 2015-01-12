
#pragma once

#include <string>
#include <list>
#include "item.h"
#include "race.h"
#include "class.h"
#include "attr.h"

class Map;
class Creature : public Race, Class
{
public:
    Creature();
    Creature(Race::Type type);
    virtual ~Creature();


    int Str() { return attr.Str() + attrmod.Str(); }
    int Dex() { return attr.Dex() + attrmod.Dex(); }
    int Con() { return attr.Con() + attrmod.Con(); }
    int Wis() { return attr.Wis() + attrmod.Wis(); }
    int Int() { return attr.Int() + attrmod.Int(); }
    int Chr() { return attr.Chr() + attrmod.Chr(); }

    int HP(void) { return hp; }
    int HPMod(int m) { 
        hp += m;
        if (hp > maxhp)
            hp = maxhp;
        return hp; 
    }
    int MAXHP(void) { return maxhp; }

    Attr    attr;
    std::string type;
    std::string name;

    // type:
    bool Move(int xmod, int ymod, Creature*& c);
    bool MoveTo(int tx, int ty, Creature*& c);
    int& x() { return this->x_; }
    int& y() { return this->y_; }

    // item:
    std::list<Item*> inventory;
    void Equip(Item* item);
    void Give(Item* item);

    // battle:
    void Melee(Creature* o);


    int speed(void) { return speed_; }
    void SetSpeed(int sp) { speed_ = sp; }
    virtual bool turn(int turncount, bool& again) = 0;

    void * arg;

protected:
    int         x_;
    int         y_;

    int         hp;
    int         maxhp;
    int         speed_;

    void    Generate(void);
};

