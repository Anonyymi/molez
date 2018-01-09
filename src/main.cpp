#include <iostream>
#include <ctime>
#include <SDL.h>
#include "3rdparty/mlibc_log.h"
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
	Level level(L_EARTH, DisplayManager::ACTIVE_WINDOW->width, DisplayManager::ACTIVE_WINDOW->height, 1234);
	level.generate();

	// Keep track of mouse
	int mouseX = 0, mouseY = 0;
	bool mouseL = false, mouseR = false, mouseM = false;

	bool running = true;
	while (running)
	{
		// Level draw pixels
		if (mouseL)
		{
			level.alter(M_WATER, 8, mouseX, mouseY);
		}
		else if (mouseM)
		{
			level.alter(M_LAVA, 8, mouseX, mouseY);
		}
		else if (mouseR)
		{
			level.alter(M_VOID, 12, mouseX, mouseY);
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
					switch (sdl_event.key.keysym.sym)
					{
						case SDLK_ESCAPE: running = false; break;
						case SDLK_r: level.regenerate(time(NULL)); break;
					} break;
				} break;
				case SDL_MOUSEBUTTONDOWN:
				{
					switch (sdl_event.button.button)
					{
						case SDL_BUTTON_LEFT:
						{
							mouseL = true;
						} break;
						case SDL_BUTTON_RIGHT:
						{
							mouseR = true;
						} break;
						case SDL_BUTTON_MIDDLE:
						{
							mouseM = true;
						} break;
					}
				} break;
				case SDL_MOUSEBUTTONUP:
				{
					switch (sdl_event.button.button)
					{
						case SDL_BUTTON_LEFT:
						{
							mouseL = false;
						} break;
						case SDL_BUTTON_RIGHT:
						{
							mouseR = false;
						} break;
						case SDL_BUTTON_MIDDLE:
						{
							mouseM = false;
						} break;
					}
				} break;
				case SDL_MOUSEMOTION:
				{
					mouseX = sdl_event.motion.x / DisplayManager::ACTIVE_WINDOW->scale;
					mouseY = sdl_event.motion.y / DisplayManager::ACTIVE_WINDOW->scale;

					//mlibc_inf("MouseX: %d, MouseY: %d", mouseX, mouseY);
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
	AudioManager::quit();
	DisplayManager::quit();
	SDL_Quit();
	mlibc_log_free();

	return return_code;
}