
#include <ncurses.h>

#include "ui.h"
#include "game.h"


//      LINE_NESW  - X for on, O for off
#define LINE_XOXO 4194424
#define LINE_OXOX 4194417
#define LINE_XXOO 4194413
#define LINE_OXXO 4194412
#define LINE_OOXX 4194411
#define LINE_XOOX 4194410
#define LINE_XXXO 4194420
#define LINE_XXOX 4194422
#define LINE_XOXX 4194421
#define LINE_OXXX 4194423
#define LINE_XXXX 4194414

ui::ui()
{
}

ui::~ui()
{
    uninit();
}

int ui::run()
{
#if 0
    {
        game g;
        int x = 0;
        int y = 0;
        for (int j = y - 20; j < y + 20; j++) {
            for (int i = x - 65; i < x + 65; i++) {
                overmap::oter o = g.m().get_terrain(i, j);
                // int cx = i - (x - 20);
                // int cy = j - (y - 20);
                int ch = '.';
                if (o == overmap::ot_river_center)
                    ch = '~';
                else if (o == overmap::ot_rock)
                    ch = '*';
                else
                    ch = '.';
                printf("%c", ch);
            }
            printf("\n");
        }

        return 0;
    }
#endif

    init();
    // draw_splash();
    // draw_main_menu();

    game    g;
    g.load("test");

    int input = 0;
    int x = 0;
    int y = 0;
    int z = 0;
    while (input != 'q') {
        for (int i = x - 50; i <= x + 50; i++) {
            for (int j = y - 20; j <= y + 20; j++) {
                overmap::oter o = g.m().get_terrain(i, j, z);

                int cx = i - (x - 50);
                int cy = j - (y - 20);
                int ch = '.';
                if (o.is_water())
                    ch = '~';
                else if (o == overmap::ot_rock)
                    ch = '*';
                else if (o == overmap::ot_road_ns)
                    ch = LINE_XOXO;
                else if (o == overmap::ot_road_ew)
                    ch = LINE_OXOX;
                else if (o == overmap::ot_road_ne)
                    ch = LINE_XXOO;
                else if (o == overmap::ot_road_es)
                    ch = LINE_OXXO;
                else if (o == overmap::ot_road_sw)
                    ch = LINE_OOXX;
                else if (o == overmap::ot_road_wn)
                    ch = LINE_XOOX;
                else if (o == overmap::ot_road_nes)
                    ch = LINE_XXXO;
                else if (o == overmap::ot_road_new)
                    ch = LINE_XXOX;
                else if (o == overmap::ot_road_nsw)
                    ch = LINE_XOXX;
                else if (o == overmap::ot_road_esw)
                    ch = LINE_OXXX;
                else if (o == overmap::ot_road_nesw)
                    ch = LINE_XXXX;
                else if (o == overmap::ot_road_null)
                    ch = '#';
                else if (o == overmap::ot_bridge_ns)
                    ch = LINE_XOXO;
                else if (o == overmap::ot_bridge_ew)
                    ch = LINE_OXOX;
                else if (o == overmap::ot_shop)
                    ch = 'S';
                else if (o == overmap::ot_house_north)
                    ch = '^';
                else if (o == overmap::ot_house_south)
                    ch = 'v';
                else if (o == overmap::ot_house_west)
                    ch = '<';
                else if (o == overmap::ot_house_east)
                    ch = '>';
                else if (o == overmap::ot_park)
                    ch = '#';
                else if (o == overmap::ot_forest)
                    ch = 'f';
                else if (o == overmap::ot_forest_thick)
                    ch = 'F';
                else if (o == overmap::ot_forest_water)
                    ch = 'R';
                else
                    ch = '.';

                mvaddch(cy, cx, ch);
            }
        }
        mvaddch(20, 50, '@');

        mvprintw(2, 102, "(%d, %d, %d)", x, y, z);
        refresh();

        input = get_input();
        if (input == 'h')
            x--;
        if (input == 'l')
            x++;
        if (input == 'j')
            y++;
        if (input == 'k')
            y--;
        if (input == '>')
            z++;
        if (input == '<')
            z--;
    }

    uninit();
    return 0;
}

void ui::init()
{
    initscr();
    noecho();
    cbreak();
    // raw();
    keypad(stdscr, true);
    curs_set(false);
    // start_color();
}

void ui::uninit()
{
    endwin();
    echo();
    curs_set(true);
}

int ui::get_input()
{
    return getch();
}

void ui::draw_splash()
{
    printw("splash.\n");
    get_input();
}

void ui::draw_main_menu()
{
    printw("main menu.\n");
    get_input();
}





