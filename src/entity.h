#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include <map>
#include "sprite.h"
#include "math.h"
#include "physics.h"

class Game;
class Level;

enum Entity_t : uint8_t
{
	E_PLAYER_OFFLINE = 0,
	E_PLAYER_ONLINE = 1,
	E_AI_OFFLINE = 2,
	E_AI_ONLINE = 3
};

enum EntityState_t : uint8_t
{
	ES_DEAD = 0,
	ES_SPAWNING = 1,
	ES_ALIVE = 2
};

struct EntityProps
{
	Entity_t type;
	std::string name;
	Math::vec2 spawn;
	float respawnTime;
	EntityState_t state;
	float health;
};

struct EntityCtrl
{
	std::map<std::string, bool> ctrl_map;
	std::map<int, std::string> bind_map;
};

class Entity
{
public:
	Entity(
		Game * const game,
		Level * level,
		EntityProps props,
		EntityCtrl ctrl,
		Sprite * sprite
	);
	virtual ~Entity();

	virtual void update(float t, float dt);
	virtual void render();

	void setCtrl(EntityCtrl ctrl);
	void setState(EntityState_t state);

	EntityProps & getProps();
	EntityCtrl getCtrl() const;
	Sprite * getSprite() const;
	Physics::PosVelAcc getPVA() const;
	EntityState_t getState() const;
	float getHealth() const;
private:
protected:
	Game * const m_game;
	Level * m_level;
	EntityProps m_props;
	EntityCtrl m_ctrl;
	Sprite * m_sprite;
	float m_time;
	Physics::PosVelAcc m_pva;
	EntityState_t m_state;
	float m_health;
};

#endif // ENTITY_H