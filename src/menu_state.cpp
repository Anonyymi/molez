#include "menu_state.h"
#include "audio_manager.h"
#include "display_manager.h"

MenuState::MenuState(
	Game * const game
) :
	GameState(game),
	m_menu("MOLEZ"),
	m_menu_game_cfg("GAME CFG"),
	m_menu_level_cfg("LEVEL CFG")
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
	m_menu.add_item(new MenuItem("GAME CONFIG", MI_SUBMENU, &m_menu_game_cfg, MIV_EMPTY));
	m_menu.add_item(new MenuItem("LEVEL CONFIG", MI_SUBMENU, &m_menu_level_cfg, MIV_EMPTY));
	m_menu.add_item(new MenuItem("EXIT TO OS", MI_EMPTY, nullptr, MIV_EMPTY));

	// Switch to menu music
	AudioManager::play_music("MENU.MUS");

	// Init menu camera, center to menu
	DisplayManager::Window * window = DisplayManager::ACTIVE_WINDOW;
	DisplayManager::load_camera("menu", window->width / 2, -window->height / 2);
	DisplayManager::activate_camera("menu");

	//m_menu.add_item(new MenuItem("NEW GAME", MI_EMPTY, nullptr, MIV_EMPTY));
	//std::function<void()> menu_regen = [&level]() { level.regenerate((uint32_t)time(NULL)); };
	//m_menu.add_item(new MenuItem("REGEN LEVEL", MI_BUTTON, nullptr, MIV_EMPTY, menu_regen));
	//m_menu.add_item(new MenuItem("GAME CONFIG", MI_SUBMENU, &menu_g_cfg, MIV_EMPTY));
	//m_menu.add_item(new MenuItem("LEVEL CONFIG", MI_SUBMENU, &menu_l_cfg, MIV_EMPTY));
	//std::function<void()> menu_exit = [&running]() { running = false; };
	//m_menu.add_item(new MenuItem("EXIT", MI_BUTTON, nullptr, MIV_EMPTY, menu_exit));
}

MenuState::~MenuState()
{

}

void MenuState::update(float dt, float t)
{
	m_menu.update();
}

void MenuState::render()
{
	m_menu.render();
}