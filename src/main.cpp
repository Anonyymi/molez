#include <iostream>
#include <ctime>
#include <SDL.h>
#include "3rdparty/mlibc_log.h"
#include "input_manager.h"
#include "display_manager.h"
#include "audio_manager.h"
#include "texture_manager.h"
#include "level.h"
#include "menu.h"
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

	// Init DisplayManager (molez resolution = 640x467 in DOS)
	DisplayManager::init();
	DisplayManager::load_window("molez", 640, 467, 1);
	DisplayManager::activate_window("molez");
	DisplayManager::clear(0x00000000);
	DisplayManager::load_camera("main", 0, 0, 5);
	DisplayManager::activate_camera("main");

	// Init AudioManager
	AudioManager::init();
	AudioManager::set_music_volume(64);
	//AudioManager::play_music("INGAME1.MUS");

	// Init TextureManager
	TextureManager::init();

	// Test level generation
	LevelConfig level_cfg;
	level_cfg.seed = 12345;
	level_cfg.type = L_EARTH;
	level_cfg.width = 1028;
	level_cfg.height = 1028;
	level_cfg.n_scale = 0.0075f;
	level_cfg.n_water = 48;
	level_cfg.n_lava = 16;

	Level level(level_cfg);
	level.generate();

	// Test menu system
	Menu menu("Molez");
	menu.add_item(new MenuItem("NEW GAME", MI_EMPTY, nullptr, nullptr));
	menu.add_item(new MenuItem("REGEN LEVEL", MI_EMPTY, nullptr, nullptr));
	menu.add_item(new MenuItem("GAME INFO", MI_EMPTY, nullptr, nullptr));
	menu.add_item(new MenuItem("EXIT", MI_EMPTY, nullptr, nullptr));

	// Center main camera
	DisplayManager::ACTIVE_CAMERA->x += level_cfg.width / 2;
	DisplayManager::ACTIVE_CAMERA->y -= level_cfg.height / 2;

	bool running = true;
	while (running)
	{
		// Level alter pixels
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

		// Level move camera
		if (InputManager::KBOARD[SDLK_w])
		{
			DisplayManager::ACTIVE_CAMERA->y += DisplayManager::ACTIVE_CAMERA->speed;
		}
		else if (InputManager::KBOARD[SDLK_s])
		{
			DisplayManager::ACTIVE_CAMERA->y -= DisplayManager::ACTIVE_CAMERA->speed;
		}

		if (InputManager::KBOARD[SDLK_d])
		{
			DisplayManager::ACTIVE_CAMERA->x += DisplayManager::ACTIVE_CAMERA->speed;
		}
		else if (InputManager::KBOARD[SDLK_a])
		{
			DisplayManager::ACTIVE_CAMERA->x -= DisplayManager::ACTIVE_CAMERA->speed;
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

		// Clear screen fbo
		DisplayManager::clear(0x00666666);

		// Update level
		level.update();

		// Update menu
		menu.update();

		// Render level
		level.render();

		// Render menu
		DisplayManager::ACTIVE_CAMERA = nullptr;
		menu.render();
		DisplayManager::activate_camera("main");

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
					int x_t = sdl_event.motion.x / DisplayManager::ACTIVE_WINDOW->scale;
					int y_t = sdl_event.motion.y / DisplayManager::ACTIVE_WINDOW->scale;

					// Negate camera translation
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