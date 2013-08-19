

#include "rule.h"

#include "creature.h"
#include "dice.h"
#include "ui.h"
#include "log.h"
#include <string>
#include <sstream>

namespace rule {

int MaxHP(Creature* c)
{
    DCHECK(c != NULL);
    if (!c)
        return 0;
    int conmod = (c->Con() - 10) / 2;
    if (conmod < 1)
        conmod = 1;
    return dice(8, 1) + dice(8, conmod);
}

bool Hit(Creature* h, Creature* d, bool& Critical)
{
    DCHECK(h != NULL);
    DCHECK(d != NULL);
    if (!h || !d)
        return false;

    int dd = d20();
    if (dd >= 19) {
        Critical = true;
        return true;
    }

    Critical = false;
    int hit = (h->Str() - 10) / 2 + dd;
    int ac = ArmorClass(d);

    std::stringstream   ss;
    ss << hit << " vs " << ac << ". ";

    bool bHit = false;
    if (hit >= ac) {
        ss << " Hit!";
        bHit = true;
    } else {
        ss << " Miss!";
        bHit = false;
    }
    UI::Log(ss.str());
    return bHit;
}

int  ArmorClass(Creature* c)
{
    DCHECK(c != NULL);
    if (!c)
        return 0;
    return (c->Dex() - 10) / 2;
}

int  Damage(Creature* h, Creature* d, bool Critical)
{
    int dm = (h->Str() - 10) / 2;
    if (dm < 0)
        dm = 0;
    if (Critical)
        dm += 4;
    else
        dm += dice(4, 1);
    d->HPMod(-dm);
    return dm;
}

};



