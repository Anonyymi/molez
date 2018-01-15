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
			engine.render();
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


	engine_cfg.threadCount = 3;
	engine_cfg.tickrate = 30;
	engine_cfg.fps = 60;
	engine_cfg.cappedFps = true;
	engine_cfg.gameSpeed = 1.0;
	engine.init(engine_cfg);

	if(!gameLoop()){
		return_code = 1;
	}
	
	// Quit gracefully

	mlibc_log_free();

	return return_code;
}