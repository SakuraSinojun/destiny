

#pragma once


class Creature;
namespace rule {

int  MaxHP(Creature* c);
bool Hit(Creature* o, Creature* d, bool& Critical);
int  Damage(Creature* h, Creature* d, bool Critical);
int  CriticalDamage(Creature* h, Creature* d);
int  ArmorClass(Creature* c);

};


