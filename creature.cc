
#include "creature.h"
#include "gamemap.h"
#include "ui.h"
#include "rule.h"
#include <stdlib.h>
#include <string>
#include <sstream>
#include <algorithm>

#include "dice.h"

#include "log.h"

Creature::Creature(Race::Type type)
    : Race(type)
    , attr(10)
    , arg(NULL)
    , x_(0)
    , y_(0)
{
    Generate();
}

Creature::Creature()
    : Race(Race::Race_Random)
    , attr(-1)
    , arg(NULL)
    , x_(0)
    , y_(0)
{
    Generate();
}

Creature::~Creature()
{
    Map::Node * n = Map::Get()->at(x(), y());
    if (n->creature == this)
        n->creature = NULL;
    std::list<Item*>::iterator  it;
    for (it=inventory.begin(); it!=inventory.end(); it++) {
        std::stringstream ss;
        ss << "drop " << (*it)->name;
        UI::Log(ss.str());
        n->items.push_back(new Item(*(*it)));
        delete *it;
    }
}

void Creature::Generate(void)
{
    maxhp = rule::MaxHP(this);
    hp = maxhp;
    speed_ = dice(6, 1);
    /*
    int cm = (Con() - 10) / 2;
    if (cm < 1)
        cm = 1;
    maxhp = dice(8, 1) + dice(8, cm);
    hp = maxhp;
    */
}

bool Creature::Move(int xmod, int ymod, Creature*& c)
{
    int dx = x() + xmod;
    int dy = y() + ymod;
    return MoveTo(dx, dy, c);
}

bool Creature::MoveTo(int tx, int ty, Creature*& c)
{
    Map* mp = Map::Get();
    Map::Node * s = mp->at(x(), y());
    Map::Node * n = mp->at(tx, ty);
    if (n->creature) {
        c = n->creature;
        return false;
    }
    /*
    if (!n->canpass) {
        return false;
    }
    */
    x_ = tx;
    y_ = ty;

    if (s->creature == this) {
        s->creature = NULL;
    }
    n->creature = this;
    return true;
}

// battle
void Creature::Melee(Creature* o)
{
    std::stringstream   ss;
    ss.str("");
    ss << "ATTACK: " << name << " => " << o->name;
    UI::Log(ss.str());
    bool critical = false;
    bool hit = rule::Hit(this, o, critical);
    if (hit) {
        int d = rule::Damage(this, o, critical);
        ss.str("");
        ss << d << " point damage. (" << o->HP() << "/" << o->MAXHP() << ")";
        UI::Log(ss.str());
    }
}

void Creature::Equip(Item* item)
{
    if (!item)
        return;
    std::list<Item*>::iterator  it;
    it = std::find(inventory.begin(), inventory.end(), item);
    if (it == inventory.end())
        return;

    if (item->Enabled()) {
        item->Enable(false);
        return;
    }
    for (it = inventory.begin(); it != inventory.end(); it++) {
        if ((*it)->type == item->type && (*it)->Enabled()) {
            (*it)->Enable(false);
        }
    }
    item->Enable(true);
    
}

void Creature::Give(Item* item)
{
    if (!item)
        return;
    inventory.push_back(item);
}







