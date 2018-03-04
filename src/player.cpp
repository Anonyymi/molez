#include "player.h"
#include <SDL2/SDL.h>

Player::Player(
	Game * const game,
	Level * level,
	EntityProps props
) :
	Entity(
		game,
		level,
		props,
		EntityCtrl(),
		new Sprite("PLAYER.JSON")
	)
{
	// Init controller with default bindings
	m_ctrl.ctrl_map["left"] = false;
	m_ctrl.ctrl_map["right"] = false;
	m_ctrl.ctrl_map["up"] = false;
	m_ctrl.ctrl_map["down"] = false;
	m_ctrl.ctrl_map["jump"] = false;
	m_ctrl.ctrl_map["fire"] = false;
	m_ctrl.ctrl_map["reload"] = false;
	m_ctrl.ctrl_map["change"] = false;

	m_ctrl.bind_map[SDLK_LEFT] = "left";
	m_ctrl.bind_map[SDLK_RIGHT] = "right";
	m_ctrl.bind_map[SDLK_UP] = "up";
	m_ctrl.bind_map[SDLK_DOWN] = "down";
	m_ctrl.bind_map[SDLK_RCTRL] = "jump";
	m_ctrl.bind_map[SDLK_RETURN] = "fire";
	m_ctrl.bind_map[SDLK_BACKSPACE] = "reload";
	m_ctrl.bind_map[SDLK_RSHIFT] = "change";
}

Player::~Player()
{
}

void Player::update(float t, float dt)
{
	// Call superclass update first
	Entity::update(t, dt);

	// Continue with own update code

}