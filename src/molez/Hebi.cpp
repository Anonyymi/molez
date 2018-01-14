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


    
    level_cfg.seed = 12345;
	level_cfg.type = L_EARTH;
	level_cfg.width = DisplayManager::ACTIVE_WINDOW->width;
	level_cfg.height = DisplayManager::ACTIVE_WINDOW->height;
	level_cfg.n_scale = 0.0075f;
	level_cfg.n_water = 48;
	level_cfg.n_lava = 2;
	setLevelConfig(level_cfg);
	levelGenerate();

    player.setPos(200,200);





    tPool.spawn(engineCfg.threadCount, tPool, *this, workQueue);
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



void Hebi::
processInput(Tick *tick){
    if (InputManager::MOUSE_L){
        level.alter(M_WATER, 8, InputManager::MOUSE_X, InputManager::MOUSE_Y);
    }
    else if (InputManager::MOUSE_M){
        level.alter(M_LAVA, 8, InputManager::MOUSE_X, InputManager::MOUSE_Y);
    }
    else if (InputManager::MOUSE_R){
        level.alter(M_VOID, 12, InputManager::MOUSE_X, InputManager::MOUSE_Y);
    }

    // Level reset
    if (InputManager::KBOARD[SDLK_r]){
        while(!syncThreads());
        level.regenerate(rand());
    }

    if (InputManager::KBOARD[SDLK_d]){
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
        if(engineCfg.fps>1)
            engineCfg.fps--;
    }


    tick->move.y = 0;
    tick->move.x = 0;

    //Test movement
    if (InputManager::KBOARD[SDLK_w]){
        tick->move.y -= 3;
    }
    if (InputManager::KBOARD[SDLK_a]){
        tick->move.x -= 3;
    }
    if (InputManager::KBOARD[SDLK_s]){
        tick->move.y += 3;
    }
    if (InputManager::KBOARD[SDLK_d]){
        tick->move.x += 3;
    }

    // Program exit

}

void Hebi::
movePlayer(Tick *tick){
    player.move(tick->move.x, tick->move.y);
}

void Hebi::
fluidSim(uint32_t y, uint32_t x){
    level.update(y,x);
}

void Hebi::
render(){
    while(!syncThreads());


    for(int i=1070;i>0; i--){            
	    level.render(i);
    }


    DisplayManager::render();
}

bool Hebi::
syncThreads(){
    if(tPool.isBusy()){
        std::this_thread::sleep_for(std::chrono::microseconds(50));
        return false;
    }
    return true;
}



bool Hebi::
nextTick(Tick *tick){

    while(!syncThreads());

    uint32_t *wurk = (uint32_t*)malloc(sizeof(uint32_t)*5);
    wurk[0] = FLUID;
    wurk[1] = 1070; //to height
    wurk[2] = 480; //to width
    wurk[3] = 1; //from height
    wurk[4] = 1; //from width;
    workQueue.push(wurk);

    wurk = (uint32_t*)malloc(sizeof(uint32_t)*5);
    wurk[0] = FLUID;
    wurk[1] = 1070; //to height
    wurk[2] = 960; //to width
    wurk[3] = 1; //from height
    wurk[4] = 480; //from width;
    workQueue.push(wurk);

    wurk = (uint32_t*)malloc(sizeof(uint32_t)*5);
    wurk[0] = FLUID;
    wurk[1] = 1070; //to height
    wurk[2] = 1440; //to width
    wurk[3] = 1; //from height
    wurk[4] = 960; //from width;
    workQueue.push(wurk);

    wurk = (uint32_t*)malloc(sizeof(uint32_t)*5);
    wurk[0] = FLUID;
    wurk[1] = 1070; //to height
    wurk[2] = 1910; //to width
    wurk[3] = 1; //from height
    wurk[4] = 1440; //from width;
    workQueue.push(wurk);
    
    
    processInput(tick);

    movePlayer(tick);

    
    //
    //level.alter(M_WATER, 5, player.getX(), player.getY());

    if(debug){
        //Add more debugging properties here if necessary
        printDebugInformation();

    }

   
    tick->count++;

    if (InputManager::KBOARD[SDLK_ESCAPE])
        return false;
    else  
        return true;
}




uint32_t ThreadPool::
busyThreads(){
    return threadCount - idleCount;
}

uint32_t ThreadPool::
totalThreads(){
    return threadCount;
}

bool ThreadPool::
isBusy(){
    for(int i=0; i<threads.size(); i++){
        if(idleThreads[threadIds.at(i)] == false)
            return true;
    }
    return false;

}

void ThreadPool::
threadIdle(std::thread::id uid){
    idleThreads[uid] = true;
    idleCount++;
}
void ThreadPool::
threadBusy(std::thread::id uid){
    idleThreads[uid] = false;
    idleCount--;
}

void worker(ThreadPool &tPool, Hebi &engine, HQueue &que){
    tPool.storeThreadId(std::this_thread::get_id());
    tPool.threadIdle(std::this_thread::get_id());
    while(tPool.isRunning()){
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        uint32_t *wurk = que.pull();
        if(wurk != NULL){
            //tPool.threadBusy(std::this_thread::get_id());
            switch(wurk[0]){
                case FLUID:
                    for(int i=wurk[1];i>wurk[3];i--){
                        for(int j = wurk[2]; j>wurk[4];j--){
                            engine.fluidSim(i,j);
                        }
                    }
            }
            tPool.threadIdle(std::this_thread::get_id());
            free(wurk);
        }
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

void ThreadPool::
storeThreadId(std::thread::id uid){
    threadIds.push_back(uid);
}

bool ThreadPool::
spawn(int32_t n, ThreadPool &tPool, Hebi &engine, HQueue &que){
    for(int i=0;i<n;i++){
        threads.push_back(std::thread(worker, std::ref(tPool), std::ref(engine),std::ref(que)));
        threadCount++;
    }
    return true;
}




void ThreadPool::
quit(){
    running = false;
    for(int i=0;i<threads.size();i++){
        threads.at(i).join();
    }
}