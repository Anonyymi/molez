#ifndef GAME_H
#define GAME_H

#include <string>
#include <stack>

class GameState;

enum GameRunState_t
{
	GRS_STOPPED = 0,
	GRS_RUNNING = 1,
	GRS_ERROR = 2
};

struct GameConfig
{
	// window
	int win_width;
	int win_height;
	int win_scale;
	bool win_fullscreen;
	// graphics
	float gfx_framerate;
	// audio
	int sfx_music_vol;
	int sfx_audio_vol;
	// physics
	float phy_tickrate;
	float phy_timestep;
};

class Game
{
public:
	Game(
		GameConfig & cfg
	);
	~Game();

	GameRunState_t run();
	void stop();
	void update(float dt, float t);
	void render();
	void input();

	void setConfig(GameConfig cfg);
	GameConfig & getConfig();
	void setState(GameState * state);
	GameState * const getState();
private:
	GameConfig & m_cfg;
	GameRunState_t m_run_state;
	GameState * m_state;
};

#endif // GAME_H