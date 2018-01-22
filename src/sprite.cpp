#include "sprite.h"
#include "3rdparty/json.hpp"
#include "texture_manager.h"

using json = nlohmann::json;

Sprite::Sprite(
	const std::string & filePath
) :
	m_filePath(filePath),
	m_sheet(nullptr),
	m_frames(),
	m_animRepeat(true)
{

}