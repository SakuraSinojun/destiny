
#pragma once


// ncurses.
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
};

