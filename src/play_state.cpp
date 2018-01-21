#include "play_state.h"
#include <functional>
#include <ctime>
#include "game.h"
#include "level.h"
#include "audio_manager.h"
#include "display_manager.h"
#include "texture_manager.h"

PlayState::PlayState(
	Game * const game,
	Level * const level
) :
	GameState(game),
	m_level(level)
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
}

PlayState::~PlayState()
{

}

void PlayState::update(float state, float t, float dt)
{
	// Update level
	if (m_level)
		m_level->update(state, t, dt);

	// Update gui
	//m_menu.update();
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

	// Switch to gui camera
	DisplayManager::activate_camera("gui");

	// Render gui
	//m_menu.render();
}