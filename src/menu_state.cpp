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
	m_menu_game_cfg.add_item(new MenuItem("WIN WIDTH", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("WIN HEIGHT", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("WIN SCALE", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("FBO RES X", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("FBO RES Y", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("FULLSCREEN", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("GFX FPS", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("AUDIO VOL", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("MUSIC VOL", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("PHYS TICKRATE", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu_game_cfg.add_item(new MenuItem("PHYS TIMESTEP", MI_EMPTY, nullptr, MIV_EMPTY));

	// Define level cfg menu
	m_menu_level_cfg.add_item(new MenuItem("SEED", MI_EMPTY, nullptr, MIV_UINT32));
	m_menu_level_cfg.add_item(new MenuItem("TYPE", MI_EMPTY, nullptr, MIV_UINT8));
	m_menu_level_cfg.add_item(new MenuItem("WIDTH", MI_EMPTY, nullptr, MIV_INT32));
	m_menu_level_cfg.add_item(new MenuItem("HEIGHT", MI_EMPTY, nullptr, MIV_INT32));
	m_menu_level_cfg.add_item(new MenuItem("NOISE", MI_EMPTY, nullptr, MIV_FLOAT));
	m_menu_level_cfg.add_item(new MenuItem("WATER", MI_EMPTY, nullptr, MIV_UINT8));
	m_menu_level_cfg.add_item(new MenuItem("LAVA", MI_EMPTY, nullptr, MIV_UINT8));

	// Define game main menu
	m_menu.add_item(new MenuItem("NEW GAME", MI_EMPTY, nullptr, MIV_EMPTY));
	m_menu.add_item(new MenuItem("SERVER BROWSER", MI_EMPTY, nullptr, MIV_EMPTY));
	std::function<void()> action_regen = [this]() { m_level->regenerate((uint32_t)time(NULL)); };
	m_menu.add_item(new MenuItem("REGEN LEVEL", MI_BUTTON, nullptr, MIV_EMPTY, action_regen));
	m_menu.add_item(new MenuItem("GAME CONFIG", MI_SUBMENU, &m_menu_game_cfg, MIV_EMPTY));
	m_menu.add_item(new MenuItem("LEVEL CONFIG", MI_SUBMENU, &m_menu_level_cfg, MIV_EMPTY));
	std::function<void()> action_exit = [this]() { m_game->stop(); };
	m_menu.add_item(new MenuItem("EXIT TO OS", MI_BUTTON, nullptr, MIV_EMPTY, action_exit));

	// Switch to menu music
	AudioManager::play_music("MENU.MUS");

	// Init menu camera, center to menu
	DisplayManager::Window * window = DisplayManager::ACTIVE_WINDOW;
	DisplayManager::load_camera("menu", window->width / 2, -window->height / 2);
	DisplayManager::activate_camera("menu");
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
		m_level->render();

	// Render menu
	m_menu.render();
}