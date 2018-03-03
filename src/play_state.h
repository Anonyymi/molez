#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include <vector>
#include "math.h"
#include "game_state.h"
#include "menu.h"

using namespace Math;

class Level;
class Sprite;
class Entity;

class PlayState : public GameState
{
public:
	PlayState(
		Game * const game,
		Level * level
	);
	virtual ~PlayState() override;

	virtual void update(float state, float t, float dt) override;
	virtual void render(float state) override;

	Entity * getEntityByName(const std::string & name);
private:
	Level * m_level;
	std::vector<Entity *> m_entities;
	float m_time;
};

#endif // PLAY_STATE_H