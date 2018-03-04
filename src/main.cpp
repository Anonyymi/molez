#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <SDL2/SDL.h>
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
		cfg.n_players = 2;

		// Setup player controller bindings
		cfg.c_players = std::vector<std::map<int, std::string>>();

		// Player 1
		std::map<int, std::string> p1;
		p1[SDLK_LEFT] = "left";
		p1[SDLK_RIGHT] = "right";
		p1[SDLK_UP] = "up";
		p1[SDLK_DOWN] = "down";
		p1[SDLK_RCTRL] = "jump";
		p1[SDLK_RETURN] = "fire";
		p1[SDLK_BACKSPACE] = "reload";
		p1[SDLK_RSHIFT] = "change";

		// Player 2
		std::map<int, std::string> p2;
		p2[SDLK_a] = "left";
		p2[SDLK_d] = "right";
		p2[SDLK_w] = "up";
		p2[SDLK_s] = "down";
		p2[SDLK_SPACE] = "jump";
		p2[SDLK_f] = "fire";
		p2[SDLK_r] = "reload";
		p2[SDLK_y] = "change";

		cfg.c_players.push_back(p1);
		cfg.c_players.push_back(p2);

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