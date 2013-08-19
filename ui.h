
#pragma once

#include <string>
#include <map>
#include <list>

class Map;
class Hero;
class Creature;
class Item;
class UI {
private:
    UI();
public:
    static UI* Get(void);
    ~UI();
    static void init(void);
    static void uninit(void);

    int  GetInput(void);

    void * NewWin(std::string id, int x, int y, int w, int h);
    void DelWin(std::string id);

    void ShowMessage(std::string msg);
    void ClearMessageBox(void);

    void refresh(void);

    // map
    void ToggleFog(void) { fog = !fog; }
    void DrawMap(void);
    void DrawHero(void);
    void DrawHeroStatus(void);
    void DrawCreatureAttacked(Creature* c);

    // views
    void DrawInventory(int index = 0, int highlight = -1);
    void ShowInventory(void);
    void ShowItemList(std::list<Item*>& l);

    // helper 
    static void Log(std::string log);
    void Sleep(int ms);

    void ShowHelper(std::string helper);

private:
    void ShowLog(std::string log);

    std::map<std::string, void*> wins;
    void * WinFromId(std::string id);

    int m_map_width;
    int m_map_height;

    std::list<std::string>  logs;
    bool fog;
};


