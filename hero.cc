

#include "hero.h"
#include "ui.h"
#include "dice.h"
#include "gamemap.h"
#include <stdlib.h>
#include <string>
#include <sstream>
#include "log.h"

static Hero* g_hero = NULL;

Hero::Hero()
    : Creature()
{
    type = "hero";
    name = "hero";
    SetSpeed(8);

    bool b = false;
    Creature* c;
    while (!b){
        x_ += dice(3000, 1) - 1500;
        y_ += dice(3000, 1) - 1500;
        Debug() << "trying (" << x_ << ", " << y_ << ")";
        b = MoveTo(x_, y_, c);
    }
    maxhp += 10;
    hp = maxhp;

}

Hero::~Hero()
{
}

Hero* Hero::Get()
{
    if (!g_hero)
        g_hero = new Hero();
    return g_hero;
}

bool Hero::turn(int turncount, bool& again)
{
    UI* ui = UI::Get();
    ui->ShowHelper("It's your turn. 'i' for inventory, 'hjkl' to move/melee.");

    int c = ui->GetInput();
    bool b;
    Creature* m = NULL;
    switch (c) {
    case 'q':
    case 'Q':
        return false;
        break;
    case 'h':
        b = Move(-1, 0, m);
        break;
    case 'l':
        b = Move(1, 0, m);
        break;
    case 'j':
        b = Move(0, 1, m);
        break;
    case 'k':
        b = Move(0, -1, m);
        break;
    case 'y':
        b = Move(-1, -1, m);
        break;
    case 'u':
        b = Move(1, -1, m);
        break;
    case 'b':
        b = Move(-1, 1, m);
        break;
    case 'n':
        b = Move(1, 1, m);
        break;
    case '\'':
        UI::Get()->ToggleFog();
        again = true;
        return true;
    case 'i':
        UI::Get()->ShowInventory();
        again = true;
        return true;
    case 'e':
        {
            ui->ShowHelper("which direction? h/j/k/l/.");
            int cc = 0;
            while (cc != 'h' && cc != 'j' && cc != 'k' && cc != 'l' && cc != '.' && cc != 27) {
                cc = ui->GetInput();
            }
            int xx = x(), yy = y();
            switch (cc) {
            case 'h':
                xx--;
                break;
            case 'j':
                yy++;
                break;
            case 'k':
                yy--;
                break;
            case 'l':
                xx++;
                break;
            case '.':
                break;
            case 27: 
                ui->ShowHelper("");
                again = true; 
                return true;
            default:
                break;
            }
            Map::Node* n = Map::Get()->at(xx, yy);
            if (n->items.empty()) {
                ui->ShowHelper("there is nothing in that direction.");
                ui->GetInput();
            } else {
                ui->ShowItemList(n->items);
            }
            again = true;
            return true;
        }
        break;
    default:
        b = false;
        break;
    }
    if (b) {
        std::stringstream   oss;
        oss << "You move to (" << x() << ", " << y() << ")";
        UI::Log(oss.str());
    } else {
        if (m) {
            Melee(m);
            UI::Get()->DrawCreatureAttacked(m);
        }
    }
    again = false;
    return true;
}

