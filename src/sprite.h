#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
#include <map>
#include <cstdint>

struct Texture;

struct SpriteFrame
{
	int x;									// spritesheet frame x coord
	int y;									// spritesheet frame y coord
	int w;									// spritesheet frame width
	int h;									// spritesheet frame height
	float t;								// frame length in seconds
	int32_t * data;							// frame data from spritesheet

	SpriteFrame(
		int x = 0,
		int y = 0,
		int w = 16,
		int h = 16,
		float t = 0.1f,
		int32_t * data = nullptr
	) :
		x(x),
		y(y),
		w(w),
		h(h),
		t(t),
		data(data)
	{

	}
};

struct SpriteAnim
{
	std::string identifier;					// animation identifier
	float dt;								// dt multiplier for anim
	std::vector<SpriteFrame> m_frames;
};

class Sprite
{
public:
	Sprite(
		const std::string & filePath
	);
private:
	std::string m_filePath;
	Texture * const m_sheet;
	std::map<std::string, SpriteAnim *> m_anims;
	bool m_animRepeat;
};

#endif // SPRITE_H