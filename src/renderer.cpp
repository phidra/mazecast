#include <cassert>
#include <stdexcept>
#include <ncurses.h>

#include "renderer.h"
#include "mazemap.h"
#include "player.h"
#include "screen.h"

using namespace std;

static const int CEILING_MIN = 10;
static const int CEILING_MAX = 40;
static const int CEILING_SPAN = CEILING_MAX - CEILING_MIN;
static const int FLOOR_MIN = 10;
static const int FLOOR_MAX = 40;
static const int FLOOR_SPAN = FLOOR_MAX - FLOOR_MIN;

static const float PLAYER_FOV = M_PI / 4;
static const float HALF_PLAYER_FOV = PLAYER_FOV / 2;

static const float RAY_SPEED = 0.3;
static const float UPPER_BOUND_FOR_RAY_DISTANCE = 100;

const string CHAR_VERY_FAR   = "\u2588";  // █
const string CHAR_FAR        = "\u2593";  // ▓
const string CHAR_NEAR       = "\u2592";  // ▒
const string CHAR_VERY_NEAR  = "\u2591";  // ░
const string CHAR_MAGIC      = "\u2584";  // ▄ 

const float NEAR_THRESHOLD = 6.0;
const float FAR_THRESHOLD = 14.0;
const float VERY_FAR_THRESHOLD = 24.0;

const char MAGIC_WALL_ID = 'X';

const float CEIL_VERY_NEAR_RATIO = 0.10;
const float CEIL_NEAR_RATIO      = 0.20;
const float CEIL_FAR_RATIO       = 0.30;

const float FLOOR_VERY_FAR_RATIO = 1.0 - CEIL_FAR_RATIO;
const float FLOOR_FAR_RATIO = 1.0 - CEIL_NEAR_RATIO;
const float FLOOR_NEAR_RATIO = 1.0 - CEIL_VERY_NEAR_RATIO;


Renderer::Renderer(MazeMap& the_map_, Player& player_, Screen& screen_) :
    player(player_),
    mazemap(the_map_),
    screen(screen_),
    distance_by_col(screen.nb_cols)
{
}


float Renderer::compute_column_angle(int current_col) const
// For a given col, compute the angle at which we must cast ray
{
    // Desired mapping from x to angle :
    //      x = 0           ->    player.angle - HALF_PLAYER_FOV
    //      x = line_width  ->    player.angle + HALF_PLAYER_FOV
    return (player.get_angle() - HALF_PLAYER_FOV) + (static_cast<float>(current_col) / screen.nb_cols) * PLAYER_FOV;
}


void Renderer::compute_room_thresholds(const float& distance_from_player, int& ceiling_threshold, int& floor_threshold) const
{
    int ceiling_pos_from_bottom = 0;  // ceiling as a distance from bottom of screen (0 = no ceiling)

    // Desired mapping from "distance_from_player" to floor+ceiling thresholds :
    //      distance <= 0             --> floor = FLOOR_MIN  /  ceiling = CEILING_MIN
    //      distance >= DISTANCE_MAX  --> floor = FLOOR_MAX  /  ceiling = CEILING_MAX
    if (distance_from_player <= 0)
    {
        ceiling_threshold = FLOOR_MIN;
        ceiling_pos_from_bottom = CEILING_MIN;
    }
    else if (distance_from_player >= mazemap.max_distance)
    {
        ceiling_threshold = FLOOR_MAX;
        ceiling_pos_from_bottom = CEILING_MAX;
    }
    else
    {
        ceiling_threshold = FLOOR_MIN + static_cast<int>(FLOOR_SPAN * distance_from_player / mazemap.max_distance);
        ceiling_pos_from_bottom = CEILING_MIN + static_cast<int>(CEILING_SPAN * distance_from_player / mazemap.max_distance);
    }

    floor_threshold = screen.nb_lines - ceiling_pos_from_bottom;
}


float Renderer::cast_ray(const int current_col, char& wall_char) const
{
    float column_angle = compute_column_angle(current_col);

    // ray casting must stop when a wall is hit :
    bool wall_is_hit = false;

    float distance_from_player = 0;  // starting from player position
    while (!wall_is_hit)
    {
        distance_from_player += RAY_SPEED;
        assert(distance_from_player < UPPER_BOUND_FOR_RAY_DISTANCE); // avoid infinite loops

        // ray increments step by step from player position :
        float ray_position_y = player.get_pos_y() + (distance_from_player * sin(column_angle));
        float ray_position_x = player.get_pos_x() + (distance_from_player * cos(column_angle));

        // vérifier si on est sur un mur, si oui, arrêter
        wall_is_hit = mazemap.is_wall_block(int(ray_position_y), int(ray_position_x), wall_char);
    }

    return distance_from_player;
}


string Renderer::compute_wall_pixel(float distance_from_player) const
{
    // the further the darker :
    if (distance_from_player >= VERY_FAR_THRESHOLD)
    {
        return CHAR_VERY_FAR;
    }

    if (distance_from_player >= FAR_THRESHOLD)
    {
        return CHAR_FAR;
    }

    if (distance_from_player >= NEAR_THRESHOLD)
    {
        return CHAR_NEAR;
    }
    return CHAR_VERY_NEAR;
}


string Renderer::compute_floorceil_pixel(const int current_line) const
{ 
    float ratio = static_cast<float>(current_line) / screen.nb_lines;

    if (ratio < CEIL_VERY_NEAR_RATIO)
    {
        return CHAR_VERY_NEAR;
    }
    else if(ratio < CEIL_NEAR_RATIO)
    {
        return CHAR_NEAR;
    }
    else if (ratio < CEIL_FAR_RATIO)
    {
        return CHAR_FAR;
    }
    else if (ratio < FLOOR_VERY_FAR_RATIO)
    {
        return CHAR_VERY_FAR;
    }
    else if(ratio < FLOOR_FAR_RATIO)
    {
        return CHAR_FAR;
    }
    else if (ratio < FLOOR_NEAR_RATIO)
    {
        return CHAR_NEAR;
    }
    return CHAR_VERY_NEAR;
}


void Renderer::draw_line_pixel(
    Screen& screen,
    const int current_line,
    const int current_col,
    const float distance_from_player,
    const char wall_char,
    const int ceiling_threshold,
    const int floor_threshold) const
{
    assert(current_line < screen.nb_lines);
    assert(current_col < screen.nb_cols);

    string pixel;
    unsigned int color;
    if (current_line <= ceiling_threshold)
    {
        pixel = compute_floorceil_pixel(current_line);
        color = screen.CEIL_COLOR;
    }
    else if (current_line < floor_threshold)
    {
        pixel = compute_wall_pixel(distance_from_player);
        if (wall_char == 'R')
            color = screen.RED_WALL_COLOR;
        else if (wall_char == 'G')
            color = screen.GREEN_WALL_COLOR;
        else if (wall_char == 'B')
            color = screen.BLUE_WALL_COLOR;
        else if (wall_char == 'C')
            color = screen.CYAN_WALL_COLOR;
        else if (wall_char == 'M')
            color = screen.MAGENTA_WALL_COLOR;
        else if (wall_char == 'Y')
            color = screen.YELLOW_WALL_COLOR;
        else if (wall_char == MAGIC_WALL_ID)  // magic wall
        {
            color = screen.MAGIC_WALL_COLOR;
            pixel = CHAR_MAGIC;
        }
        else
            throw logic_error("unknown wall_char : '" + string(1, wall_char) + "'");
    }
    else
    {
        pixel = compute_floorceil_pixel(current_line);
        color = screen.FLOOR_COLOR;
    }
    screen.draw_pixel(current_line, current_col, pixel, color);
}


void Renderer::render(Screen& screen)
{
    int ceiling_threshold = 0;
    int floor_threshold = screen.nb_lines;

    for (int current_col = 0; current_col < screen.nb_cols; ++current_col)
    {
        // for a given col, we cast ray, and see at which distance it hits wall :
        char wall_char;
        const float distance_from_player = cast_ray(current_col, wall_char);
        distance_by_col[current_col] = distance_from_player;

        // for the given distance, compute floor/ceiling thresholds :
        compute_room_thresholds(distance_from_player, ceiling_threshold, floor_threshold);

        // for each "pixel" of the current col, draw the appropriate char :
        for (int current_line = 0; current_line < screen.nb_lines; ++current_line)
        {
            draw_line_pixel(screen, current_line, current_col, distance_from_player, wall_char, ceiling_threshold, floor_threshold);
        }
    }
}


float Renderer::get_distance(const int col) const
{
    return distance_by_col.at(col);
}


bool Renderer::loop()
{
    MEVENT event;
    int pressed_key = -1;
    string debug_mouse = "";

    while (true)
    {
        render(screen);
        screen.draw_additions(
            player,
            mazemap,
            {
                "Click a column to see its distance",
                "(mouseY, mouseX)-distance = " + debug_mouse,
                "Player angle (rad) : " + to_string(-player.get_angle()),
                "See player position on minimap below"
            },
            {
                "Find and enter the magic wall !",
                "Press 'q' to exit, 'd' to debug"
            }
        );
        screen.refresh();


        pressed_key = getch();
        debug_mouse = "";
        try
        {
            if (pressed_key == KEY_LEFT)
            {
                player.turn_left();
            }
            else if (pressed_key == KEY_RIGHT)
            {
                player.turn_right();
            }
            else if (pressed_key == KEY_SLEFT)
            {
                player.strafe_left();
            }
            else if (pressed_key == KEY_SRIGHT)
            {
                player.strafe_right();
            }
            else if (pressed_key == KEY_UP)
            {
                player.move_forward();
            }
            else if (pressed_key == KEY_DOWN)
            {
                player.move_backward();
            }
            else if (pressed_key == 'q')
            {
                return false;
            }
            else if (pressed_key == 'd')
            {
                screen.toggle_debug_mode();
            }
            else if (pressed_key == KEY_MOUSE)
            {
                if(getmouse(&event) == OK)
                {
                    if(event.bstate & BUTTON1_CLICKED || event.bstate & BUTTON1_DOUBLE_CLICKED || event.bstate & BUTTON1_TRIPLE_CLICKED)
                    {
                        debug_mouse = "(" + to_string(event.y) + "," + to_string(event.x) + ")-" + to_string(get_distance(event.x));
                    }
                    else if(event.bstate & BUTTON3_CLICKED || event.bstate & BUTTON3_DOUBLE_CLICKED || event.bstate & BUTTON3_TRIPLE_CLICKED)
                    {
                        debug_mouse = "(" + to_string(event.y) + "," + to_string(event.x) + ")-" + to_string(get_distance(event.x));
                    }
                }
            }
        }
        catch (SuccessfullyEscaped)
        {
            return true;
        }
    }
}
