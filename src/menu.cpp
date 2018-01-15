#include "menu.h"
#include <SDL2/SDL.h>
#include "3rdparty/mlibc_log.h"
#include "display_manager.h"
#include "input_manager.h"
#include "texture_manager.h"
#include "audio_manager.h"

Menu::Menu(
	const std::string & title
) :
	m_title(title),
	m_items(),
	m_selected_idx(0)
{

}

Menu::~Menu()
{

}

void Menu::add_item(MenuItem * item)
{
	m_items.push_back(item);
}

void Menu::render()
{
	// Get display props
	int d_w = DisplayManager::ACTIVE_WINDOW->width;
	int d_h = DisplayManager::ACTIVE_WINDOW->height;

	// Calculate menu dimensions
	int m_sw = d_w / 4;
	int m_sh = d_h / 4;
	int m_w = d_w - m_sw;
	int m_h = d_h - m_sh;
	int m_x = m_sw / 2;
	int m_y = m_sh / 2;

	// Render menu background
	DisplayManager::set_rect(m_x, m_y, m_w, m_h, 0, 0, 0, 160, true);

	// Calculate menu item dimensions
	int mi_w = m_w - (m_w / 4);
	int mi_h = m_h / 10;
	int mi_x = m_x + (m_w / 8);
	int mi_y = m_y + mi_h * 2;

	// Get font and render title
	TextureManager::Font * font = TextureManager::load_font("MOLEZ.JSON");
	int mif_w = mi_h - (mi_h / 2);
	int mif_h = mif_w;
	int mif_x = mi_x + mi_x / 3 + mi_x / 2; // TODO; This is fucked up. Add support for centering text!
	int mif_y = m_y + mif_h;
	DisplayManager::set_text(mif_x, mif_y, mif_w, mif_h, m_title, 255, 255, 255, font);

	// Render menu items (only 8 at a time)
	for (size_t i = 0; i < m_items.size(); i++)
	{
		MenuItem * item = m_items[i];

		// Recalculate y-pos for this item
		int mi_y_ = mi_y + (i * mi_h);

		// Only highlight current selected item
		if (i == m_selected_idx)
			DisplayManager::set_rect(mi_x, mi_y_, mi_w, mi_h, 128, 128, 128, 128, true);

		// Calculate font dimensions
		mif_x = mi_x;
		mif_y = mi_y_ + mif_h / 2;

		// Render text
		DisplayManager::set_text(mif_x, mif_y, mif_w, mif_h, item->text, 255, 255, 255, font);
	}
}

void Menu::update()
{
	// Navigate the menu UP/DOWN
	if (InputManager::KBOARD[SDLK_UP])
	{
		m_selected_idx = (m_selected_idx > 0) ? m_selected_idx - 1 : m_selected_idx;
		AudioManager::play_audio("MOVEUP.SFX");
		InputManager::KBOARD[SDLK_UP] = false;

		mlibc_dbg("idx: %zu", m_selected_idx);
	}
	else if (InputManager::KBOARD[SDLK_DOWN])
	{
		m_selected_idx = (m_selected_idx < m_items.size() - 1) ? m_selected_idx + 1 : m_selected_idx;
		AudioManager::play_audio("MOVEDOWN.SFX");
		InputManager::KBOARD[SDLK_DOWN] = false;

		mlibc_dbg("idx: %zu", m_selected_idx);
	}

	// Select menu item
	if (InputManager::KBOARD[SDLK_RETURN])
	{
		AudioManager::play_audio("SELECT.SFX");
		InputManager::KBOARD[SDLK_RETURN] = false;
	}
}