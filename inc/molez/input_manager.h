#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <map>

namespace InputManager
{

	// Mouse
	extern int MOUSE_X, MOUSE_Y;
	extern bool MOUSE_L;
	extern bool MOUSE_R;
	extern bool MOUSE_M;

	// Keyboard
	extern std::map<int, bool> KBOARD;

	// Init
	void init();

	bool SDLInput();

	// Quit (clears memory)
	void quit();

}


#endif // INPUT_MANAGER_H