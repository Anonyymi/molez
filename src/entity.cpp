#include "entity.h"
#include "game.h"
#include "level.h"
#include "audio_manager.h"
#include "input_manager.h"
#include "3rdparty/mlibc_log.h"

Entity::Entity(
	Game * const game,
	Level * level,
	EntityProps props,
	EntityCtrl ctrl,
	Sprite * sprite
) :
	m_game(game),
	m_level(level),
	m_props(props),
	m_ctrl(ctrl),
	m_sprite(sprite),
	m_time(0.0f),
	m_pva(m_props.spawn),
	m_state(m_props.state),
	m_health(m_props.health),
	m_aabb()
{

}

Entity::~Entity()
{

}

void Entity::update(float t, float dt)
{
	// Update AABB
	if (m_sprite)
	{
		// Get current sprite anim frame data
		auto frame = m_sprite->getCurrentFrame();

		m_aabb.setMinP(Math::vec2(
			m_pva.pos.x - static_cast<float>(frame->w * 0.5f),
			m_pva.pos.y - static_cast<float>(frame->h * 0.5f)
		));
		m_aabb.setMaxP(Math::vec2(
			m_pva.pos.x + static_cast<float>(frame->w * 0.5f),
			m_pva.pos.y + static_cast<float>(frame->h * 0.5f)
		));
	}

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
			m_level->alter(M_VOID, T_AIR, 24, static_cast<int>(m_pva.pos.x), static_cast<int>(m_pva.pos.y), true);

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

	// Alive
	if (m_state == ES_ALIVE)
	{
		// Simulate physics step
		m_pva.pos = m_pva.pos + m_pva.vel * dt;

		// Apply air friction
		m_pva.vel -= m_pva.vel * 0.05f;

		// Apply gravity force
		m_pva.vel += Math::vec2(0.0f, -9.8f);
	}

	// Handle controller
	for (auto & kv : m_ctrl.bind_map)
	{
		// Get key & bind location
		auto key = kv.first;
		auto val = kv.second;

		// Skip if bind not found
		if (InputManager::KBOARD.count(key) == 0 || m_ctrl.ctrl_map.count(val) == 0)
			continue;

		// Toggle controller key state
		if (InputManager::KBOARD[key])
		{
			m_ctrl.ctrl_map[val] = true;
		}
		else
		{
			m_ctrl.ctrl_map[val] = false;
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
		// Get current sprite anim frame data
		auto frame = m_sprite->getCurrentFrame();

		m_sprite->render(
			static_cast<int>(m_pva.pos.x - static_cast<float>(frame->w * 0.5f)),
			static_cast<int>(m_pva.pos.y - static_cast<float>(frame->h * 0.5f)),
			(m_state == ES_SPAWNING) ? 90 : -1
		);
	}

	// Render AABB
	m_aabb.render(0, 255, 0, 64);
}

void Entity::setCtrl(EntityCtrl ctrl)
{
	m_ctrl = ctrl;
}

void Entity::setState(EntityState_t state)
{
	m_state = state;
}

EntityProps & Entity::getProps()
{
	return m_props;
}

EntityCtrl Entity::getCtrl() const
{
	return m_ctrl;
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