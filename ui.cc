

#include "ui.h"
#include "cursesdef.h"
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include "log.h"
#include "gamemap.h"
#include "hero.h"
#include "color.h"
#include "item.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

static UI* g_ui = NULL;
UI* UI::Get(void)
{
    if (!g_ui)
        g_ui = new UI();
    return g_ui;
}

UI::UI()
{
    m_map_width = 50;
    m_map_height = 36;
    fog = true;
    g_ui = this;
}

UI::~UI()
{
}

void UI::init(void)
{
    initscr();
    noecho();
    cbreak();
    // raw();
    keypad(stdscr, true);
    curs_set(false);
    init_colors();
}

void UI::uninit(void)
{
    endwin();
    echo();
    curs_set(true);
}

int UI::GetInput(void)
{
    return getch();
}

void * UI::NewWin(std::string id, int x, int y, int w, int h)
{
    std::map<std::string, void*>::iterator  iter;
    iter = wins.find(id);
    if (iter != wins.end())
        return (iter->second);
    WINDOW* ww = newwin(h, w, y, x);
    if (!ww)
        return (void*)ww;
    wins[id] = (void*)ww;
    wborder(ww, '|', '|', '-', '-', '+', '+', '+', '+');
    box(ww, 0, 0);
    wrefresh(ww);
    return (void*)ww;
}

void UI::DelWin(std::string id)
{
    std::map<std::string, void*>::iterator  iter;
    iter = wins.find(id);
    if (iter != wins.end()) {
        wborder((WINDOW*)iter->second, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
        wrefresh((WINDOW*)iter->second);
        delwin((WINDOW*)iter->second);
        wins.erase(iter);
    }
}

void UI::ShowMessage(std::string msg)
{
    WINDOW * m = (WINDOW*)WinFromId("msgbox");
    if (!m)
        m = (WINDOW*)NewWin("msgbox", 10, 15, 60, 5);

    std::string space(58, ' ');
    mvwprintw(m, 2, 1, "%s", space.c_str());
    mvwprintw(m, 2, 1, "%s", msg.c_str());
    wrefresh(m);

}

void UI::ClearMessageBox(void)
{
    DelWin("msgbox");
}

void * UI::WinFromId(std::string id)
{
    std::map<std::string, void*>::iterator  iter;
    iter = wins.find(id);
    if (iter != wins.end()) {
        return iter->second;
    }
    return NULL;
}

void UI::refresh(void)
{
    ::refresh();
}

void UI::DrawMap()
{
    WINDOW * m = (WINDOW*)WinFromId("map");
    if (!m)
        m = (WINDOW*)NewWin("map", 0, 0, m_map_width, m_map_height);
    Map* mp = Map::Get();
    int x, y;
    for (x=-(m_map_width/2-1); x<(m_map_width/2-1); x++) {
        for (y=-(m_map_height/2-1); y<(m_map_height/2-1); y++) {
            Map::Node * n = mp->uiat(x, y);
            int xx = x + (m_map_width / 2);
            int yy = y + (m_map_height / 2);
            if (!fog || mp->IsNodeCanBeSeenByHero(n)) {
                if (n->creature) {
                    if (n->creature->HP() > 0) {
                        mvwaddch(m, yy, xx, n->creature->type[0]);
                        mvwchgat(m, yy, xx, 1, A_BOLD | A_COLOR , 6, NULL);
                    } else {
                        mvwaddch(m, yy, xx, ']');
                        mvwchgat(m, yy, xx, 1, A_BOLD | A_COLOR , 2, NULL);
                    }
                } else {
                    mvwaddch(m, yy, xx, n->icon);
                    if (n->type == Map::NODE_WATER) {
                        mvwchgat(m, yy, xx, 1, A_BOLD | A_COLOR , 4, NULL);
                    } else if (n->type == Map::NODE_VOLCANO) {
                        mvwchgat(m, yy, xx, 1, A_BOLD | A_COLOR , 2, NULL);
                    } else if (n->type == Map::NODE_TREE) {
                        mvwchgat(m, yy, xx, 1, A_NORMAL | A_COLOR , 3, NULL);
                    } else {
                        mvwchgat(m, yy, xx, 1, A_NORMAL | A_COLOR , 1, NULL);
                    }
                }
                if (!n->creature && !n->items.empty()) {
                    mvwaddch(m, yy, xx, '{');
                    mvwchgat(m, yy, xx, 1, A_BOLD | A_COLOR , 7, NULL);
                }
            } else {
                mvwaddch(m, yy, xx, '#');
            }
        }
    }
    wrefresh(m);
}

void UI::DrawHero(void)
{
    WINDOW * m = (WINDOW*)WinFromId("map");
    if (!m)
        m = (WINDOW*)NewWin("map", 0, 0, m_map_width, m_map_height);
    mvwaddch(m, m_map_height/2, m_map_width/2, '@');
    mvwchgat(m, m_map_height / 2, m_map_width / 2, 1, A_BOLD | A_COLOR, 5, NULL);
    wrefresh(m);
}

void UI::DrawCreatureAttacked(Creature* c)
{
    DCHECK(c != NULL);
    if (!c)
        return;
    WINDOW * w = (WINDOW*)WinFromId("map");
    if (!w)
        return;
    Map* m = Map::Get();
    int y = m_map_height / 2 + c->y() - m->HeroY();
    int x = m_map_width / 2 + c->x() - m->HeroX();
    // Map::Node* n = m->at(c->x(), c->y());
    mvwchgat(w, y, x, 1, A_BOLD | A_BLINK, 2, NULL);
    wrefresh(w);
#ifdef _WIN32
    Sleep(1);
#else
    usleep(100000);
#endif
    mvwchgat(w, y, x, 1, A_BOLD | A_BLINK, 3, NULL);
    wrefresh(w);
}

void UI::DrawHeroStatus(void)
{
    WINDOW * m = (WINDOW*)WinFromId("status");
    if (!m)
        m = (WINDOW*)NewWin("status", m_map_width + 27, 0, 25, 10);
    Hero* h = Hero::Get();
    mvwprintw(m, 1, 1, "NAME (%d/%d)", h->HP(), h->MAXHP());
    mvwprintw(m, 2, 1, "    HERO    MOD   TOTAL");
    mvwprintw(m, 3, 1, "STR  %3d    %3d     %3d", h->attr.Str(), h->attrmod.Str(), h->Str());
    mvwprintw(m, 4, 1, "DEX  %3d    %3d     %3d", h->attr.Dex(), h->attrmod.Dex(), h->Dex());
    mvwprintw(m, 5, 1, "CON  %3d    %3d     %3d", h->attr.Con(), h->attrmod.Con(), h->Con());
    mvwprintw(m, 6, 1, "WIS  %3d    %3d     %3d", h->attr.Wis(), h->attrmod.Wis(), h->Wis());
    mvwprintw(m, 7, 1, "INT  %3d    %3d     %3d", h->attr.Int(), h->attrmod.Int(), h->Int());
    mvwprintw(m, 8, 1, "CHR  %3d    %3d     %3d", h->attr.Chr(), h->attrmod.Chr(), h->Chr());
    wrefresh(m);
}

void UI::Log(std::string log)
{
    UI::Get()->ShowLog(log);
}

void UI::ShowLog(std::string log)
{
    WINDOW * m = (WINDOW*)WinFromId("log");
    if (!m)
        m = (WINDOW*)NewWin("log", m_map_width + 0, m_map_height - 10, 52, 10);

    logs.push_back(log);
    if (logs.size() > 20) {
        size_t count = logs.size() - 20;
        size_t i;
        for (i=0; i<count; i++)
            logs.pop_front();
    }

    if (logs.size() > 0) {
        int start = logs.size() - 8;
        if (start < 0)
            start = 0;
        std::list<std::string>::iterator    it = logs.begin();;
        int i;
        for (i=0; i<start; i++) {
            it++;
            if (it == logs.end())
                break;
        }
        i = 1;
        std::string space(50, ' ');
        while (it != logs.end()) {
            mvwprintw(m, i, 1, "%s", space.c_str());
            mvwprintw(m, i, 1, "%s", (*it).c_str());
            it++;
            i++;
        }
    }
    wrefresh(m);
}

void UI::ShowHelper(std::string helper)
{
    WINDOW* m = (WINDOW*)WinFromId("helper");
    if (!m)
        m = (WINDOW*)NewWin("helper", 0, m_map_height, m_map_width + 52, 3);
    std::string space(m_map_width + 50, ' ');
    mvwprintw(m, 1, 1, "%s", space.c_str());
    mvwprintw(m, 1, 1, helper.c_str());
    wrefresh(m);
}

void UI::Sleep(int ms)
{
#ifdef _WIN32
#else
    usleep(ms * 1000);
#endif
}

void UI::DrawInventory(int index, int highlight)
{
    WINDOW * m = (WINDOW*)WinFromId("inventory");
    if (!m)
        m = (WINDOW*)NewWin("inventory", m_map_width + 0, 0, 27, m_map_height - 10);

    mvwprintw(m, 1, 1, "WEARING");
    mvwprintw(m, 2, 1, "头:                      ");
    mvwprintw(m, 3, 1, "胸:                      ");
    mvwprintw(m, 4, 1, "腿:                      ");
    mvwprintw(m, 5, 1, "脚:                      ");
    mvwprintw(m, 6, 1, "颈:                      ");
    mvwprintw(m, 7, 1, "指:                      ");
    mvwprintw(m, 8, 1, "手:                      ");
    mvwhline(m, 9, 1, 0, 25);

    Hero* h = Hero::Get();
    std::list<Item*>::iterator   it;
    for (it = h->inventory.begin(); it != h->inventory.end(); it++) {
        Item* item = (*it);
        if (item->Enabled()) {
            if (item->type <= Item::ITEM_HAND) {
                int y = (int)item->type + 2;
                mvwprintw(m, y, 5, "%s", item->name.c_str());
            }
        }
    }


    int i = 0;
    it = h->inventory.begin();
    for (i=0; i<index; i++) {
        if (it == h->inventory.end())
            break;
        it++;
    }

    std::string space(25, ' ');
    for (i=0; i < m_map_height - 21; i++) {
        mvwprintw(m, 10 + i, 1, "%s", space.c_str());
        if (it != h->inventory.end()) {
            if ((*it)->Enabled()) {
                mvwaddch(m, 10 + i, 1, '*');
            } else {
                mvwaddch(m, 10 + i, 1, ' ');
            }
            mvwprintw(m, 10 + i, 2, "%s", (*it)->name.c_str());
            if (highlight == i) {
                mvwchgat(m, 10 + i, 2, 24, A_BOLD | A_COLOR , 3, NULL);
            } else {
                mvwchgat(m, 10 + i, 2, 24, A_NORMAL | A_COLOR , 1, NULL);
            }
            it++;
        }
    }
    wrefresh(m);
}

void UI::ShowInventory(void)
{
    int showindex = 0;
    int selindex = 0;
    int count = m_map_height - 22;
    Hero* h = Hero::Get();

    WINDOW * m = (WINDOW*)WinFromId("inventory");
    if (!m)
        m = (WINDOW*)NewWin("inventory", m_map_width + 0, 0, 27, m_map_height - 10);
    
    ShowHelper("press 'w' to put on/off.");
    Item* last = NULL;
    while (true) {
        int y = selindex - showindex;
        DrawInventory(showindex, y);
        Item* curr = NULL;
        std::list<Item*>::iterator it = h->inventory.begin();
        for (int i=0; i < selindex;  i++) {
            it++;
            if (it == h->inventory.end())
                break;
        }
        if (it != h->inventory.end()) {
            curr = *it;
            if (curr != last || last == NULL)
                ShowLog(curr->name);
            last = curr;
        }

        int c = GetInput();
        switch (c) {
        case 27:
        case 'q':
        case 'i':
            return;
        case 'j':
            if (y >= count) {
                if (showindex < (int)h->inventory.size() - count - 1) {
                    showindex++;
                    selindex++;
                }
            } else {
                if (selindex < (int)h->inventory.size() - 1)
                    selindex++;
            }
            break;
        case 'k':
            if (y <= 0) {
                if (showindex > 0) {
                    showindex --;
                    selindex --;
                }
            } else {
                if (selindex > 0)
                    selindex --;
            }
            break;
        case 'w':
            if (curr) {
                if (curr->type <= Item::ITEM_HAND)
                    h->Equip(curr);
            }
            break;
        default:
            break;
        }
    }
}

void UI::ShowItemList(std::list<Item*>& l)
{
    int height = m_map_height - 20;
    int width = 25;
    int left = m_map_width + 27;
    int top = 10;

    WINDOW * m = (WINDOW*)WinFromId("info");
    if (!m)
        m = (WINDOW*)NewWin("info", left, top, width, height);

    int showindex = 0;
    int selindex = 0;
    while (true) {
        int y = selindex - showindex;
        Item* curr = NULL;
        
        std::string space(width - 2, ' ');
        int i;
        std::list<Item*>::iterator  it = l.begin();
        for (i=0; i<showindex; i++) {
            if (it != l.end())
                it++;
        }
        for (i = 1; i < height - 1; i++) {
            mvwprintw(m, i, 1, "%s", space.c_str());
            if (it != l.end()) {
                if ((*it)->Enabled()) {
                    mvwaddch(m, i, 1, '*');
                } else {
                    mvwaddch(m, i, 1, ' ');
                }
                mvwprintw(m, i, 2, "%s", (*it)->name.c_str());
                if (i-1 == y) {
                    mvwchgat(m, i, 2, width - 3, A_BOLD | A_COLOR , 3, NULL);
                    curr = *it;
                } else {
                    mvwchgat(m, i, 2, width - 3, A_NORMAL | A_COLOR , 1, NULL);
                }
                it++;
            }
        }
        wrefresh(m);
        if (l.size() == 0)
            break;

        int c = GetInput();
        switch (c) {
        case 'j':
            if (y >= height - 3) {
                if (showindex < (int)l.size() - height - 1) {
                    showindex++;
                    selindex++;
                }
            } else {
                if (selindex < (int)l.size() - 1)
                    selindex++;
            }
            break;
        case 'k':
            if (y <= 0) {
                if (showindex > 0) {
                    showindex--;
                    selindex--;
                }
            } else {
                if (selindex > 0)
                    selindex--;
            }
            break;
        case ' ':
            if (curr) {
                curr->Enable(!curr->Enabled());
            }
            break;
        case 27:
        case 'q':
            for (i = 1; i < height - 1; i++) {
                mvwprintw(m, i, 1, "%s", space.c_str());
            }
            wrefresh(m);
            return;
        case 10:
            {
                Hero* h = Hero::Get();
                bool flag = true;
                while (flag) {
                    flag = false;
                    for (it = l.begin(); it != l.end(); it++) {
                        if ((*it)->Enabled()) {
                            h->Give(new Item(*(*it)));
                            delete *it;
                            l.erase(it);
                            flag = true;
                            break;
                        }
                    }
                }
                DrawInventory();
                showindex = 0;
                selindex = 0;
            }
            break;
        default:
            {
                std::stringstream   oss;
                oss << "Press " << c;
                ShowLog(oss.str());
            }
            break;
        };
    }

}


















