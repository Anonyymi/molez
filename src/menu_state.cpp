#include "menu_state.h"
#include <functional>
#include <ctime>
#include "game.h"
#include "level.h"
#include "audio_manager.h"
#include "display_manager.h"

MenuState::MenuState(
	Game * const game,
	Level * const level
) :
	GameState(game),
	m_menu("MOLEZ"),
	m_menu_game_cfg("GAME CFG"),
	m_menu_level_cfg("LEVEL CFG"),
	m_level(level)
{
	// Define game cfg menu
	m_menu_game_cfg.add_item(new MenuItem("WIN WIDTH", MI_NUMERIC, &m_game->getConfig().win_width, MIV_INT32));
	m_menu_game_cfg.add_item(new MenuItem("WIN HEIGHT", MI_NUMERIC, &m_game->getConfig().win_height, MIV_INT32));
	m_menu_game_cfg.add_item(new MenuItem("WIN SCALE", MI_NUMERIC, &m_game->getConfig().win_scale, MIV_INT32));
	m_menu_game_cfg.add_item(new MenuItem("FULLSCREEN", MI_NUMERIC, &m_game->getConfig().win_fullscreen, MIV_BOOL));
	m_menu_game_cfg.add_item(new MenuItem("GFX FRAMERATE", MI_NUMERIC, &m_game->getConfig().gfx_framerate, MIV_FLOAT));
	m_menu_game_cfg.add_item(new MenuItem("AUDIO VOL", MI_NUMERIC, &m_game->getConfig().sfx_audio_vol, MIV_INT32));
	m_menu_game_cfg.add_item(new MenuItem("MUSIC VOL", MI_NUMERIC, &m_game->getConfig().sfx_music_vol, MIV_INT32));
	m_menu_game_cfg.add_item(new MenuItem("PHYS TICKRATE", MI_NUMERIC, &m_game->getConfig().phy_tickrate, MIV_FLOAT));
	m_menu_game_cfg.add_item(new MenuItem("PHYS TIMESTEP", MI_NUMERIC, &m_game->getConfig().phy_timestep, MIV_FLOAT));

	// Define level cfg menu
	m_menu_level_cfg.add_item(new MenuItem("SEED", MI_NUMERIC, &m_level->get_config().seed, MIV_UINT32));
	m_menu_level_cfg.add_item(new MenuItem("TYPE", MI_NUMERIC, &m_level->get_config().type, MIV_UINT8));
	m_menu_level_cfg.add_item(new MenuItem("WIDTH", MI_NUMERIC, &m_level->get_config().width, MIV_INT32));
	m_menu_level_cfg.add_item(new MenuItem("HEIGHT", MI_NUMERIC, &m_level->get_config().height, MIV_INT32));
	m_menu_level_cfg.add_item(new MenuItem("NOISE", MI_NUMERIC, &m_level->get_config().n_scale, MIV_FLOAT));
	m_menu_level_cfg.add_item(new MenuItem("WATER", MI_NUMERIC, &m_level->get_config().n_water, MIV_UINT8));
	m_menu_level_cfg.add_item(new MenuItem("LAVA", MI_NUMERIC, &m_level->get_config().n_lava, MIV_UINT8));

	// Define game main menu
	m_menu.add_item(new MenuItem("NEW GAME", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu.add_item(new MenuItem("SERVER BROWSER", MI_EMPTY, nullptr, MIV_EMPTY));
	std::function<void()> action_regen = [this]() {
		// Re-generate level
		m_level->regenerate((uint32_t)time(NULL));

		// Init menu camera, translate to window center
		int m_camera_x = DisplayManager::ACTIVE_WINDOW->width / 2;
		int m_camera_y = -DisplayManager::ACTIVE_WINDOW->height / 2;
		DisplayManager::Camera * m_camera = DisplayManager::load_camera("menu", 0, 0, 1);
		m_camera->x = m_camera_x;
		m_camera->y = m_camera_y;

		// Init level camera, translate to level center
		int l_camera_x = m_level->get_config().width / 2;
		int l_camera_y = -m_level->get_config().height / 2;
		DisplayManager::Camera * l_camera = DisplayManager::load_camera("level", 0, 0, 1);
		l_camera->x = l_camera_x;
		l_camera->y = l_camera_y;
	};
	m_menu.add_item(new MenuItem("REGEN LEVEL", MI_BUTTON, nullptr, MIV_EMPTY, action_regen));
	m_menu.add_item(new MenuItem("GAME CONFIG", MI_SUBMENU, &m_menu_game_cfg, MIV_EMPTY));
	m_menu.add_item(new MenuItem("LEVEL CONFIG", MI_SUBMENU, &m_menu_level_cfg, MIV_EMPTY));
	std::function<void()> action_exit = [this]() { m_game->stop(); };
	m_menu.add_item(new MenuItem("EXIT TO OS", MI_BUTTON, nullptr, MIV_EMPTY, action_exit));

	// Switch to menu music
	//AudioManager::play_music("MENU.MUS");

	// Init level, init camera
	action_regen();
}

MenuState::~MenuState()
{

}

void MenuState::update(float dt, float t)
{
	// Update level
	if (m_level)
		m_level->update();

	// Update menu
	m_menu.update();
}

void MenuState::render()
{
	// Render level
	if (m_level)
	{
		// Switch to level camera
		DisplayManager::activate_camera("level");

		// Render level
		m_level->render();
	}

	// Switch to menu camera
	DisplayManager::activate_camera("menu");

	// Render menu
	m_menu.render();
}