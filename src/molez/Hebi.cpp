#include "Hebi.hpp"





Hebi::
Hebi(){


}

Hebi::
~Hebi(){
    tPool.quit();
    TextureManager::quit();
	//AudioManager::quit();
	DisplayManager::quit();
	InputManager::quit();
	SDL_Quit();

}



bool Hebi::
init(EngineConfig cfg){
    engineCfg = cfg;
    
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
    

	DisplayManager::init();
	DisplayManager::load_window("molez", 1920, 1080, 2);
	DisplayManager::activate_window("molez");
	DisplayManager::clear(0x00000000);

    TextureManager::init();
    InputManager::init();


    LevelConfig level_cfg;
    level_cfg.seed = 12345;
	level_cfg.type = L_EARTH;
	level_cfg.width = DisplayManager::ACTIVE_WINDOW->width;
	level_cfg.height = DisplayManager::ACTIVE_WINDOW->height;
	level_cfg.n_scale = 0.0075f;
	level_cfg.n_water = 48;
	level_cfg.n_lava = 2;
	setLevelConfig(level_cfg);
	levelGenerate();




    tPool.spawn(engineCfg.threadCount, tPool, *this);
}

void Hebi::
setLevelConfig(LevelConfig cfg){
    level.setConfig(cfg);
}

void Hebi::
levelGenerate(){
    level.generate();
}

void Hebi::
printDebugInformation(){
    
}

void Hebi::
threadWork(){
    //
}

uint32_t Hebi::
tickrate(){
    return engineCfg.tickrate;
}

uint32_t Hebi::
maxfps(){
    return engineCfg.fps;
}

float Hebi::
gameSpeed(){
    return engineCfg.gameSpeed;
}



bool Hebi::
nextTick(Tick *tick){
    
    level.update();

	level.render();

    if (InputManager::MOUSE_L)
    {
        level.alter(M_WATER, 8, InputManager::MOUSE_X, InputManager::MOUSE_Y);
    }
    else if (InputManager::MOUSE_M)
    {
        level.alter(M_LAVA, 8, InputManager::MOUSE_X, InputManager::MOUSE_Y);
    }
    else if (InputManager::MOUSE_R)
    {
        level.alter(M_VOID, 12, InputManager::MOUSE_X, InputManager::MOUSE_Y);
    }

    // Level reset
    if (InputManager::KBOARD[SDLK_r])
    {
        level.regenerate(rand());
    }

    if (InputManager::KBOARD[SDLK_d])
    {
        debug = true;
    }
    //Change tickrate
    if (InputManager::KBOARD[SDLK_j]){
        engineCfg.tickrate++;
    }

    if (InputManager::KBOARD[SDLK_k]){
        if(engineCfg.tickrate>1)
            engineCfg.tickrate--;
    }

    //Change fps
    if (InputManager::KBOARD[SDLK_u]){
            engineCfg.fps++;
    }
    if (InputManager::KBOARD[SDLK_i]){
        if(engineCfg.fps>1){
            engineCfg.fps--;
        }
    }

    // Program exit
    if (InputManager::KBOARD[SDLK_ESCAPE])
    {
        return false;
    }

    if(debug){
        //Add more debugging properties here if necessary
        printDebugInformation();

    }
    debug = false;

    tick->count++;
    return true;
}


void worker(ThreadPool &tPool, Hebi &engine){
    while(tPool.isRunning()){
        
        usleep(100);
        engine.threadWork();
        //Magic happens here. Thread shouldn't be able to access everything but its ok for now
        

    }
}


ThreadPool::
ThreadPool(){
    running = true;
}
ThreadPool::
~ThreadPool(){

}

bool ThreadPool::
isRunning(){
    return running;
}


bool ThreadPool::
spawn(int32_t n, ThreadPool &tPool, Hebi &engine){
    for(int i=0;i<n;i++)
        threads.push_back(std::thread(worker, std::ref(tPool), std::ref(engine)));
    return true;
}




void ThreadPool::
quit(){
    running = false;
    for(int i=0;i<threads.size();i++){
        threads.at(i).join();
    }
}