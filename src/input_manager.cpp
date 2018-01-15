#include "input_manager.h"
#include <SDL2/SDL.h>
#include "3rdparty/mlibc_log.h"

namespace InputManager
{

	// Mouse
	int MOUSE_X = 0, MOUSE_Y = 0;
	bool MOUSE_L = false;
	bool MOUSE_R = false;
	bool MOUSE_M = false;

	// Keyboard
	std::map<int, bool> KBOARD;

	// Init
	void init()
	{
		mlibc_inf("InputManager::init().");
	}
	
	// Quit (clears memory)
	void quit()
	{

	}

	// Process SDL2 input
	bool SDLInput(){
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

		return true;
	}

}