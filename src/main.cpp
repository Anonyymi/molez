#include <iostream>
#include "game.h"

int main(int argc, char * argv[])
{
	int return_code = 0;

	// Bootstrap game
	try
	{
		// Init game cfg
		GameConfig cfg;
		cfg.win_width = 640;
		cfg.win_height = 467;
		cfg.win_scale = 1;
		cfg.win_fullscreen = true;
		cfg.gfx_framerate = 60.0f;
		cfg.sfx_music_vol = 64;
		cfg.sfx_audio_vol = 40;
		cfg.phy_tickrate = 100.0f;
		cfg.phy_timestep = 1e-2f;

		// Create game
		Game game(
			cfg
		);

		// Run game, thread halts here
		game.run();
	}
	catch (const std::exception & e)
	{
		printf("::main exception! Message:\n%s", e.what());

		return_code = -1;
	}

	return return_code;
}