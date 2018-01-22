#ifndef PHYSICS_H
#define PHYSICS_H

#include "math.h"

namespace Physics
{

	struct PosVelAcc
	{
		Math::vec2 pos;
		Math::vec2 vel;
		Math::vec2 acc;

		PosVelAcc(
			Math::vec2 pos = Math::vec2(),
			Math::vec2 vel = Math::vec2(),
			Math::vec2 acc = Math::vec2()
		) :
			pos(pos),
			vel(vel),
			acc(acc)
		{

		}

	};

}

#endif // PHYSICS_H