#include "display_manager.h"
#include <SDL2/SDL.h>
#include "3rdparty/mlibc_log.h"
#include "texture_manager.h"
#include "math.h"

namespace DisplayManager
{

	Window * ACTIVE_WINDOW = NULL;
	Camera * ACTIVE_CAMERA = NULL;
	std::map<std::string, Window *> LOADED_WINDOWS = std::map<std::string, Window *>();
	std::map<std::string, Camera *> LOADED_CAMERAS = std::map<std::string, Camera *>();

	// Init
	void init()
	{
		mlibc_inf("DisplayManager::init().");
	}

	// Quit (clears memory)
	void quit()
	{
		// Clean up fbos/windows
		for (auto _w : LOADED_WINDOWS)
		{
			Window * w = _w.second;

			delete w->framebuffer;
			SDL_DestroyTexture(w->texture);
			SDL_DestroyRenderer(w->renderer);
			SDL_DestroyWindow(w->handle);
			delete w;

			mlibc_inf("DisplayManager::quit(). Destroyed Window instance.");
		}

		mlibc_inf("DisplayManager::quit().");
	}

	// Windows
	Window * load_window(
		const std::string & title,
		int width,
		int height,
		int scale,
		bool fullscreen
	)
	{
		if (LOADED_WINDOWS.count(title) == 0)
		{
			Window * window = new Window;

			window->title = title;
			window->width = width;
			window->height = height;
			window->scale = scale;

			window->handle = SDL_CreateWindow(
				title.c_str(),
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				width * scale,
				height * scale,
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | ((fullscreen) ? SDL_WINDOW_FULLSCREEN : 0)
			);

			if (window->handle == NULL)
			{
				delete window;
				mlibc_err("DisplayManager::load_window(%s). Error creating a new SDL_Window instance!", title.c_str());
				return nullptr;
			}

			window->renderer = SDL_CreateRenderer(
				window->handle,
				-1,
				SDL_RENDERER_ACCELERATED
			);

			if (window->renderer == NULL)
			{
				SDL_DestroyWindow(window->handle);
				delete window;
				mlibc_err("DisplayManager::load_window(%s). Error creating a new SDL_Renderer instance!", title.c_str());
				return nullptr;
			}

			window->texture = SDL_CreateTexture(
				window->renderer,
				SDL_PIXELFORMAT_ARGB8888,
				SDL_TEXTUREACCESS_STATIC,
				width,
				height
			);

			if (window->texture == NULL)
			{
				SDL_DestroyRenderer(window->renderer);
				SDL_DestroyWindow(window->handle);
				delete window;
				mlibc_err("DisplayManager::load_window(%s). Error creating a new SDL_Texture instance!", title.c_str());
				return nullptr;
			}

			window->framebuffer = new int32_t[width * height];
			if (window->framebuffer == nullptr)
			{
				SDL_DestroyTexture(window->texture);
				SDL_DestroyRenderer(window->renderer);
				SDL_DestroyWindow(window->handle);
				delete window;
				mlibc_err("DisplayManager::load_window(%s). Error allocating a new framebuffer object!", title.c_str());
				return nullptr;
			}

			LOADED_WINDOWS[title] = window;

			mlibc_inf("DisplayManager::load_window(%s). Loaded a new window into memory.", title.c_str());
		}

		return LOADED_WINDOWS[title];
	}

	void activate_window(const std::string & title)
	{
		if (LOADED_WINDOWS.count(title) > 0)
		{
			ACTIVE_WINDOW = LOADED_WINDOWS[title];
		}
		else
		{
			mlibc_err("DisplayManager::activate_window(%s). Error activating window!", title.c_str());
		}
	}

	// Cameras
	Camera * load_camera(
		const std::string & identifier,
		int x,
		int y,
		int speed
	)
	{
		if (LOADED_CAMERAS.count(identifier) == 0)
		{
			Camera * camera = new Camera();

			camera->x = x;
			camera->y = y;
			camera->speed = speed;

			LOADED_CAMERAS[identifier] = camera;

			mlibc_inf("DisplayManager::load_camera(%s). Loaded a new camera into memory.", identifier.c_str());
		}

		return LOADED_CAMERAS[identifier];
	}

	void activate_camera(const std::string & identifier)
	{
		if (LOADED_CAMERAS.count(identifier) > 0)
		{
			ACTIVE_CAMERA = LOADED_CAMERAS[identifier];
		}
		else
		{
			mlibc_err("DisplayManager::activate_camera(%s). Error activating camera!", identifier.c_str());
		}
	}

	void render()
	{
		if (ACTIVE_WINDOW != nullptr)
		{
			SDL_UpdateTexture(
				ACTIVE_WINDOW->texture,
				NULL,
				ACTIVE_WINDOW->framebuffer,
				ACTIVE_WINDOW->width * sizeof(int32_t)
			);
			SDL_RenderClear(ACTIVE_WINDOW->renderer);
			SDL_RenderCopy(ACTIVE_WINDOW->renderer, ACTIVE_WINDOW->texture, NULL, NULL);
			SDL_RenderPresent(ACTIVE_WINDOW->renderer);
		}
		else
		{
			mlibc_err("DisplayManager::render(). Error, ACTIVE_WINDOW is pointing to NULL!");
		}
	}

	void clear(const int32_t argb)
	{
		if (ACTIVE_WINDOW != nullptr)
		{
			size_t resolution = static_cast<size_t>(ACTIVE_WINDOW->width * ACTIVE_WINDOW->height);

			for (size_t i = 0; i < resolution; i++)
				ACTIVE_WINDOW->framebuffer[i] = argb;
		}
		else
		{
			mlibc_err("DisplayManager::clear(). Error, ACTIVE_WINDOW is pointing to NULL!");
		}
	}

	void set_pixel(
		int x,
		int y,
		uint8_t r,
		uint8_t g,
		uint8_t b,
		uint8_t a,
		bool grey
	)
	{
		if (ACTIVE_WINDOW != nullptr)
		{
			// Calculate camera translation
			if (ACTIVE_CAMERA != nullptr)
			{
				// Camera translation
				x -= ACTIVE_CAMERA->x;
				y += ACTIVE_CAMERA->y;

				// Window offset
				x += ACTIVE_WINDOW->width / 2;
				y += ACTIVE_WINDOW->height / 2;
			}

			// Prevent altering memory outside fbo
			if (x < 0 || x >= ACTIVE_WINDOW->width || y < 0 || y >= ACTIVE_WINDOW->height)
				return;

			// Alpha, mix between old and new color
			if (a < 255)
			{
				// Get old ARGB value + decode to RGB components
				auto argb_ = ACTIVE_WINDOW->framebuffer[x + y * ACTIVE_WINDOW->width];
				auto r_ = (argb_ & 0x00FF0000) >> 16;
				auto g_ = (argb_ & 0x0000FF00) >> 8;
				auto b_ = (argb_ & 0x000000FF);

				// Linearly interpolate between old and new RGB components based on input alpha
				r = Math::lerp(r_, r, a);
				g = Math::lerp(g_, g, a);
				b = Math::lerp(b_, b, a);
			}

			// Greyscale rendering
			if (grey)
			{
				// Get max color component value
				uint8_t c = std::max(r, std::max(g, b));

				// Set all components to same value
				r = c;
				g = c;
				b = c;
			}

			// Encode new ARGB component values back hex
			auto argb = ((a << 24) | (r << 16) | (g << 8) | b);

			ACTIVE_WINDOW->framebuffer[x + y * ACTIVE_WINDOW->width] = argb;
		}
		else
		{
			mlibc_err("DisplayManager::set_pixel(). Error, ACTIVE_WINDOW is pointing to NULL!");
		}
	}

	void set_rect(
		int x,
		int y,
		int w,
		int h,
		uint8_t r,
		uint8_t g,
		uint8_t b,
		uint8_t a,
		bool grey
	)
	{
		if (ACTIVE_WINDOW != nullptr)
		{
			// Translate width and height by x and y
			w += x;
			h += y;

			// Render the rectangle
			for (int i = y; i < h; i++)
			{
				for (int j = x; j < w; j++)
				{
					set_pixel(j, i, r, g, b, a, grey);
				}
			}
		}
		else
		{
			mlibc_err("DisplayManager::set_rect(). Error, ACTIVE_WINDOW is pointing to NULL!");
		}
	}

	void set_text(
		int x,
		int y,
		int c_width,
		int c_height,
		std::string text,
		uint8_t r,
		uint8_t g,
		uint8_t b,
		TextureManager::Font * font
	)
	{
		if (ACTIVE_WINDOW != nullptr)
		{
			// Render each character
			int c_idx = 0;
			for (char & c : text)
			{
				// Get char texture index
				TextureManager::CharIdx char_idx = font->char_map[c];

				// Skip rendering if it's a space
				if (isspace(char_idx.key))
				{
					c_idx++;
					continue;
				}

				// Calculate font size <-> char size ratio
				float char_rat_w = static_cast<float>(font->char_width) / static_cast<float>(c_width);
				float char_rat_h = static_cast<float>(font->char_height) / static_cast<float>(c_height);

				for (int i = 0; i < c_width; i++)
				{
					for (int j = 0; j < c_height; j++)
					{
						// Calculate texcoords and sample the pixel + scale according to size diff
						int char_tex_x = char_idx.x * font->char_width + static_cast<int>(j * char_rat_w);
						int char_tex_y = char_idx.y * font->char_height + static_cast<int>(i * char_rat_h);
						auto char_argb = TextureManager::sample_texture(font->texture->file_path, char_tex_x, char_tex_y);

						// Decode char ARGB hex value into component(s)
						auto char_a = (char_argb & 0xFF000000) >> 24;

						// Calculate on-screen fbo metrics
						int char_x = x + j + (c_width * c_idx);
						int char_y = y + i;

						// Set pixel
						set_pixel(char_x, char_y, r, g, b, char_a, false);
					}
				}

				// increase char idx
				c_idx++;
			}
		}
		else
		{
			mlibc_err("DisplayManager::set_text(). Error, ACTIVE_WINDOW is pointing to NULL!");
		}
	}

}