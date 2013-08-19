
#pragma once

#include "scene.h"
#include "creature.h"
#include <list>

class Monster;
class Map;
class Hero;

class SceneGame : public Scene
{
public:
    SceneGame();
    virtual ~SceneGame();
    virtual bool run(void);
private:
    void GenMonsters(void);
    void RemoveDeadMonsters(void);

    typedef struct CreatureCircle {
    public:
        CreatureCircle() : c(NULL), next(NULL), prev(NULL) {}
        CreatureCircle(Creature* m) : c(m), next(NULL), prev(NULL) { c->arg = (void*)this; }
        Creature* c;
        CreatureCircle* next;
        CreatureCircle* prev;
    } CreatureCircle;

    CreatureCircle* current_creature;
    void AttachCreature(Creature* c);
    void DetachCreature(Creature* c);
    void DetachCreature(CreatureCircle* cc);
};

