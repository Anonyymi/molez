#ifndef ENTITY_H
#define ENTITY_H

#include "math.h"
#include "physics.h"

class Game;

class Entity
{
public:
private:
protected:
	Game * const m_game;
	Physics::PosVelAcc m_pva;
};

#endif // ENTITY_H