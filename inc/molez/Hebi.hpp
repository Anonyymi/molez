/*          HEBI
A game engine for lierolike games
Made by xoxo 2018-
*/
#ifndef HEBI_HPP
#define HEBI_HPP
#include "level.h"
#include "input_manager.h"
#include "Tick.hpp"
#include <vector>
#include <thread>
#include <unistd.h>

#ifdef __linux__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif





class Hebi;

class ThreadPool{
private:
    std::vector<std::thread> threads;
    int numThreads;
    bool running;
public:

    ThreadPool();
    ~ThreadPool();

    //Start n amount of threads
    bool spawn(int32_t n, ThreadPool &tPool, Hebi &engine);
    bool isRunning();
    void quit();



};

typedef struct EngineConfig{
    uint32_t threadCount;
    uint32_t tickrate;
    uint32_t fps;
    bool cappedFps;
    float gameSpeed;

} EngineConfig;

class Hebi{
private:
    ThreadPool tPool;
    EngineConfig engineCfg;
    Level level;
    bool debug = false;

public:
    
    Hebi();
    ~Hebi();


    
    bool init(EngineConfig cfg);
    void setLevelConfig(LevelConfig cfg);
    void levelGenerate();
    void printDebugInformation();
    bool nextTick(Tick *tick);

    void threadWork();

    uint32_t tickrate();
    float gameSpeed();
    uint32_t maxfps();



};

typedef struct ThreadData{
    //Contains useful information for threads private uses
    //Probably not needed


}ThreadData;




void worker(ThreadPool &tPool, Hebi &engine);

#endif