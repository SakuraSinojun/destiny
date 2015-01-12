

#include "scene_game.h"
#include "gamemap.h"
#include <sstream>
#include <iostream>
#include "ui.h"
#include "log.h"
#include "hero.h"
#include "item.h"
#include "monster.h"
#include "creature.h"
#include "dice.h"

SceneGame::SceneGame()
    : current_creature(NULL)
{
    current_creature = new CreatureCircle(Hero::Get());
    current_creature->next = current_creature;
    current_creature->prev = current_creature;

    GenMonsters();
    UI::Get()->refresh();
    UI::Get()->DrawMap();
    UI::Log("Game Start");
}

SceneGame::~SceneGame()
{
}

bool SceneGame::run(void)
{
    static int turncount = 0;

    UI* ui = UI::Get();
    Hero* h = Hero::Get();
    Map* m = Map::Get();

    m->CenterMap(h->x(), h->y());

    ui->refresh();
    ui->DrawMap();
    ui->DrawInventory();

    /*
    CreatureCircle* cc = current_creature;
    do {
        if (cc->c != Hero::Get())
            ui->DrawCreature(cc->c);
        cc = cc->next;
    } while (cc != current_creature);
    */

    ui->DrawHero();
    ui->DrawHeroStatus();

    bool again = false;
    bool ret = current_creature->c->turn(turncount++, again);
    if (Hero::Get()->HP() <= 0) {
        ui->DrawHeroStatus();
        ui->ShowMessage("   You Died!");
        ui->GetInput();
        return false;
    }

    if (again) {
        return true;
    }
    RemoveDeadMonsters();
    current_creature = current_creature->next;

    return ret;

}
 
void SceneGame::GenMonsters(void)
{
    Hero* hero = Hero::Get();
    int i;
    for (i=0; i<1; i++) {
        Monster* m = new Monster();
        m->type = "Z";
        m->name = "Zombie";
        bool b = false;
        Creature* c = NULL;
        while (!b) {
            b = m->MoveTo(hero->x() - 10 + d20(), hero->y() - 10 + d20(), c);
        }
        AttachCreature(m);
    }
}

void SceneGame::RemoveDeadMonsters(void)
{
    CreatureCircle* c = current_creature;
    while (c->next != current_creature) {
        if (c->c->HP() <= 0 && c->c != Hero::Get()) {
            delete (Monster*)c->c;
            c->c = NULL;
            CreatureCircle* cc = c->next;
            if (current_creature == c)
                current_creature = cc;
            DetachCreature(c);
            c = cc;
        } else {
            c = c->next;
        }
    }
}

void SceneGame::AttachCreature(Creature* c)
{
    DCHECK(c != NULL);
    if (!c)
        return;

    CreatureCircle * cc = new CreatureCircle(c);
    cc->prev = current_creature;
    cc->next = current_creature->next;
    current_creature->next->prev = cc;
    current_creature->next = cc;
}

void SceneGame::DetachCreature(Creature* c)
{
    CreatureCircle* cc = (CreatureCircle*)c->arg;
    DCHECK(cc != NULL);
    if (!cc)
        return;
    if (cc->next == cc || cc->c == Hero::Get()) {
        return;
    }
    cc->prev->next = cc->next;
    cc->prev->next->prev = cc->prev;
    delete cc;
}

void SceneGame::DetachCreature(CreatureCircle* cc)
{
    DCHECK(cc != NULL);
    if (!cc) return;
    if (cc->next == cc || cc->c == Hero::Get()) {
        return;
    }
    cc->prev->next = cc->next;
    cc->prev->next->prev = cc->prev;
    delete cc;
}









