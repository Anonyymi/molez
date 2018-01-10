#include <iostream>
#include <ctime>
#include <SDL.h>
#include "3rdparty/mlibc_log.h"
#include "input_manager.h"
#include "display_manager.h"
#include "audio_manager.h"
#include "texture_manager.h"
#include "level.h"
#include "math.h"

mlibc_log_logger * mlibc_log_instance = NULL;

int main(int argc, char * argv[])
{
	int return_code = 0;

	// Init mlibc_log
	return_code = mlibc_log_init(MLIBC_LOG_LEVEL_DBG);
	if (return_code != MLIBC_LOG_CODE_OK)
	{
		printf("::main(). mlibc_log_init Error: %d", return_code);
		return return_code;
	}
	mlibc_inf("::main(). mlibc_log Initialized successfully.");

	// Init SDL2
	return_code = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (return_code != 0)
	{
		mlibc_err("::main(). SDL_Init Error: %s", SDL_GetError());
		return return_code;
	}
	mlibc_inf("::main(). SDL2 Initialized successfully.");

	// Init InputManager
	InputManager::init();

	// Init DisplayManager
	DisplayManager::init();
	DisplayManager::load_window("molez", 640, 360, 2);
	DisplayManager::activate_window("molez");
	DisplayManager::clear(0x00000000);

	// Init AudioManager
	AudioManager::init();
	AudioManager::set_music_volume(64);
	AudioManager::load_music("INGAME1.MUS");
	AudioManager::play_music("INGAME1.MUS");

	// Init TextureManager
	TextureManager::init();

	// Test level generation
	LevelConfig level_cfg;
	level_cfg.seed = 12345;
	level_cfg.type = L_EARTH;
	level_cfg.width = DisplayManager::ACTIVE_WINDOW->width;
	level_cfg.height = DisplayManager::ACTIVE_WINDOW->height;
	level_cfg.n_scale = 0.0099f;
	level_cfg.n_water = 48;
	level_cfg.n_lava = 16;

	Level level(level_cfg);
	level.generate();

	bool running = true;
	while (running)
	{
		// Level draw pixels
		if (InputManager::MOUSE_L)
		{
			level.alter(M_WATER, 8, InputManager::MOUSE_X, InputManager::MOUSE_Y);
		}
		else if (InputManager::MOUSE_M)
		{
			level.alter(M_LAVA, 8, InputManager::MOUSE_X, InputManager::MOUSE_Y);
		}
		else if (InputManager::MOUSE_R)
		{
			level.alter(M_VOID, 12, InputManager::MOUSE_X, InputManager::MOUSE_Y);
		}

		// Program exit
		if (InputManager::KBOARD[SDLK_ESCAPE])
		{
			running = false;
		}

		// Level reset
		if (InputManager::KBOARD[SDLK_r])
		{
			level.regenerate((uint32_t)time(NULL));
		}

		// Update level
		level.update();

		// Render level
		level.render();

		// Update screen fbo
		DisplayManager::render();

		// Handle SDL events
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
					InputManager::MOUSE_X = sdl_event.motion.x / DisplayManager::ACTIVE_WINDOW->scale;
					InputManager::MOUSE_Y = sdl_event.motion.y / DisplayManager::ACTIVE_WINDOW->scale;
				} break;
				case SDL_QUIT:
				{
					running = false;
				} break;
			}
		}

		SDL_Delay(10);
	}

	// Quit gracefully
	TextureManager::quit();
	AudioManager::quit();
	DisplayManager::quit();
	InputManager::quit();
	SDL_Quit();
	mlibc_log_free();

	return return_code;
}