#pragma once

#include <vector>
#include <string>
#include <memory>


class Player;
class MazeMap;


class Screen
{
    private:
        bool debug_mode = false;
        class Curses;
        std::unique_ptr<Curses> curses;

    public:
        Screen();
        ~Screen();
        void draw_pixel(const int line_index, const int col_index, std::string pixel, const unsigned int color);
        void draw_additions(const Player& player, const MazeMap& mazemap, const std::vector<std::string>& debug_lines, const std::vector<std::string>& instructions);
        void refresh();
        void toggle_debug_mode();

        const unsigned int FLOOR_COLOR = 1;
        const unsigned int CEIL_COLOR = 1;
        const unsigned int BLUE_WALL_COLOR = 2;
        const unsigned int RED_WALL_COLOR = 3;
        const unsigned int GREEN_WALL_COLOR = 4;
        const unsigned int YELLOW_WALL_COLOR = 5;
        const unsigned int CYAN_WALL_COLOR = 6;
        const unsigned int MAGENTA_WALL_COLOR = 7;
        const unsigned int MAGIC_WALL_COLOR = 8;

        const int nb_lines = 0;
        const int nb_cols = 0;
};
