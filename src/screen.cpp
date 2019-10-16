#include <ncurses.h>

#include "screen.h"
#include "player.h"
#include "mazemap.h"

using namespace std;

class Screen::Curses
{
    public:
        Curses()
        {
            setlocale(LC_ALL, "");
            initscr();
            noecho();
            curs_set(FALSE);
            keypad(stdscr, TRUE);
            getmaxyx(stdscr, nb_lines, nb_cols);
            mousemask(ALL_MOUSE_EVENTS, NULL);
            start_color();
        }
        ~Curses()
        {
            endwin();
        }
        int nb_lines = 0;
        int nb_cols = 0;
};


Screen::Screen() : curses{new Screen::Curses{}}, nb_lines{curses->nb_lines}, nb_cols{curses->nb_cols}
{
    init_pair(FLOOR_COLOR,         COLOR_BLACK, COLOR_WHITE);
    init_pair(BLUE_WALL_COLOR,     COLOR_BLACK, COLOR_BLUE);
    init_pair(GREEN_WALL_COLOR,    COLOR_BLACK, COLOR_GREEN);
    init_pair(RED_WALL_COLOR,      COLOR_BLACK, COLOR_RED);
    init_pair(YELLOW_WALL_COLOR,   COLOR_BLACK, COLOR_YELLOW);
    init_pair(MAGENTA_WALL_COLOR,  COLOR_BLACK, COLOR_MAGENTA);
    init_pair(CYAN_WALL_COLOR,     COLOR_BLACK, COLOR_CYAN);
    init_pair(MAGIC_WALL_COLOR,    COLOR_WHITE, COLOR_RED);

    // See available colors in /usr/include/ncursesw/ncurses.h
}

Screen::~Screen() = default;

void Screen::draw_pixel(const int line_index, const int col_index, string pixel, const unsigned int color)
{
    attron(COLOR_PAIR(color));
    mvprintw(line_index, col_index, pixel.c_str());
    attroff(COLOR_PAIR(color));
}


void Screen::draw_additions(const Player& player, const MazeMap& mazemap, const vector<string>& debug_lines, const vector<string>& instructions)
{
    if (debug_mode)
    {
        // adding debug lines :
        unsigned int index = 0;
        for (const auto& debug_line: debug_lines)
        {
            mvprintw(index++, 0, debug_line.c_str());
        }

        // adding map :
        const int map_begin = debug_lines.size() + 1;
        int map_index = map_begin;
        for(vector<string>::const_iterator blockline = mazemap.blocks.begin(); blockline != mazemap.blocks.end(); ++blockline)
        {
            mvprintw(map_index++, 0, blockline->c_str());
        }

        // adding player position on map :
        mvprintw(map_begin + player.get_pos_y(), player.get_pos_x(), player.get_player_representation().c_str());
    }

    // adding game instructions
    unsigned int line = 0;
    for (const auto& instruction: instructions)
    {
        int col = (nb_cols / 2) - (instruction.size() / 2);
        mvprintw(line++, col, instruction.c_str());
    }
}


void Screen::refresh()
{
    ::refresh();
}


void Screen::toggle_debug_mode()
{
    debug_mode = !debug_mode;
}
