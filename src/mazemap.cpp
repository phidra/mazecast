#include <algorithm>
#include <cassert>

#include "mazemap.h"

using namespace std;


MazeMap::MazeMap(vector<string>&& blocks_) :
    blocks(blocks_),
    height(blocks.size()),
    width(blocks.at(0).size()),
    max_distance(sqrt(float(width*width + height*height)))
{
    bool are_blocks_consistent = all_of(blocks.begin(), blocks.end(), [this](const string& line) { return line.size() == this->width; });
    assert(are_blocks_consistent);
}


float MazeMap::get_center_y() const
{ 
    return height / 2;
}


float MazeMap::get_center_x() const
{ 
    return width / 2;
}


bool MazeMap::is_wall_block(const int y, const int x, char& wall_char) const
{
    const auto& map_line = blocks.at(y);
    wall_char = map_line.at(x);
    return wall_char != '.';
}
