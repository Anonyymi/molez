#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <string>
#include <vector>
#include <map>
#include <cstdint>

namespace TextureManager
{

	struct Texture
	{
		std::string file_path;
		int width, height, components;
		std::vector<int32_t> data;
	};

	struct CharIdx
	{
		char key;
		int x, y;
	};

	struct Font
	{
		std::string file_path;
		Texture * texture;
		int char_width, char_height;
		std::map<char, CharIdx> char_map;
	};

	extern const std::string DATA_DIR_TEX;
	extern const std::string DATA_DIR_FNT;
	extern std::map<std::string, Texture *> LOADED_TEXTURES;
	extern std::map<std::string, Font *> LOADED_FONTS;

	// Init
	void init();

	// Quit (clears memory)
	void quit();

	// Textures (png, jpg, tga, tiff, gif, etc..)
	Texture * const load_texture(const std::string & file_path);
	int32_t sample_texture(const std::string & file_path, int x, int y);

	// Fonts
	Font * const load_font(const std::string & file_path);

}

#endif // TEXTURE_MANAGER_H