#include "menu_state.h"
#include <functional>
#include <ctime>
#include "game.h"
#include "level.h"
#include "audio_manager.h"
#include "display_manager.h"
#include "texture_manager.h"

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
	m_menu_game_cfg.add_item(new MenuItem("WIN WIDTH", MI_NUMERIC, MenuItemVal(MIV_INT32, &m_game->getConfig().win_width, 1)));
	m_menu_game_cfg.add_item(new MenuItem("WIN HEIGHT", MI_NUMERIC, MenuItemVal(MIV_INT32, &m_game->getConfig().win_height, 1)));
	m_menu_game_cfg.add_item(new MenuItem("WIN SCALE", MI_NUMERIC, MenuItemVal(MIV_INT32, &m_game->getConfig().win_scale, 1, 1, 5)));
	m_menu_game_cfg.add_item(new MenuItem("FULLSCREEN", MI_NUMERIC, MenuItemVal(MIV_BOOL, &m_game->getConfig().win_fullscreen)));
	m_menu_game_cfg.add_item(new MenuItem("GFX FRAMERATE", MI_NUMERIC, MenuItemVal(MIV_FLOAT, &m_game->getConfig().gfx_framerate, 0.1f)));
	m_menu_game_cfg.add_item(new MenuItem("AUDIO VOL", MI_NUMERIC, MenuItemVal(MIV_INT32, &m_game->getConfig().sfx_audio_vol, 4, 0, 128)));
	m_menu_game_cfg.add_item(new MenuItem("MUSIC VOL", MI_NUMERIC, MenuItemVal(MIV_INT32, &m_game->getConfig().sfx_music_vol, 4, 0, 128)));
	m_menu_game_cfg.add_item(new MenuItem("PHYS TICKRATE", MI_NUMERIC, MenuItemVal(MIV_FLOAT, &m_game->getConfig().phy_tickrate, 0.1f)));

	// Define level cfg menu
	m_menu_level_cfg.add_item(new MenuItem("SEED", MI_NUMERIC, MenuItemVal(MIV_UINT32, &m_level->get_config().seed, 1)));
	m_menu_level_cfg.add_item(new MenuItem("TYPE", MI_NUMERIC, MenuItemVal(MIV_UINT8, &m_level->get_config().type, 1, 0, 255)));
	m_menu_level_cfg.add_item(new MenuItem("WIDTH", MI_NUMERIC, MenuItemVal(MIV_INT32, &m_level->get_config().width, 8)));
	m_menu_level_cfg.add_item(new MenuItem("HEIGHT", MI_NUMERIC, MenuItemVal(MIV_INT32, &m_level->get_config().height, 8)));
	m_menu_level_cfg.add_item(new MenuItem("NOISE", MI_NUMERIC, MenuItemVal(MIV_FLOAT, &m_level->get_config().n_scale, 0.001f)));
	m_menu_level_cfg.add_item(new MenuItem("WATER", MI_NUMERIC, MenuItemVal(MIV_UINT8, &m_level->get_config().n_water, 1)));
	m_menu_level_cfg.add_item(new MenuItem("LAVA", MI_NUMERIC, MenuItemVal(MIV_UINT8, &m_level->get_config().n_lava, 1)));

	// Define game main menu
	m_menu.add_item(new MenuItem("NEW GAME", MI_EMPTY, MenuItemVal()));
	m_menu.add_item(new MenuItem("SERVER BROWSER", MI_EMPTY, MenuItemVal()));
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
	m_menu.add_item(new MenuItem("REGEN LEVEL", MI_BUTTON, MenuItemVal(), action_regen));
	m_menu.add_item(new MenuItem("GAME CONFIG", MI_SUBMENU, MenuItemVal(MIV_EMPTY, &m_menu_game_cfg)));
	m_menu.add_item(new MenuItem("LEVEL CONFIG", MI_SUBMENU, MenuItemVal(MIV_EMPTY, &m_menu_level_cfg)));
	std::function<void()> action_exit = [this]() { m_game->stop(); };
	m_menu.add_item(new MenuItem("EXIT TO OS", MI_BUTTON, MenuItemVal(), action_exit));

	// Switch to menu music
	AudioManager::play_music("MENU.MUS");

	// Init level, init camera
	action_regen();
}

MenuState::~MenuState()
{

}

void MenuState::update(float state, float t, float dt)
{
	// Update level
	if (m_level)
		m_level->update(state, t, dt);

	// Update menu
	m_menu.update();
}

void MenuState::render(float state)
{
	// Render level
	if (m_level)
	{
		// Switch to level camera
		DisplayManager::activate_camera("level");

		// Render level
		m_level->render(state);
	}

	// Switch to menu camera
	DisplayManager::activate_camera("menu");

	// Render menu
	m_menu.render();

	// Render debug info
	DisplayManager::set_text(0, 0, 16, 16, "FPS:" + std::to_string(m_game->getConfig().gfx_framerate), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*1, 16, 16, "UPS:" + std::to_string(m_game->getConfig().phy_tickrate), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*2, 16, 16, "T:" + std::to_string(m_game->getPhysState().t), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*3, 16, 16, "DT:" + std::to_string(m_game->getPhysState().dt), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*4, 16, 16, "T_CURRENT:" + std::to_string(m_game->getPhysState().t_curr), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*5, 16, 16, "T_ACCUM:" + std::to_string(m_game->getPhysState().t_acc), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*6, 16, 16, "S_CURRENT:" + std::to_string(m_game->getPhysState().s_curr), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*7, 16, 16, "S_PREVIOUS:" + std::to_string(m_game->getPhysState().s_prev), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*8, 16, 16, "S_LERP:" + std::to_string(m_game->getPhysState().s_lerp), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
	DisplayManager::set_text(0, 16*9, 16, 16, "ALPHA:" + std::to_string(m_game->getPhysState().alpha), 255, 0, 255, TextureManager::load_font("MOLEZ.JSON"));
}