#include <iostream>
#include <thread>
#include <chrono>

#include "mazemap.h"
#include "player.h"
#include "screen.h"
#include "renderer.h"


/*
STILL TO DO :
- use clang formatter ?

Dependencies :
    sudo apt install libncursesw5-dev

Improvements :
- fix fish-eye effect
- build a colour minimap (instead of the current ascii one)

In case ncurses messed up your terminal, just type "reset"

*/


int main(int argc, char* argv[])
{

    MazeMap mazemap({
        {"BRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBR"},
        {"R.................RRRRRR.................R"},
        {"B.................RRRRRR....CC...........B"},
        {"R......GGG...............................R"},
        {"B......GGG.........................YY....B"},
        {"R..................................YY....R"},
        {"R..............................RR..YY....R"},
        {"B......CCC.....................RR........B"},
        {"R......XCC...............................R"},
        {"B......CCC...............................B"},
        {"R........................................R"},
        {"R........................................R"},
        {"B......MMM........YY..........GGGG.......B"},
        {"R......MMM........YY..........GGGG.......R"},
        {"B.................YY.....................B"},
        {"R.................YYCC...................R"},
        {"BRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBRBR"},
    });
    bool found = false;
    Player player{mazemap};
    {
        Screen screen;
        Renderer renderer{mazemap, player, screen};
        found = renderer.loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }

    std::cout << std::endl << std::endl;
    std::cout << (found ? "Congrats, you found the magic wall !" : "Tired of searching ? So disappointing...") << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    return 0;
}
