#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

namespace InputManager
{

	// Mouse
	extern int MOUSE_X, MOUSE_Y;
	extern bool MOUSE_L;
	extern bool MOUSE_R;
	extern bool MOUSE_M;

	// Keyboard
	extern bool KBOARD[255];

	// Init
	void init();

	// Quit (clears memory)
	void quit();

}


#endif // INPUT_MANAGER_H