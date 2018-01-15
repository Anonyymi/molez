#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <string>
#include <map>

typedef struct _Mix_Music Mix_Music;
typedef struct Mix_Chunk Mix_Chunk;

namespace AudioManager
{

	extern const std::string DATA_DIR;
	extern Mix_Music * PLAYING_MUSIC;
	extern Mix_Chunk * PLAYING_AUDIO;
	extern std::map<std::string, Mix_Music *> LOADED_MUSIC;
	extern std::map<std::string, Mix_Chunk *> LOADED_AUDIO;

	// Init
	void init();

	// Quit (clears memory)
	void quit();

	// Music (mp3, wav, ogg, flac, mod, xm, etc..)
	Mix_Music * const load_music(const std::string & file_path);
	void play_music(const std::string & file_path);
	void set_music_volume(int volume);

	// Audio (mp3, wav, ogg, flac, etc..)
	Mix_Chunk * const load_audio(const std::string & file_path);
	void play_audio(const std::string & file_path);
	void set_audio_volume(int volume);

}


#endif // AUDIO_MANAGER_H