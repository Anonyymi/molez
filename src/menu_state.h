#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "game_state.h"
#include "menu.h"

class Level;

class MenuState : public GameState
{
public:
	MenuState(
		Game * const game,
		Level * const level
	);
	virtual ~MenuState() override;

	virtual void update(float state, float t, float dt) override;
	virtual void render(float state) override;
private:
	Menu m_menu;
	Menu m_menu_game_cfg;
	Menu m_menu_level_cfg;
	Level * m_level;
};

#endif // MENU_STATE_H