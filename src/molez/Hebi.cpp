#include "Hebi.hpp"





Hebi::
Hebi(){


}

Hebi::
~Hebi(){
    tPool.quit();

}



bool Hebi::
init(EngineConfig cfg){
    engineCfg = cfg;
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

	// Render level
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
    if (InputManager::KBOARD[SDLK_j]){
        engineCfg.tickrate++;
    }

    if (InputManager::KBOARD[SDLK_k]){
        if(engineCfg.tickrate>1)
            engineCfg.tickrate--;
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