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
};

struct PhysicsState
{
	float t;			// time in seconds
	float dt;			// timestep in seconds
	float t_curr;		// current hires_time_in_seconds()
	float t_acc;		// accumulator for time that has to be simulated
	float s_curr;		// current physics state
	float s_prev;		// previous physics state
	float s_lerp;		// linearly interpolated state
	float alpha;		// value for state interpolation, [0..1]
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
	void update(float state, float t, float dt);
	void render(float state);
	void input();

	void setCfg(GameConfig cfg);
	GameConfig & getCfg();
	void setState(GameState * state);
	GameState * const getState();
	PhysicsState getPhysState() const;
	float getTimeInSec() const;
private:
	GameConfig & m_cfg;
	GameRunState_t m_run_state;
	GameState * m_state;
	PhysicsState m_phys;
};

#endif // GAME_H