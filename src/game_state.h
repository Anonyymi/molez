#ifndef GAME_STATE_H
#define GAME_STATE_H

class Game;

class GameState
{
public:
	GameState(Game * const game);
	virtual ~GameState();

	virtual void update(float state, float t, float dt) = 0;
	virtual void render(float state) = 0;
protected:
	Game * m_game;
};

#endif // GAME_STATE_H