#include "input_manager.h"
#include "3rdparty/mlibc_log.h"

#ifdef __linux__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

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
					InputManager::MOUSE_X = sdl_event.motion.x;
					InputManager::MOUSE_Y = sdl_event.motion.y;
				} break;
				case SDL_QUIT:
				{
					return false;
				} break;
			}

		}
		return true;
	}
	// Quit (clears memory)
	void quit()
	{

	}

}