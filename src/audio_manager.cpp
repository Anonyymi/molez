#include "audio_manager.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include "3rdparty/mlibc_log.h"

namespace AudioManager
{

	const std::string DATA_DIR = "./data/sfx/";
	Mix_Music * PLAYING_MUSIC = NULL;
	std::map<std::string, Mix_Music *> LOADED_MUSIC = std::map<std::string, Mix_Music *>();

	// Init
	void init()
	{
		int return_code = 0;

		// Init SDL2_mixer
		return_code = Mix_Init(MIX_INIT_MOD);
		if (return_code == 0)
		{
			Mix_OpenAudio(11025, MIX_DEFAULT_FORMAT, 2, 4096);
			mlibc_inf("AudioManager::init(). SDL2_mixer Initialized successfully.");
		}
		else
		{
			// TODO: Throw runtime exception
			mlibc_err("AudioManager::init(). Mix_Init Error: %s", SDL_GetError());
		}

		// Pre-load music
		load_music("MENU.MUS");
		load_music("INGAME1.MUS");
		load_music("INGAME2.MUS");

		mlibc_inf("AudioManager::init().");
	}

	// Quit (clears memory)
	void quit()
	{
		Mix_PauseMusic();
		Mix_CloseAudio();

		for (auto m : LOADED_MUSIC)
		{
			mlibc_inf("AudioManager::quit(). Destroy music.");

			Mix_FreeMusic(m.second);
		}

		Mix_Quit();
	}

	// Music (mp3, wav, ogg, flac, mod, xm, etc..)
	Mix_Music * const load_music(const std::string & file_path)
	{
		std::string full_file_path = DATA_DIR + file_path;

		if (LOADED_MUSIC.count(file_path) == 0)
		{
			LOADED_MUSIC[file_path] = Mix_LoadMUS(full_file_path.c_str());

			if (LOADED_MUSIC[file_path] == NULL)
			{
				LOADED_MUSIC.erase(file_path);
				mlibc_err("AudioManager::load_music(%s). Error loading file into memory!", file_path.c_str());
				return nullptr;
			}

			mlibc_inf("AudioManager::load_music(%s). Loaded file into memory.", file_path.c_str());
		}

		return LOADED_MUSIC[file_path];
	}

	void play_music(const std::string & file_path)
	{
		if (LOADED_MUSIC.count(file_path) > 0)
		{
			PLAYING_MUSIC = LOADED_MUSIC[file_path];
			if (Mix_PlayMusic(PLAYING_MUSIC, -1) != 0)
			{
				mlibc_err("AudioManager::play_music(%s). Error playing music!", file_path.c_str());
				return;
			}

			mlibc_inf("AudioManager::play_music(%s). Started playing music.", file_path.c_str());
		}
	}

	void set_music_volume(int volume)
	{
		Mix_VolumeMusic(volume);
	}

}