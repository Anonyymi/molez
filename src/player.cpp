#include "player.h"


Player::
Player(){

}
Player::
~Player(){

}

void Player::
setPos(uint32_t newX, uint32_t newY){
    x = newX;
    y = newY;
}

void Player::
move(int32_t amountX, int32_t amountY){
    x += amountX;
    y += amountY;
}

uint32_t Player::
getX(){
    return x;
}

uint32_t Player::
getY(){
    return y;
}