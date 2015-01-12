
#include "monster.h"
#include "dice.h"
#include "hero.h"
#include "gamemap.h"
#include "ui.h"

#include "dice.h"

#include <stdlib.h>

Monster::Monster()
    : moving(0)
{
    // SetSpeed(dice(100, 1));
    int i;
    for (i=0; i<30; i++) {
        inventory.push_back(new Item());
    }
}

Monster::~Monster()
{
}

void Monster::step(void)
{
    Hero* h = Hero::Get();
    Creature* cc = NULL;
    int dx = h->x() - x();
    int dy = h->y() - y();
    if (dx != 0)
        dx = dx / abs(dx);
    if (dy != 0)
        dy = dy / abs(dy);

    bool b = Move(dx, dy, cc);
    if (!b) {
        if (!cc) {
            int x = dice(3, 1) - 2;
            int y = dice(3, 1) - 2;
            b = Move(x, y, cc);
        }
        if (!b && cc) {
            Melee(cc);
            UI::Get()->DrawCreatureAttacked(cc);
        }
    }
}

bool Monster::turn(int turncount, bool& again)
{
    Hero* h = Hero::Get();
    if (moving >= h->speed()) {
        UI::Get()->Sleep(100);
        moving -= h->speed();
        again = true;
        step();
        return true;
    }
    moving += speed();
    if (moving >= h->speed()) {
        moving -= h->speed();
        step();
        if (moving >= h->speed()) {
            again = true;
            return true;
        }
    }
    again = false;
    return true;
}







