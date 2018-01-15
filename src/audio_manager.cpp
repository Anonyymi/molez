#include "audio_manager.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "3rdparty/mlibc_log.h"


namespace AudioManager
{

	const std::string DATA_DIR = "./data/sfx/";
	Mix_Music * PLAYING_MUSIC = NULL;
	Mix_Chunk * PLAYING_AUDIO = NULL;
	std::map<std::string, Mix_Music *> LOADED_MUSIC = std::map<std::string, Mix_Music *>();
	std::map<std::string, Mix_Chunk *> LOADED_AUDIO = std::map<std::string, Mix_Chunk *>();

	// Init
	void init()
	{
		int return_code = 0;

		// Init SDL2_mixer
		return_code = Mix_Init(MIX_INIT_MOD);
		if (return_code == 0)
		{
			Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
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

		// Pre-laod audio
		load_audio("MOVEUP.SFX");
		load_audio("MOVEDOWN.SFX");
		load_audio("SELECT.SFX");

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

			mlibc_dbg("AudioManager::load_music(%s). Loaded file into memory.", file_path.c_str());
		}

		return LOADED_MUSIC[file_path];
	}

	void play_music(const std::string & file_path)
	{
		if (LOADED_MUSIC.count(file_path) > 0)
		{
			PLAYING_MUSIC = LOADED_MUSIC[file_path];

			Mix_FadeOutMusic(500);
			if (Mix_FadeInMusic(PLAYING_MUSIC, -1, 500) != 0)
			{
				mlibc_err("AudioManager::play_music(%s). Error playing music!", file_path.c_str());
				return;
			}

			mlibc_dbg("AudioManager::play_music(%s). Started playing music.", file_path.c_str());
		}
	}

	void set_music_volume(int volume)
	{
		Mix_VolumeMusic(volume);
	}

	// Audio (mp3, wav, ogg, flac, etc..)
	Mix_Chunk * const load_audio(const std::string & file_path)
	{
		std::string full_file_path = DATA_DIR + file_path;

		if (LOADED_AUDIO.count(file_path) == 0)
		{
			LOADED_AUDIO[file_path] = Mix_LoadWAV_RW(SDL_RWFromFile(full_file_path.c_str(), "rb"), 1);

			if (LOADED_AUDIO[file_path] == NULL)
			{
				LOADED_AUDIO.erase(file_path);
				mlibc_err("AudioManager::load_audio(%s). Error loading file into memory!", file_path.c_str());
				return nullptr;
			}

			mlibc_dbg("AudioManager::load_audio(%s). Loaded file into memory.", file_path.c_str());
		}

		return LOADED_AUDIO[file_path];
	}

	void play_audio(const std::string & file_path)
	{
		if (LOADED_AUDIO.count(file_path) > 0)
		{
			PLAYING_AUDIO = LOADED_AUDIO[file_path];

			if (Mix_PlayChannel(0, PLAYING_AUDIO, 0) != 0)
			{
				mlibc_err("AudioManager::play_audio(%s). Error playing audio!", file_path.c_str());
				return;
			}

			mlibc_dbg("AudioManager::play_audio(%s). Started playing audio.", file_path.c_str());
		}
	}

	void set_audio_volume(int volume)
	{
		Mix_Volume(0, volume);
	}

}
