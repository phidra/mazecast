#include <cassert>
#include <math.h>

#include "player.h"
#include "mazemap.h"

using namespace std;


enum class MoveDirection {forward, backward, strafe_left, strafe_right };


static const float HALF_PI_OVER_4 = M_PI / 8;


Player::Player(const MazeMap& mazemap_) :
    mazemap(mazemap_),
    pos_y(mazemap.get_center_y()),
    pos_x(mazemap.get_center_x()),
    angle(0)
{
}


void Player::move_forward() { move_position(MoveDirection::forward); }
void Player::move_backward() { move_position(MoveDirection::backward); }
void Player::strafe_left() { move_position(MoveDirection::strafe_left); }
void Player::strafe_right() { move_position(MoveDirection::strafe_right); }


void Player::turn_left()
{
    angle -= ANGLE_INCREMENT;
    fit_angle();
}


void Player::turn_right()
{
    angle += ANGLE_INCREMENT;
    fit_angle();
}

void Player::fit_angle()
{
    if (angle > M_PI)
    {
        angle -= (2 * M_PI);
    }

    else if (angle < -M_PI)
    {
        angle += (2 * M_PI);
    }
}


void Player::move_position(const MoveDirection& direction)
{
    float increment_y = (POS_INCREMENT * sin(angle));
    float increment_x = (POS_INCREMENT * cos(angle));
    float new_pos_y;
    float new_pos_x;

    if (direction == MoveDirection::forward)
    {
        new_pos_y = pos_y + increment_y;
        new_pos_x = pos_x + increment_x;
    }
    else if (direction == MoveDirection::backward)
    {
        new_pos_y = pos_y - increment_y;
        new_pos_x = pos_x - increment_x;
    }
    else if (direction == MoveDirection::strafe_left)
    {
        new_pos_y = pos_y - increment_x;
        new_pos_x = pos_x + increment_y;
    }
    else
    {
        assert(direction == MoveDirection::strafe_right);
        new_pos_y = pos_y + increment_x;
        new_pos_x = pos_x - increment_y;
    }

    // only move if not hitting wall
    char new_block;
    if (!mazemap.is_wall_block(new_pos_y, new_pos_x, new_block))
    {
        pos_y = new_pos_y;
        pos_x = new_pos_x;
    }

    if (new_block == 'X')
    {
        throw SuccessfullyEscaped{};
    }
}

string Player::get_player_representation() const
{
    float rev_angle = -angle;
    if(-7 * HALF_PI_OVER_4 <= rev_angle && rev_angle < -5 * HALF_PI_OVER_4)
    {
        return "\u2199"; // ↙
    }
    else if(-5 * HALF_PI_OVER_4 <= rev_angle && rev_angle < -3 * HALF_PI_OVER_4)
    {
        return "\u2193"; // ↓
    }
    else if(-3 * HALF_PI_OVER_4 <= rev_angle && rev_angle < -HALF_PI_OVER_4)
    {
        return "\u2198"; // ↘
    }
    else if (-HALF_PI_OVER_4 <= rev_angle && rev_angle < HALF_PI_OVER_4)
    {
        return "\u2192"; // →
    }
    else if(HALF_PI_OVER_4 <= rev_angle && rev_angle < 3*HALF_PI_OVER_4)
    {
        return "\u2197"; // ↗
    }
    else if(3*HALF_PI_OVER_4 <= rev_angle && rev_angle < 5*HALF_PI_OVER_4)
    {
        return "\u2191"; // ↑
    }
    else if(5*HALF_PI_OVER_4 <= rev_angle && rev_angle < 7*HALF_PI_OVER_4)
    {
        return "\u2196"; // ↖
    }
    return "\u2190"; // ←
}
