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
		std::string filepath;
		int width, height, components;
		std::vector<int32_t> data;
	};

	extern const std::string DATA_DIR;
	extern std::map<std::string, Texture *> LOADED_TEXTURES;

	// Init
	void init();

	// Quit (clears memory)
	void quit();

	// Textures (png, jpg, tga, tiff, gif, etc..)
	Texture * const load_texture(const std::string & file_path);
	uint32_t sample_texture(const std::string & file_path, int x, int y);

}

#endif // TEXTURE_MANAGER_H