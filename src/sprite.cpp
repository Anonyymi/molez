#include "sprite.h"
#include <fstream>
#include <exception>
#include "3rdparty/json.hpp"
#include "3rdparty/mlibc_log.h"
#include "texture_manager.h"
#include "display_manager.h"

using json = nlohmann::json;

Sprite::Sprite(
	const std::string & filePath
) :
	m_filePath(filePath),
	m_sheet(nullptr),
	m_anims(),
	m_animRepeat(true),
	m_animIdent("GO_RIGHT"),
	m_animFrame(NULL),
	m_animate(true)
{
	// Read sprite cfg from JSON file
	std::ifstream spr_file("./data/spr/" + m_filePath, std::ifstream::binary);

	if (spr_file.is_open() == false)
	{
		throw std::runtime_error("Sprite::Sprite(). Error loading sprite cfg file into memory!");
	}

	json spr_json;
	spr_file >> spr_json;
	spr_file.close();

	// Load spritesheet texture
	m_sheet = TextureManager::load_texture(spr_json["sheet"].get<std::string>());

	// Parse sprite animations
	json & json_anims = spr_json["anims"];
	size_t n_anims = json_anims.size();
	for (size_t i = 0; i < n_anims; i++)
	{
		json & json_anim = json_anims[i];

		// Construct anim object
		SpriteAnim anim(
			json_anim["identifier"].get<std::string>(),
			json_anim["dt"].get<float>(),
			std::vector<SpriteFrame>()
		);

		// Parse individual frames for this anim
		json & json_frames = json_anim["frames"];
		size_t n_frames = json_frames.size();
		for (size_t j = 0; j < n_frames; j++)
		{
			json & json_frame = json_frames[j];

			// Construct frame object
			SpriteFrame frame(
				json_frame["x"].get<int>(),
				json_frame["y"].get<int>(),
				json_frame["w"].get<int>(),
				json_frame["h"].get<int>()
			);

			// Sample frame pixel data from spritesheet texture data
			frame.data = TextureManager::sample_texture(m_sheet->file_path, frame.x * frame.w, frame.y * frame.h, frame.w, frame.h);

			// Push to anim frame vector
			anim.frames.push_back(frame);
		}

		// Push to anim map
		m_anims[anim.identifier] = anim;

		mlibc_dbg("Sprite::Sprite(). Parsed sprite animation. identifier: %s, frames: %zu", anim.identifier.c_str(), anim.frames.size());
	}

	m_animRepeat = spr_json["anim_repeat"].get<bool>();
}

Sprite::~Sprite()
{

}

void Sprite::update(float t, float dt)
{
	// Do not continue if current animation does not exist
	if (m_anims.count(m_animIdent) == 0)
	{
		return;
	}

	// Calculate current anim frame
	if (m_animate)
	{
		// Get anim data
		SpriteAnim * anim = &m_anims[m_animIdent];

		// Calculate current frame index, scale by anim dt
		m_animFrame = static_cast<size_t>(t / anim->dt) % anim->frames.size();
	}
}

void Sprite::render(int x, int y, int alpha)
{
	// Do not continue if current animation does not exist
	if (m_anims.count(m_animIdent) == 0)
	{
		return;
	}

	// Get current animation + frame & render it
	SpriteAnim * anim = &m_anims[m_animIdent];
	SpriteFrame * frame = &anim->frames[m_animFrame];
	for (size_t i = 0; i < frame->data.size(); i++)
	{
		// Get pixel at x,y
		size_t p_x = i % frame->w;
		size_t p_y = static_cast<size_t>(std::floor(i / frame->w));
		int32_t argb = *frame->data[p_y + p_x * frame->w];

		// Decode argb to rgba components
		auto a = (argb & 0xFF000000) >> 24;
		auto r = (argb & 0x00FF0000) >> 16;
		auto g = (argb & 0x0000FF00) >> 8;
		auto b = (argb & 0x000000FF);

		// Either force alpha or use sprite alpha
		if (alpha != -1 && a != 0)
		{
			a = static_cast<uint8_t>(alpha);
		}

		// Plot it!
		DisplayManager::set_pixel(x + p_x, y + p_y, r, g, b, a, false);
	}
}