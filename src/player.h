#pragma once

#include <string>

class MazeMap;
class SuccessfullyEscaped : public std::exception {};

enum class MoveDirection;

class Player
{

    public:
        Player(const MazeMap&);
        void move_forward();
        void move_backward();
        void strafe_left();
        void strafe_right();
        void turn_left();
        void turn_right();
        void fit_angle();
        std::string get_player_representation() const;
        inline float get_pos_y() const { return pos_y; }
        inline float get_pos_x() const { return pos_x; }
        inline float get_angle() const { return angle; }

   private:
        const MazeMap& mazemap;
        float pos_y;
        float pos_x;
        float angle;
        const float ANGLE_INCREMENT = 0.1;  // speed at which player turns
        const float POS_INCREMENT = 0.5;    // speed at which player moves

        void move_position(const MoveDirection& direction);

};

