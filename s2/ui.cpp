
#include <ncurses.h>

#include "ui.h"
#include "game.h"

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
    while (input != 'q') {
        for (int i = x - 50; i < x + 50; i++) {
            for (int j = y - 20; j < y + 20; j++) {
                overmap::oter o = g.m().get_terrain(i, j);

                int cx = i - (x - 20);
                int cy = j - (y - 20);
                int ch = '.';
                if (o == overmap::ot_river_center)
                    ch = '~';
                else if (o == overmap::ot_rock)
                    ch = '*';
                else
                    ch = '.';

                mvaddch(cy, cx, ch);
            }
        }
        mvaddch(21, 21, '@');

        input = get_input();
        if (input == 'h')
            x--;
        if (input == 'l')
            x++;
        if (input == 'j')
            y++;
        if (input == 'k')
            y--;
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
    start_color();
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





