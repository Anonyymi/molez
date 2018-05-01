#ifndef SPRITE_H
#define SPRITE_H

#include <vector>
#include <map>
#include <cstdint>

namespace TextureManager
{
struct Texture;
}

struct SpriteFrame
{
	int x;									// spritesheet frame x coord
	int y;									// spritesheet frame y coord
	int w;									// spritesheet frame width
	int h;									// spritesheet frame height
	// float t;								// frame length in seconds			<-- DO WE WANT THIS?
	std::vector<int32_t *> data;			// frame data from spritesheet

	SpriteFrame(
		int x = 0,
		int y = 0,
		int w = 16,
		int h = 16,
		// float t = 0.1f,														<-- DO WE WANT THIS?
		std::vector<int32_t *> data = std::vector<int32_t *>()
	) :
		x(x),
		y(y),
		w(w),
		h(h),
		//t(t),
		data(data)
	{

	}
};

struct SpriteAnim
{
	std::string identifier;					// animation identifier
	float dt;								// dt multiplier for anim
	std::vector<SpriteFrame> frames;		// animation frame list

	SpriteAnim(
		const std::string & identifier = "NULL",
		float dt = 1.0f,
		std::vector<SpriteFrame> frames = std::vector<SpriteFrame>()
	) :
		identifier(identifier),
		dt(dt),
		frames(frames)
	{

	}
};

class Sprite
{
public:
	Sprite(
		const std::string & filePath
	);
	~Sprite();

	void update(float t, float dt);
	void render(int x, int y, int alpha = -1);

	SpriteAnim * getCurrentAnim();
	SpriteFrame * getCurrentFrame();
private:
	std::string m_filePath;
	TextureManager::Texture * m_sheet;
	std::map<std::string, SpriteAnim> m_anims;
	bool m_animRepeat;
	std::string m_animIdent;
	size_t m_animFrame;
	bool m_animate;
};

#endif // SPRITE_H