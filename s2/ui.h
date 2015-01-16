
#pragma once


// ncurses.
class game;
class ui
{
public:
    ui();
    virtual ~ui();

    int run();

private:
    void init();
    void uninit();
    int get_input();


    void draw_splash();
    void draw_main_menu();
    void draw_map(game& g, int x, int y, int z);
    void draw_overmap(game& g, int x, int y, int z);

    void init_colors();
};

