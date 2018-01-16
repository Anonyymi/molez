#include "game.h"
#include <exception>
#include <SDL2/SDL.h>
#include "3rdparty/mlibc_log.h"
#include "input_manager.h"
#include "display_manager.h"
#include "audio_manager.h"
#include "texture_manager.h"
#include "game_state.h"
#include "menu_state.h"
#include "level.h"

mlibc_log_logger * mlibc_log_instance = NULL;

Game::Game(
	GameConfig & cfg
) :
	m_cfg(cfg),
	m_run_state(GRS_STOPPED),
	m_state(nullptr)
{
	int return_code;

	// Init mlibc_log
	return_code = mlibc_log_init(MLIBC_LOG_LEVEL_DBG);
	if (return_code != MLIBC_LOG_CODE_OK)
	{
		throw std::runtime_error("Game::Game(), mlibc_log_init error: " + return_code);
	}
	mlibc_inf("Game::Game(), mlibc_log Initialized successfully.");

	// Init SDL2
	return_code = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (return_code != 0)
	{
		throw std::runtime_error("Game::Game(), SDL_Init error: " + std::string(SDL_GetError()));
	}
	mlibc_inf("Game::Game(), SDL2 Initialized successfully.");

	// Init InputManager
	InputManager::init();

	// Init DisplayManager (molez resolution = 640x467 in DOS)
	DisplayManager::init();
	DisplayManager::load_window(
		"molez",
		m_cfg.win_width,
		m_cfg.win_height,
		m_cfg.win_scale
	);
	DisplayManager::activate_window("molez");
	DisplayManager::clear(0x00000000);
	DisplayManager::load_camera("main", 0, 0, 1);
	DisplayManager::activate_camera("main");

	// Init AudioManager
	AudioManager::init();
	AudioManager::set_music_volume(m_cfg.sfx_music_vol);
	AudioManager::set_audio_volume(m_cfg.sfx_audio_vol);

	// Init TextureManager
	TextureManager::init();

	// Init level
	LevelConfig level_cfg;
	level_cfg.seed = 12345;
	level_cfg.type = L_EARTH;
	level_cfg.width = DisplayManager::ACTIVE_WINDOW->width;
	level_cfg.height = DisplayManager::ACTIVE_WINDOW->height;
	level_cfg.n_scale = 0.005f;
	level_cfg.n_water = 40;
	level_cfg.n_lava = 12;
	Level * level = new Level(level_cfg);

	// Init GameState to MenuState
	setGameState(new MenuState(this, level));
}

Game::~Game()
{
	// Destroy GameState
	delete m_state;

	// Cleanup memory
	TextureManager::quit();
	AudioManager::quit();
	DisplayManager::quit();
	InputManager::quit();
	SDL_Quit();
	mlibc_log_free();
}

GameRunState_t Game::run()
{
	// If m_run_state == GRS_RUNNING, don't continue
	if (m_run_state == GRS_RUNNING)
	{
		mlibc_err("Game::run() error! m_run_state is already GRS_RUNNING, please make sure the game is stopped first!");

		m_run_state = GRS_ERROR;
		return m_run_state;
	}

	// Game loop
	// TODO; Fixed timestep physics!
	m_run_state = GRS_RUNNING;
	while (m_run_state == GRS_RUNNING)
	{
		update(m_cfg.phy_timestep, 1.0f);
		render();
		input();
	}

	return m_run_state;
}

void Game::stop()
{
	// If m_run_state == GRS_STOPPED, don't continue
	if (m_run_state == GRS_STOPPED)
	{
		mlibc_err("Game::stop() error! m_run_state is already GRS_STOPPED, please make sure the game is started first!");

		m_run_state = GRS_ERROR;
		return;
	}

	m_run_state = GRS_STOPPED;
}

void Game::update(float dt, float t)
{
	// If m_state == NULL, don't continue
	if (m_state == nullptr)
	{
		mlibc_err("Game::update(dt:%.3f, t:%.3f) error! m_state points to NULL!", dt, t);
		return;
	}

	m_state->update(dt, t);
}

void Game::render()
{
	// If m_state == NULL, don't continue
	if (m_state == nullptr)
	{
		mlibc_err("Game::render() error! m_state points to NULL!");
		return;
	}

	// Clear window FBO
	DisplayManager::clear(0x00333333);

	// Render GameState
	m_state->render();

	// Update window FBO
	DisplayManager::render();

}

void Game::input()
{
	SDL_Event sdl_event;

	while (SDL_PollEvent(&sdl_event))
	{
		switch (sdl_event.type)
		{
			case SDL_KEYDOWN:
			{
				InputManager::KBOARD[sdl_event.key.keysym.sym] = true;
			} break;
			case SDL_KEYUP:
			{
				InputManager::KBOARD[sdl_event.key.keysym.sym] = false;
			} break;
			case SDL_MOUSEBUTTONDOWN:
			{
				switch (sdl_event.button.button)
				{
					case SDL_BUTTON_LEFT:	InputManager::MOUSE_L = true; break;
					case SDL_BUTTON_RIGHT:	InputManager::MOUSE_R = true; break;
					case SDL_BUTTON_MIDDLE:	InputManager::MOUSE_M = true; break;
				}
			} break;
			case SDL_MOUSEBUTTONUP:
			{
				switch (sdl_event.button.button)
				{
					case SDL_BUTTON_LEFT:	InputManager::MOUSE_L = false; break;
					case SDL_BUTTON_RIGHT:	InputManager::MOUSE_R = false; break;
					case SDL_BUTTON_MIDDLE:	InputManager::MOUSE_M = false; break;
				}
			} break;
			case SDL_MOUSEMOTION:
			{
				int x_t = sdl_event.motion.x / DisplayManager::ACTIVE_WINDOW->scale;
				int y_t = sdl_event.motion.y / DisplayManager::ACTIVE_WINDOW->scale;

				// Negate camera translation for mouse xy
				if (DisplayManager::ACTIVE_CAMERA != nullptr)
				{
					// Camera translation
					x_t += DisplayManager::ACTIVE_CAMERA->x;
					y_t -= DisplayManager::ACTIVE_CAMERA->y;

					// Window offset
					x_t -= DisplayManager::ACTIVE_WINDOW->width / 2;
					y_t -= DisplayManager::ACTIVE_WINDOW->height / 2;
				}

				InputManager::MOUSE_X = x_t;
				InputManager::MOUSE_Y = y_t;
			} break;
			case SDL_QUIT:
			{
				m_run_state = GRS_STOPPED;
			} break;
		}
	}
}

void Game::setGameState(GameState * state)
{
	// Save previous state to temp var
	GameState * temp = m_state;

	// Switch to new state
	m_state = state;

	// Destroy previous state
	delete temp;
}

GameState * const Game::getGameState()
{
	return m_state;
}
