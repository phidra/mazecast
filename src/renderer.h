#pragma once

#include <string>
#include <vector>
#include <math.h>

class Player;
class MazeMap;
class Screen;

class Renderer
{
    public:
        Renderer(MazeMap& the_map_, Player& player_, Screen& screen_);
        bool loop();

    private:
        float compute_column_angle(int current_col) const;
        void compute_room_thresholds(const float& distance_from_player, int& ceiling_threshold, int& floor_threshold) const;
        float cast_ray(const int current_col, char& wall_char) const;
        std::string compute_wall_pixel(float distance_from_player) const;
        std::string compute_floorceil_pixel(const int current_line) const;
        void draw_line_pixel(
            Screen& screen,
            const int current_line,
            const int current_col,
            const float distance_from_player,
            const char wall_char,
            const int ceiling_threshold,
            const int floor_threshold) const;
        void render(Screen& screen);
        float get_distance(const int col) const;

        Player& player;
        const MazeMap& mazemap;
        Screen& screen;
        std::vector<float> distance_by_col;
};

