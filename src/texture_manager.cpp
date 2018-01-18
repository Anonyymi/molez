#include "texture_manager.h"
#include <iostream>
#include <fstream>
#define STB_IMAGE_IMPLEMENTATION
#include "3rdparty/stb_image.h"
#include "3rdparty/mlibc_log.h"
#include "3rdparty/json.hpp"

using json = nlohmann::json;

namespace TextureManager
{

	const std::string DATA_DIR_TEX = "./data/gfx/";
	const std::string DATA_DIR_FNT = "./data/fnt/";
	std::map<std::string, Texture *> LOADED_TEXTURES = std::map<std::string, Texture *>();
	std::map<std::string, Font *> LOADED_FONTS = std::map<std::string, Font *>();

	// Init
	void init()
	{
		// Init stb_image
		stbi_set_flip_vertically_on_load(false);

		// Pre-load textures
		load_texture("NULL.PNG");
		load_texture("VOID.PNG");
		load_texture("DIRT.PNG");
		load_texture("ROCK1.PNG");
		load_texture("OBSIDIAN.PNG");
		load_texture("MOSS.PNG");
		load_texture("WATER.PNG");
		load_texture("LAVA.PNG");

		// Pre-load fonts
		load_font("MOLEZ.JSON");

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

		mlibc_inf("TextureManager::quit().");
	}

	// Textures (png, jpg, tga, tiff, gif, etc..)
	Texture * const load_texture(const std::string & file_path)
	{
		std::string full_file_path = DATA_DIR_TEX + file_path;

		if (LOADED_TEXTURES.count(file_path) == 0)
		{
			Texture * texture = new Texture;
			texture->file_path = file_path;

			// Attempt to load the image
			unsigned char * data = NULL;
			data = stbi_load(full_file_path.c_str(), &texture->width, &texture->height, &texture->components, 4);

			if (data == NULL)
			{
				delete texture;
				mlibc_err("TextureManager::load_texture(%s). Error loading file into memory!", file_path.c_str());
				return nullptr;
			}

			mlibc_dbg("TextureManager::load_texture(%s). Loaded file into memory. w: %d, h: %d, c: %d", file_path.c_str(), texture->width, texture->height, texture->components);

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
		else
		{
			// This log entry locks up windows if it happens continuously.
			// TODO; In these cases, just throw an error
			//mlibc_err("TextureManager::sample_texture(%s). Error sampling texture!", file_path.c_str());
		}

		return 0;
	}

	// Fonts
	Font * const load_font(const std::string & file_path)
	{
		std::string full_file_path = DATA_DIR_FNT + file_path;

		if (LOADED_FONTS.count(file_path) == 0)
		{
			// Load font JSON file
			std::ifstream font_file(full_file_path, std::ifstream::binary);

			if (font_file.is_open() == false)
			{
				mlibc_err("TextureManager::load_font(%s). Error loading file into memory!", file_path.c_str());
				return nullptr;
			}

			json font_json;
			font_file >> font_json;
			font_file.close();

			// Attempt to load the font texture
			Font * font = new Font;
			font->file_path = file_path;
			std::string file_path_bitmap = font_json["file_path"].get<std::string>();
			font->texture = load_texture(file_path_bitmap);

			if (font->texture == nullptr)
			{
				delete font;
				mlibc_err("TextureManager::load_font(%s). Error loading texture file into memory!", file_path.c_str());
				return nullptr;
			}

			// Read the font properties into memory
			font->char_width = font_json["char_width"].get<int>();
			font->char_height = font_json["char_height"].get<int>();

			json char_json = font_json["char_map"];
			for (json::iterator it = char_json.begin(); it != char_json.end(); ++it)
			{
				CharIdx char_idx;

				// Get char map key & 2D vector values
				char key = it.key()[0];
				char_idx.key = key;
				char_idx.x = it.value()[0].get<int>();
				char_idx.y = it.value()[1].get<int>();

				// Read the key / value pair to memory for our font
				font->char_map[key] = char_idx;
			}

			// Store the loaded font into our map
			LOADED_FONTS[file_path] = font;

			mlibc_inf("TextureManager::load_font(%s). Success, converted loaded file into internal format.", file_path.c_str());
		}

		return LOADED_FONTS[file_path];
	}

}