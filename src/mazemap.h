#pragma once

#include <vector>
#include <string>

class MazeMap
{
    public:
        MazeMap(std::vector<std::string>&& blocks_);
        float get_center_y() const;
        float get_center_x() const;
        bool is_wall_block(const int y, const int x, char& wall_char) const;

        const std::vector<std::string> blocks;
        const unsigned int height;
        const unsigned int width;
        const float max_distance;
};


