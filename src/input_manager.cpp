#include "input_manager.h"
#include "3rdparty/mlibc_log.h"

namespace InputManager
{

	// Mouse
	int MOUSE_X = 0, MOUSE_Y = 0;
	bool MOUSE_L = false;
	bool MOUSE_R = false;
	bool MOUSE_M = false;

	// Keyboard
	std::map<int, bool> KBOARD;

	// Init
	void init()
	{
		mlibc_inf("InputManager::init().");
	}

	// Quit (clears memory)
	void quit()
	{

	}

}