#include "texture_manager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"
#include "3rdparty/mlibc_log.h"

namespace TextureManager
{

	extern const std::string DATA_DIR = "./data/gfx/";
	std::map<std::string, Texture *> LOADED_TEXTURES = std::map<std::string, Texture *>();

	// Init
	void init()
	{
		// Init stb_image
		stbi_set_flip_vertically_on_load(false);

		// Pre-load textures
		load_texture("NULL.PNG");
		load_texture("DIRT.PNG");
		load_texture("ROCK1.PNG");
		load_texture("OBSIDIAN.PNG");
		load_texture("MOSS.PNG");
		load_texture("WATER.PNG");
		load_texture("LAVA.PNG");

		mlibc_inf("TextureManager::init().");
	}

	// Quit (clears memory)
	void quit()
	{
		for (auto t : LOADED_TEXTURES)
		{
			mlibc_inf("TextureManager::quit(). Destroy texture.");

			delete t.second;
		}
	}

	// Textures (png, jpg, tga, tiff, gif, etc..)
	Texture * const load_texture(const std::string & file_path)
	{
		std::string full_file_path = DATA_DIR + file_path;

		if (LOADED_TEXTURES.count(file_path) == 0)
		{
			Texture * texture = new Texture;

			// Attempt to load the image
			unsigned char * data = NULL;
			data = stbi_load(full_file_path.c_str(), &texture->width, &texture->height, &texture->components, 4);

			if (data == NULL)
			{
				delete texture;
				mlibc_err("TextureManager::load_texture(%s). Error loading file into memory!", file_path.c_str());
				return nullptr;
			}

			mlibc_inf("TextureManager::load_texture(%s). Loaded file into memory. w: %d, h: %d, c: %d", file_path.c_str(), texture->width, texture->height, texture->components);

			// Convert image to data to our argb format
			texture->data = std::vector<int32_t>(texture->width * texture->height);
			for (int32_t i = 0; i < texture->height; i++)
			{
				for (int32_t j = 0; j < texture->width; j++)
				{
					// Get RGBA values
					int32_t index = j + i * texture->width;
					uint8_t r = data[(index * 4 + 0) % (texture->width * texture->height * 4)];
					uint8_t g = data[(index * 4 + 1) % (texture->width * texture->height * 4)];
					uint8_t b = data[(index * 4 + 2) % (texture->width * texture->height * 4)];
					uint8_t a = 255;
					if (texture->components >= 4)
					{
						a = data[(index * 4 + 3) % (texture->width * texture->height * 4)];
					}

					// Store the RGBA values to our data vector as ARGB
					texture->data[j + i * texture->width] = ((a << 24) | (r << 16) | (g << 8) | b);
				}
			}

			// Clear original texture data from memory
			stbi_image_free(data);

			// Store the loaded texture into our map
			LOADED_TEXTURES[file_path] = texture;

			mlibc_inf("TextureManager::load_texture(%s). Success, converted loaded file into internal format.", file_path.c_str());
		}

		return LOADED_TEXTURES[file_path];
	}

	int32_t sample_texture(const std::string & file_path, int x, int y)
	{
		if (LOADED_TEXTURES.count(file_path) > 0)
		{
			Texture * texture = LOADED_TEXTURES[file_path];

			return texture->data[(x % texture->width) + (y % texture->height) * texture->width];
		}

		return 0;
	}

}