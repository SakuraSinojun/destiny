
#include <ncurses.h>

#include "ui.h"
#include "game.h"
#include "tools/logging.h"


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

enum nc_color {
    // {{{
    c_black   = COLOR_PAIR(30),
    c_white   = COLOR_PAIR(1)  | A_BOLD,
    c_ltgray  = COLOR_PAIR(1),
    c_dkgray  = COLOR_PAIR(30) | A_BOLD,
    c_red     = COLOR_PAIR(2),
    c_green   = COLOR_PAIR(3),
    c_blue    = COLOR_PAIR(4),
    c_cyan    = COLOR_PAIR(5),
    c_magenta = COLOR_PAIR(6),
    c_brown   = COLOR_PAIR(7),
    c_ltred   = COLOR_PAIR(2)  | A_BOLD,
    c_ltgreen = COLOR_PAIR(3)  | A_BOLD,
    c_ltblue  = COLOR_PAIR(4)  | A_BOLD,
    c_ltcyan  = COLOR_PAIR(5)  | A_BOLD,
    c_pink    = COLOR_PAIR(6)  | A_BOLD,
    c_yellow  = COLOR_PAIR(7)  | A_BOLD,

    h_black   = COLOR_PAIR(20),
    h_white   = COLOR_PAIR(15) | A_BOLD,
    h_ltgray  = COLOR_PAIR(15),
    h_dkgray  = COLOR_PAIR(20) | A_BOLD,
    h_red     = COLOR_PAIR(16),
    h_green   = COLOR_PAIR(17),
    h_blue    = COLOR_PAIR(20),
    h_cyan    = COLOR_PAIR(19),
    h_magenta = COLOR_PAIR(21),
    h_brown   = COLOR_PAIR(22),
    h_ltred   = COLOR_PAIR(16) | A_BOLD,
    h_ltgreen = COLOR_PAIR(17) | A_BOLD,
    h_ltblue  = COLOR_PAIR(18) | A_BOLD,
    h_ltcyan  = COLOR_PAIR(19) | A_BOLD,
    h_pink    = COLOR_PAIR(21) | A_BOLD,
    h_yellow  = COLOR_PAIR(22) | A_BOLD,

    i_black   = COLOR_PAIR(30),
    i_white   = COLOR_PAIR(8)  | A_BLINK,
    i_ltgray  = COLOR_PAIR(8),
    i_dkgray  = COLOR_PAIR(30) | A_BLINK,
    i_red     = COLOR_PAIR(9),
    i_green   = COLOR_PAIR(10),
    i_blue    = COLOR_PAIR(11),
    i_cyan    = COLOR_PAIR(12),
    i_magenta = COLOR_PAIR(13),
    i_brown   = COLOR_PAIR(14),
    i_ltred   = COLOR_PAIR(9)  | A_BLINK,
    i_ltgreen = COLOR_PAIR(10) | A_BLINK,
    i_ltblue  = COLOR_PAIR(11) | A_BLINK,
    i_ltcyan  = COLOR_PAIR(12) | A_BLINK,
    i_pink    = COLOR_PAIR(13) | A_BLINK,
    i_yellow  = COLOR_PAIR(14) | A_BLINK,

    c_white_red   = COLOR_PAIR(23) | A_BOLD,
    c_ltgray_red  = COLOR_PAIR(23),
    c_dkgray_red  = COLOR_PAIR(9),
    c_red_red     = COLOR_PAIR(9),
    c_green_red   = COLOR_PAIR(25),
    c_blue_red    = COLOR_PAIR(26),
    c_cyan_red    = COLOR_PAIR(27),
    c_magenta_red = COLOR_PAIR(28),
    c_brown_red   = COLOR_PAIR(29),
    c_ltred_red   = COLOR_PAIR(24) | A_BOLD,
    c_ltgreen_red = COLOR_PAIR(25) | A_BOLD,
    c_ltblue_red  = COLOR_PAIR(26) | A_BOLD,
    c_ltcyan_red  = COLOR_PAIR(27) | A_BOLD,
    c_pink_red    = COLOR_PAIR(28) | A_BOLD,
    c_yellow_red  = COLOR_PAIR(29) | A_BOLD
    // }}}
};

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
    int x = 570;
    int y = -147;
    int z = 0;
    // for (int j = 0; j != y; j--) {
    //     for (int i = 0; i != x; i++) {
    //         point   pt(i, j);
    //         pt = map::x2sm(pt);
    //         g.m().get_overmap(pt.x, pt.y);
    //     }
    // }
    x = 1667;
    y = 517;
    while (input != 'q') {
        draw_map(g, x, y, z);
        draw_overmap(g, x, y, z);

        mvprintw(42, 102, "(%d, %d, %d)", x, y, z);

        point pt(x, y);
        pt = map::x2sm(pt);
        mvprintw(42, 0, "s: %d o: %d | s: %d o: %d", 
            g.m().get_ster(x, y, z).type, g.m().get_oter(pt.x, pt.y, z).type,
            g.m().get_submap_list_size(), g.m().get_overmap_list_size()
            );

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
        if (input == 'r') {
            submap* m = g.m().get_current_submap(x, y);
            m->rotate(1);
        }
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
    init_colors();
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

void ui::draw_map(game& g, int x, int y, int z)
{/*{{{*/
    for (int i = x - 50; i <= x + 50; i++) {
        for (int j = y - 20; j <= y + 20; j++) {
            submap::ster s = g.m().get_ster(i, j, z);
            int cx = i - (x - 50);
            int cy = j - (y - 20);

            nc_color color = c_white;
 
            int ch = ' ';
            if (s == submap::st_river) {
                ch = '~';
                color = c_blue;
            } else if (s == submap::st_water_sh) {
                ch = '~';
                color = c_ltblue;
            } else if (s == submap::st_water_dp) {
                ch = '~';
                color = c_blue;
            } else if (s == submap::st_sidewalk) {
                ch = '.';
                color = c_ltgray;
            } else if (s == submap::st_pavement) {
                ch = '.';
                color = c_dkgray;
            } else if (s == submap::st_pavement_y) {
                ch = '.';
                color = c_yellow;
            } else if (s == submap::st_road) {
                ch = '#';
            } else if (s == submap::st_house) {
                ch = 'H';
            } else if (s == submap::st_rock) {
                ch = '*';
            } else if (s == submap::st_dirt) {
                ch = '.';
                color = c_brown;
            } else if (s == submap::st_grass) {
                ch = '.';
                color = c_green;
            } else if (s >= submap::st_shrub && s <= submap::st_shrub_strawberry) {
                ch = '#';
            } else if (s == submap::st_tree) {
                ch = 'T';
                color = c_green;
            } else if (s == submap::st_tree_young) {
                ch = 'T';
                color = c_ltgreen;
            } else if (s == submap::st_tree_apple) {
                ch = 'T';
                color = c_red;
#if 0
            } else if (s == submap::st_wall_h) {
                ch = LINE_OXOX;
                color = c_white;
            } else if (s == submap::st_wall_v) {
                ch = LINE_XOXO;
                color = c_white;
#endif
            } else if (s == submap::st_wall_h || s == submap::st_wall_v) {
                bool t = g.m().get_ster(i, j - 1, z).is_wall_or_door();
                bool b = g.m().get_ster(i, j + 1, z).is_wall_or_door();
                bool l = g.m().get_ster(i - 1, j, z).is_wall_or_door();
                bool r = g.m().get_ster(i + 1, j, z).is_wall_or_door();
                if (t && b && l && r)
                    ch = LINE_XXXX;
                else if (t && b && l)
                    ch = LINE_XOXX;
                else if (t && b && r)
                    ch = LINE_XXXO;
                else if (t && l && r)
                    ch = LINE_XXOX;
                else if (b && l && r)
                    ch = LINE_OXXX;
                else if (t && r)
                    ch = LINE_XXOO;
                else if (t && b)
                    ch = LINE_XOXO;
                else if (t && l)
                    ch = LINE_XOOX;
                else if (r && b)
                    ch = LINE_OXXO;
                else if (r && l)
                    ch = LINE_OXOX;
                else if (b && l)
                    ch = LINE_OOXX;
                else if (t || b)
                    ch = LINE_XOXO;
                else if (r || l)
                    ch = LINE_OXOX;
                else if (s == submap::st_wall_h)
                    ch = LINE_OXOX;
                else
                    ch = LINE_XOXO;
                color = c_white;
            } else if (s == submap::st_floor) {
                ch = '.';
                color = c_cyan;
            } else if (s == submap::st_door_c) {
                ch = '+';
                color = c_brown;
            } else if (s == submap::st_door_o) {
                ch = '+';
                color = c_brown;
            } else if (s == submap::st_door_locked) {
                ch = '+';
                color = c_brown;
            } else if (s == submap::st_window) {
                ch = '"';
                color = c_blue;
            }

            attron(color);
            mvaddch(cy, cx, ch);
            attroff(color);
            // mvputch(cy, cx, color, ch);
            // mvaddch(cy, cx, ch);
            // mvchgat(cy, cx, 1, A_NORMAL, color, NULL);
        }
    }
    mvaddch(20, 50, '@');
    // mvchgat(20, 50, 1, A_NORMAL, color_to_int(c_green), NULL);

}/*}}}*/

void ui::draw_overmap(game& g, int x, int y, int z)
{/*{{{*/
    // overmap
    point smp = map::x2sm(point(x, y));
    for (int i = smp.x - 10; i <= smp.x + 10; i++) {
        for (int j = smp.y - 10; j <= smp.y + 10; j++) {
            overmap::oter o = g.m().get_oter(i, j, z);

            int cx = i - (smp.x - 10) + 103;
            int cy = j - (smp.y - 10);

            int ch = '.';
            if (o == overmap::ot_forest_water)
                ch = 'R';
            else if (o.is_water())
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
            else
                ch = '.';

            mvaddch(cy, cx, ch);
        }
    }
    mvaddch(10, 113, '@');
}/*}}}*/


void ui::init_colors()
{/*{{{*/
    start_color();

#define HILIGHT COLOR_BLUE
    init_pair( 1, COLOR_WHITE,   COLOR_BLACK  );
    init_pair( 2, COLOR_RED,     COLOR_BLACK  );
    init_pair( 3, COLOR_GREEN,   COLOR_BLACK  );
    init_pair( 4, COLOR_BLUE,    COLOR_BLACK  );
    init_pair( 5, COLOR_CYAN,    COLOR_BLACK  );
    init_pair( 6, COLOR_MAGENTA, COLOR_BLACK  );
    init_pair( 7, COLOR_YELLOW,  COLOR_BLACK  );

    // Inverted Colors
    init_pair( 8, COLOR_BLACK,   COLOR_WHITE  );
    init_pair( 9, COLOR_BLACK,   COLOR_RED    );
    init_pair(10, COLOR_BLACK,   COLOR_GREEN  );
    init_pair(11, COLOR_BLACK,   COLOR_BLUE   );
    init_pair(12, COLOR_BLACK,   COLOR_CYAN   );
    init_pair(13, COLOR_BLACK,   COLOR_MAGENTA);
    init_pair(14, COLOR_BLACK,   COLOR_YELLOW );

    // Highlighted - blue background
    init_pair(15, COLOR_WHITE,   HILIGHT);
    init_pair(16, COLOR_RED,     HILIGHT);
    init_pair(17, COLOR_GREEN,   HILIGHT);
    init_pair(18, COLOR_BLUE,    HILIGHT);
    init_pair(19, COLOR_CYAN,    HILIGHT);
    init_pair(20, COLOR_BLACK,   HILIGHT);
    init_pair(21, COLOR_MAGENTA, HILIGHT);
    init_pair(22, COLOR_YELLOW,  HILIGHT);

    // Red background - for monsters on fire
    init_pair(23, COLOR_WHITE,   COLOR_RED);
    init_pair(24, COLOR_RED,     COLOR_RED);
    init_pair(25, COLOR_GREEN,   COLOR_RED);
    init_pair(26, COLOR_BLUE,    COLOR_RED);
    init_pair(27, COLOR_CYAN,    COLOR_RED);
    init_pair(28, COLOR_MAGENTA, COLOR_RED);
    init_pair(29, COLOR_YELLOW,  COLOR_RED);

    init_pair(30, COLOR_BLACK,   COLOR_BLACK  );
}/*}}}*/


