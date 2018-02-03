#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "game_state.h"
#include "menu.h"

class Level;
class Sprite;

extern Sprite * test;

class PlayState : public GameState
{
public:
	PlayState(
		Game * const game,
		Level * const level
	);
	virtual ~PlayState() override;

	virtual void update(float state, float t, float dt) override;
	virtual void render(float state) override;
private:
	Level * m_level;
};

#endif // PLAY_STATE_H