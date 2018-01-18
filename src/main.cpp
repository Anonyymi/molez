#include <iostream>
#include <fstream>
#include "3rdparty/json.hpp"
#include "game.h"

using json = nlohmann::json;

int main(int argc, char * argv[])
{
	int return_code = 0;

	// Bootstrap game
	try
	{
		// Read game cfg from JSON file
		std::ifstream cfg_file("./data/GAME_CFG.JSON", std::ifstream::binary);

		if (cfg_file.is_open() == false)
		{
			printf("::main(). Error loading game cfg file into memory!");
			return -1;
		}

		json cfg_json;
		cfg_file >> cfg_json;
		cfg_file.close();

		// Init game cfg
		GameConfig cfg;
		cfg.win_width = cfg_json["window"]["width"].get<int>();
		cfg.win_height = cfg_json["window"]["height"].get<int>();
		cfg.win_scale = cfg_json["window"]["scale"].get<int>();
		cfg.win_fullscreen = cfg_json["window"]["fullscreen"].get<bool>();
		cfg.gfx_framerate = cfg_json["graphics"]["framerate"].get<float>();
		cfg.sfx_music_vol = cfg_json["audio"]["music_vol"].get<int>();
		cfg.sfx_audio_vol = cfg_json["audio"]["audio_vol"].get<int>();
		cfg.phy_tickrate = cfg_json["physics"]["tickrate"].get<float>();

		// Create game
		Game game(
			cfg
		);

		// Run game, thread halts here
		game.run();
	}
	catch (const std::runtime_error & e)
	{
		printf("::main runtime_error! Message:\n%s", e.what());

		return_code = -1;
	}
	catch (const std::exception & e)
	{
		printf("::main exception! Message:\n%s", e.what());

		return_code = -1;
	}

	return return_code;
}