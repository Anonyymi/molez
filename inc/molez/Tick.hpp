//One game tick
#ifndef TICK_HPP
#define TICK_HPP
#include <stdint.h>
#include <vector>

#include "player.h"


typedef struct Tick{
    //All players movements and a list of happenings goes here
    uint32_t count;
    PlayerMovement move;

    
}Tick;

#endif