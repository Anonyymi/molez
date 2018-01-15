/*          HEBI
A game engine for lierolike games
Made by Lumipallo 2018-
*/

#ifndef HEBI_HPP
#define HEBI_HPP
#include "level.h"
#include "input_manager.h"
#include "display_manager.h"
#include "texture_manager.h"
#include "3rdparty/mlibc_log.h"
#include "player.h"
#include "Tick.hpp"
#include <vector>
#include <thread>
#include <unistd.h>
#include "queue.hpp"
#include <atomic>
#include <map>

#ifdef __linux__
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif





class Hebi;


class ThreadPool{
private:
    std::vector<std::thread> threads;
    std::vector<std::thread::id> threadIds;
    std::atomic_uint threadCount;
    std::atomic_uint idleCount;
    bool running;
    std::atomic_bool busy;
    std::map<std::thread::id , bool> idleThreads;
    HQueue queue;

public:

    ThreadPool();
    ~ThreadPool();

    //Start n amount of threads
    void storeThreadId(std::thread::id uid);
    bool spawn(int32_t n, ThreadPool &tPool, Hebi &engine);
    bool isRunning();
    //uint32_t idleThreads();
    uint32_t busyThreads();
    uint32_t totalThreads();
    bool isBusy();
    void threadIdle(std::thread::id uid);
    void threadBusy(std::thread::id uid);

    void threadWait();

    void enqueue(uint32_t *data);
    void syncThreads();
    void waitThreads();
    void resumeThreads();

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
    Player player;
    LevelConfig level_cfg;
    


public:
    
    Hebi();
    ~Hebi();


    
    bool init(EngineConfig cfg);
    void setLevelConfig(LevelConfig cfg);
    void levelGenerate();
    void printDebugInformation();
    void processInput(Tick *tick);
    void movePlayer(Tick *tick);
    bool nextTick(Tick *tick);

    void fluidSim(uint32_t y, uint32_t x);
    void physSim(uint32_t y, uint32_t x);

    void render();
    

    void threadWork();

    uint32_t tickrate();
    float gameSpeed();
    uint32_t maxfps();



};

typedef struct ThreadData{
    //Contains useful information for threads private uses
    //Probably not needed


}ThreadData;

typedef enum Work_t : uint32_t{
    WAIT = 0,
    RESUME = 1,
    FLUID = 100,
    PHYS = 101,
    FINISH = 9990,

} Work_t;


void worker(ThreadPool &tPool, Hebi &engine, HQueue &que);

#endif