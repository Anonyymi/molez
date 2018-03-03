#include "entity.h"
#include "game.h"
#include "level.h"
#include "audio_manager.h"
#include "3rdparty/mlibc_log.h"

Entity::Entity(
	Game * const game,
	Level * level,
	EntityProps props,
	Sprite * sprite
) :
	m_game(game),
	m_level(level),
	m_props(props),
	m_sprite(sprite),
	m_pva(m_props.spawn),
	m_state(m_props.state),
	m_health(m_props.health)
{

}

Entity::~Entity()
{

}

void Entity::update(float t, float dt)
{
	// Reset time if dead + trigger respawn + do some other preparation
	if (m_state == ES_DEAD)
	{
		m_time = 0.0f;
		m_state = ES_SPAWNING;

		// Move to random position on level
		m_pva.pos = rng_vec2(m_level->getCfg().width, m_level->getCfg().height);

		// Generate random final spawn pos
		m_props.spawn = rng_vec2(m_level->getCfg().width, m_level->getCfg().height);
	}

	// Respawn
	if (m_state == ES_SPAWNING)
	{
		// Respawn process complete, spawn the entity + re-init
		if (m_time >= m_props.respawnTime)
		{
			m_state = ES_ALIVE;
			m_health = m_props.health;

			// Clear the level around player
			m_level->alter(M_VOID, 24, m_pva.pos.x, m_pva.pos.y, true);

			// Play spawn audio
			AudioManager::play_audio("ALIVE.SFX");
		}
		// Respawn process incomplete, move towards spawn pos
		else
		{
			// Calculate direction vector
			vec2 dir = m_props.spawn - m_pva.pos;

			// Move towards spawn
			m_pva.pos += dir * dt;
		}
	}

	// Update time
	m_time += dt;
}

void Entity::render()
{
	// Do not render when entity is dead
	if (m_state == ES_DEAD)
	{
		return;
	}

	// Render sprite
	if (m_sprite)
	{
		m_sprite->render(static_cast<int>(m_pva.pos.x), static_cast<int>(m_pva.pos.y), (m_state == ES_SPAWNING) ? 90 : -1);
	}
}

void Entity::setState(EntityState_t state)
{
	m_state = state;
}

EntityProps & Entity::getProps()
{
	return m_props;
}

Sprite * Entity::getSprite() const
{
	return m_sprite;
}

Physics::PosVelAcc Entity::getPVA() const
{
	return m_pva;
}

EntityState_t Entity::getState() const
{
	return m_state;
}

float Entity::getHealth() const
{
	return m_health;
}