#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "game_state.h"
#include "menu.h"

class MenuState : public GameState
{
public:
	MenuState(
		Game * const game
	);
	virtual ~MenuState() override;

	virtual void update(float dt, float t) override;
	virtual void render() override;
private:
	Menu m_menu;
	Menu m_menu_game_cfg;
	Menu m_menu_level_cfg;
};

#endif // MENU_STATE_H