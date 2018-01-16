#include "game_state.h"
#include "3rdparty/mlibc_log.h"
#include "game.h"

GameState::GameState(Game * const game) :
	m_game(game)
{
	mlibc_inf("GameState::GameState()");
}

GameState::~GameState()
{
	mlibc_inf("GameState::~GameState()");
}
