#ifndef GAME_STATE_H
#define GAME_STATE_H

class Game;

class GameState
{
public:
	GameState(Game * const game);
	virtual ~GameState();

	virtual void update(float dt, float t) = 0;
	virtual void render() = 0;
private:
	Game * const m_game;
};

#endif // GAME_STATE_H