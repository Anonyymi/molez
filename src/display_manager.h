#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <string>
#include <vector>
#include <map>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;

namespace TextureManager
{
	struct Font;
}

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

	struct Camera
	{
		int x, y;					// xy-translation
		int speed;					// pixels per tick

		// TODO; Viewport dimensions (split-screen)
	};

	extern Window * ACTIVE_WINDOW;
	extern Camera * ACTIVE_CAMERA;
	extern std::map<std::string, Window *> LOADED_WINDOWS;
	extern std::map<std::string, Camera *> LOADED_CAMERAS;

	// Init
	void init();

	// Quit (clears memory)
	void quit();

	// Windows
	Window * load_window(
		const std::string & title,
		int width = 640,
		int height = 480,
		int scale = 1,
		bool fullscreen = false
	);
	void activate_window(const std::string & title);

	// Cameras
	Camera * load_camera(
		const std::string & identifier,
		int x = 0,
		int y = 0,
		int speed = 1
	);
	void activate_camera(const std::string & identifier);

	void render();
	void clear(const int32_t argb);
	void set_pixel(
		size_t x,
		size_t y,
		uint8_t r,
		uint8_t g,
		uint8_t b,
		uint8_t a = 255,
		bool grey = false
	);
	void set_rect(
		size_t x,
		size_t y,
		size_t w,
		size_t h,
		uint8_t r,
		uint8_t g,
		uint8_t b,
		uint8_t a = 255,
		bool grey = false
	);
	void set_text(
		size_t x,
		size_t y,
		size_t c_width,
		size_t c_height,
		std::string text,
		uint8_t r,
		uint8_t g,
		uint8_t b,
		TextureManager::Font * font
	);

}

#endif // DISPLAY_MANAGER_H