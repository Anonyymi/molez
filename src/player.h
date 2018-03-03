#include "entity.h"

class Player : public Entity
{
public:
	Player(
		Game * const game,
		Level * level,
		EntityProps props
	);
	virtual ~Player() override;
};