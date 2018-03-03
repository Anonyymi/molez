#include "player.h"

Player::Player(
	Game * const game,
	Level * level,
	EntityProps props
) :
	Entity(
		game,
		level,
		props,
		new Sprite("PLAYER.JSON")
	)
{

}

Player::~Player()
{

}