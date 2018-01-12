#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <string>
#include <map>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;

namespace DisplayManager
{

	struct Window
	{
		std::string title;
		int width, height, scale;
		SDL_Window * handle;
		SDL_Renderer * renderer;
		SDL_Texture * texture;
		int32_t * framebuffer;
	};

	extern Window * ACTIVE_WINDOW;
	extern std::map<std::string, Window *> LOADED_WINDOWS;

	// Init
	void init();

	// Quit (clears memory)
	void quit();

	// Windows
	Window * load_window(
		const std::string & title,
		int width = 640,
		int height = 480,
		int scale = 1
	);
	void activate_window(const std::string & title);
	void render();
	void clear(const int32_t argb);
	void set_pixel(size_t x, size_t y, uint8_t r, uint8_t g, uint8_t b);

}

#endif // DISPLAY_MANAGER_H