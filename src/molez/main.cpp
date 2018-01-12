#include <iostream>
#include <ctime>
#include "3rdparty/mlibc_log.h"
#include "input_manager.h"
#include "display_manager.h"
#include "audio_manager.h"
#include "texture_manager.h"
#include "level.h"
#include "math.h"
#include "Hebi.hpp"
#include "Tick.hpp"
#include <time.h>
#include <unistd.h>

#ifdef __linux__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

mlibc_log_logger * mlibc_log_instance = NULL;


double gameSpeed = 1.0;

LevelConfig level_cfg;
EngineConfig engine_cfg;
Hebi engine;



bool running = true;



bool gameLoop(){

	clock_t startClock;
	clock_t tickStartClock;
	clock_t endClock;
	Tick tick;


	
	tickStartClock = clock();
	startClock = clock();
	while (running)
	{
		// Handle SDL events
		if(!InputManager::SDLInput())
			running = false;
		endClock = clock();
		float timems = ((float)(endClock - startClock)/(float)CLOCKS_PER_SEC)*1000.0;
		float tickTimems = ((float)(endClock - tickStartClock)/(float)CLOCKS_PER_SEC)*1000.0;
		if(timems>=1000/engine.maxfps()){
			DisplayManager::render();
			startClock = clock();
			
		}
		if(tickTimems >= 1000/engine.tickrate()){
			if(!engine.nextTick(&tick)){
				running = false;
			}
			tickStartClock = clock();
		}
	}

	return true;
}

int main(int argc, char * argv[])
{
	srand(time(NULL));

	int return_code = 0;

	// Init mlibc_log
	return_code = mlibc_log_init(MLIBC_LOG_LEVEL_DBG);
	if (return_code != MLIBC_LOG_CODE_OK)
	{
		printf("::main(). mlibc_log_init Error: %d", return_code);
		return return_code;
	}
	mlibc_inf("::main(). mlibc_log Initialized successfully.");

	// Init SDL2
	return_code = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	if (return_code != 0)
	{
		mlibc_err("::main(). SDL_Init Error: %s", SDL_GetError());
		return return_code;
	}
	mlibc_inf("::main(). SDL2 Initialized successfully.");


	engine_cfg.threadCount = 4;
	engine_cfg.tickrate = 30;
	engine_cfg.fps = 144;
	engine_cfg.cappedFps = true;
	engine_cfg.gameSpeed = 1.0;
	//Init Hebi
	engine.init(engine_cfg);

	// Init InputManager
	InputManager::init();

	// Init DisplayManager
	DisplayManager::init();
	DisplayManager::load_window("molez", 1920, 1080, 2);
	DisplayManager::activate_window("molez");
	DisplayManager::clear(0x00000000);

	// Init AudioManager
	/*
	AudioManager::init();
	AudioManager::set_music_volume(64);
	AudioManager::load_music("INGAME1.MUS");
	AudioManager::play_music("INGAME1.MUS");
	*/
	// Init TextureManager
	TextureManager::init();

	// Test level generation
	level_cfg.seed = 12345;
	level_cfg.type = L_EARTH;
	level_cfg.width = DisplayManager::ACTIVE_WINDOW->width;
	level_cfg.height = DisplayManager::ACTIVE_WINDOW->height;
	level_cfg.n_scale = 0.0075f;
	level_cfg.n_water = 48;
	level_cfg.n_lava = 2;
	engine.setLevelConfig(level_cfg);
	engine.levelGenerate();

	//Luuppi pelkästään pelaamiseen
	//Menun voi toteuttaa ulkopuolella
	if(!gameLoop()){
		return_code = 1;
	}
	
	// Quit gracefully
	TextureManager::quit();
	//AudioManager::quit();
	DisplayManager::quit();
	InputManager::quit();
	SDL_Quit();
	mlibc_log_free();

	return return_code;
}