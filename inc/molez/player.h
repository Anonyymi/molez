#ifndef PLAYER_H
#define PLAYER_H
#include <stdint.h>


typedef struct PlayerMovement{
    uint32_t playerId;
    //Vector would be nice
    int32_t x,y;
} PlayerMovement;

class Player{
private:
    uint32_t x,y;
public:
    Player();
    ~Player();

    void setPos(uint32_t newX, uint32_t newY);
    void move(int32_t amountX, int32_t amountY);
    uint32_t getX();
    uint32_t getY();
};

#endif