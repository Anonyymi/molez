#include "play_state.h"
#include <functional>
#include <ctime>
#include <SDL2/SDL.h>
#include "game.h"
#include "level.h"
#include "audio_manager.h"
#include "display_manager.h"
#include "texture_manager.h"
#include "input_manager.h"
#include "sprite.h"
#include "entity.h"
#include "player.h"

PlayState::PlayState(
	Game * const game,
	Level * level
) :
	GameState(game),
	m_level(level),
	m_entities()
{
	// Init gui camera, translate to window center
	int g_camera_x = DisplayManager::ACTIVE_WINDOW->width / 2;
	int g_camera_y = -DisplayManager::ACTIVE_WINDOW->height / 2;
	DisplayManager::Camera * g_camera = DisplayManager::load_camera("gui", 0, 0, 1);
	g_camera->x = g_camera_x;
	g_camera->y = g_camera_x;

	// Init level camera, translate to level center
	int l_camera_x = m_level->getCfg().width / 2;
	int l_camera_y = -m_level->getCfg().height / 2;
	DisplayManager::Camera * l_camera = DisplayManager::load_camera("level", 0, 0, 1);
	l_camera->x = l_camera_x;
	l_camera->y = l_camera_y;

	// Switch to ingame music
	uint8_t r_val_mus = static_cast<uint8_t>(RNG_DIST8(RNG)) % 2;
	r_val_mus++;
	AudioManager::play_music("INGAME" + std::to_string(r_val_mus) + ".MUS");

	// Play round start sound
	AudioManager::play_audio("BEGIN.SFX");

	// Get player count & spawn players
	size_t n_players = m_game->getCfg().n_players;
	for (size_t i = 0; i < n_players; i++)
	{
		// Create props
		EntityProps player_props{ E_PLAYER_OFFLINE, "player " + std::to_string(i), vec2(), 2.5f, ES_DEAD, 100.0f };

		// Create player
		Player * player_entity = new Player(m_game, m_level, player_props);

		// Set player controller
		EntityCtrl player_ctrl = player_entity->getCtrl();

		auto game_cfg = m_game->getCfg();
		if (i + 1 <= game_cfg.c_players.size())
		{
			player_ctrl.bind_map = game_cfg.c_players[i];
		}

		player_entity->setCtrl(player_ctrl);

		// Push to entity list
		m_entities.push_back(player_entity);
	}
}

PlayState::~PlayState()
{
	for (auto e : m_entities)
	{
		delete e;
	}
}

void PlayState::update(float state, float t, float dt)
{
	// Handle input
	if (InputManager::KBOARD[SDLK_1])
	{
		Entity * player = getEntityByName("player 1");

		if (player)
		{
			player->setState(ES_DEAD);
		}

		InputManager::KBOARD[SDLK_1] = false;
	}
	if (InputManager::KBOARD[SDLK_2])
	{
		Entity * player = getEntityByName("player 2");

		if (player)
		{
			player->setState(ES_DEAD);
		}

		InputManager::KBOARD[SDLK_2] = false;
	}

	// Update level
	if (m_level)
	{
		m_level->update(state, t, dt);
	}

	// Update entities
	for (auto e : m_entities)
	{
		e->update(t, dt);
	}

	// Update gui
	//m_menu.update();

	// Update time
	m_time += dt;
}

void PlayState::render(float state)
{
	// Render level
	if (m_level)
	{
		// Switch to level camera
		DisplayManager::activate_camera("level");

		// Render level
		m_level->render(state);
	}

	// Render entities
	for (auto e : m_entities)
	{
		e->render();
	}

	// Switch to gui camera
	DisplayManager::activate_camera("gui");

	// Render gui
	//m_menu.render();
}

Entity * PlayState::getEntityByName(const std::string & name)
{
	for (auto e : m_entities)
	{
		if (e->getProps().name == name)
		{
			return e;
		}
	}

	return nullptr;
}