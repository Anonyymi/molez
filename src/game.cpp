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
	m_state(nullptr),
	m_phys()
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
		m_cfg.win_scale,
		m_cfg.win_fullscreen
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
	level_cfg.dirt_n = 180;
	level_cfg.object_n = 32;
	level_cfg.water_n = 16;
	level_cfg.lava_n = 0;
	Level * level = new Level(level_cfg);

	// Init GameState to MenuState
	setState(new MenuState(this, level));
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

// Fix your timestep!
// Article; https://gafferongames.com/post/fix_your_timestep/
GameRunState_t Game::run()
{
	// If m_run_state == GRS_RUNNING, don't continue
	if (m_run_state == GRS_RUNNING)
	{
		mlibc_err("Game::run() error! m_run_state is already GRS_RUNNING, please make sure the game is stopped first!");

		m_run_state = GRS_ERROR;
		return m_run_state;
	}
	m_run_state = GRS_RUNNING;

	// Fixed timestep physics prepare
	m_phys.t = 0.0f;
	m_phys.dt = 1.0f / m_cfg.phy_tickrate;
	m_phys.t_curr = getTimeInSec();				// hires_time_in_seconds()
	m_phys.t_acc = 0.0f;						// accumulator
	m_phys.s_prev = 0.0f;						// previous state
	m_phys.s_curr = 0.0f;						// current state

	// Run the game and physics
	while (m_run_state == GRS_RUNNING)
	{
		// Fixed timestep, frame prepare
		float t_new = getTimeInSec();			// newTime
		float t_frame = t_new - m_phys.t_curr;	// frame time in seconds
		if (t_frame > 0.25f)
			t_frame = 0.25f;
		m_phys.t_curr = t_new;

		m_phys.t_acc += t_frame;

		// Fixed timestep, simulate until acc decreases to dt
		while (m_phys.t_acc >= m_phys.dt)
		{
			m_phys.s_prev = m_phys.s_curr;
			update(m_phys.s_curr, m_phys.t, m_phys.dt);
			m_phys.t += m_phys.dt;
			m_phys.t_acc -= m_phys.dt;
		}

		// Fixed timestep, frame end
		m_phys.alpha = m_phys.t_acc / m_phys.dt;	// interpolation value for state between states
		m_phys.s_lerp = m_phys.s_curr * m_phys.alpha + m_phys.s_prev * (1.0f - m_phys.alpha);

		// Render with current interpolated frame state
		render(m_phys.s_lerp);
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

void Game::update(float state, float t, float dt)
{
	// If m_state == NULL, don't continue
	if (m_state == nullptr)
	{
		mlibc_err("Game::update(dt:%.3f, t:%.3f) error! m_state points to NULL!", dt, t);
		return;
	}

	input();
	m_state->update(state, t, dt);
}

void Game::render(float state)
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
	m_state->render(state);

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

void Game::setCfg(GameConfig cfg)
{
	m_cfg = cfg;
}

GameConfig & Game::getCfg()
{
	return m_cfg;
}

void Game::setState(GameState * state)
{
	// Save previous state to temp var
	GameState * temp = m_state;

	// Switch to new state
	m_state = state;

	// Destroy previous state
	delete temp;
}

GameState * const Game::getState()
{
	return m_state;
}

PhysicsState Game::getPhysState() const
{
	return m_phys;
}

float Game::getTimeInSec() const
{
	return static_cast<float>(SDL_GetTicks()) / 1000;
}