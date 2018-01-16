#include "menu.h"
#include <SDL2/SDL.h>
#include "3rdparty/mlibc_log.h"
#include "display_manager.h"
#include "input_manager.h"
#include "texture_manager.h"
#include "audio_manager.h"

#define MAX_VISIBLE_ITEMS 7

MenuItem::MenuItem(
	const std::string & text,
	MenuItem_t type,
	void * value,
	MenuItemValue_t datatype,
	std::function<void()> action
) :
	m_text(text),
	m_type(type),
	m_value(value),
	m_datatype(datatype),
	m_action(action)
{
	mlibc_inf("MenuItem::MenuItem(%s).", m_text.c_str());
}

const std::string & MenuItem::getText() const
{
	return m_text;
}

MenuItem_t MenuItem::getType() const
{
	return m_type;
}

void * const MenuItem::getValue() const
{
	return m_value;
}

MenuItemValue_t MenuItem::getDataType() const
{
	return m_datatype;
}

std::function<void()> MenuItem::getAction() const
{
	return m_action;
}

Menu::Menu(
	const std::string & title
) :
	m_parent(nullptr),
	m_child(nullptr),
	m_title(title),
	m_items(),
	m_active_item(0)
{
	mlibc_inf("Menu::Menu(%s).", m_title.c_str());
}

Menu::~Menu()
{
	// Delete child(s)
	delete m_child;

	// Delete items
	for (MenuItem * item : m_items)
	{
		delete item;
	}

	mlibc_inf("Menu::~Menu(%s).", m_title.c_str());
}

void Menu::set_parent(Menu * parent)
{
	m_parent = parent;
}

Menu * Menu::get_parent()
{
	return m_parent;
}

void Menu::set_child(Menu * child)
{
	m_child = child;
}

Menu * Menu::get_child()
{
	return m_child;
}

void Menu::add_item(MenuItem * item)
{
	m_items.push_back(item);
}

void Menu::render()
{
	// Render active submenu instead if it's not null
	if (m_child != nullptr)
	{
		m_child->render();
		return;
	}

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

	// Render menu items (only MAX_VISIBLE_ITEMS at a time)
	const size_t items_start = (m_active_item < MAX_VISIBLE_ITEMS) ? 0 : (m_active_item - MAX_VISIBLE_ITEMS + 1);
	const size_t items_end = (m_items.size() < MAX_VISIBLE_ITEMS) ? m_items.size() : MAX_VISIBLE_ITEMS;
	for (size_t i = items_start; i < items_end + items_start; i++)
	{
		MenuItem * item = m_items[i];

		// Recalculate y-pos for this item
		int mi_y_ = mi_y + ((i - items_start) * mi_h);

		// Only highlight current selected item
		if (i == m_active_item)
			DisplayManager::set_rect(mi_x, mi_y_, mi_w, mi_h, 128, 128, 128, 128, true);

		// Calculate font dimensions
		mif_x = mi_x;
		mif_y = mi_y_ + mif_h / 2;

		// Get value as string
		std::string val_str;
		switch (item->getType())
		{
			case MI_NUMERIC:
			{
				switch (item->getDataType())
				{
					case MIV_BOOL: val_str = std::to_string(*reinterpret_cast<bool *>(item->getValue())); break;
					case MIV_UINT8: val_str = std::to_string(*reinterpret_cast<uint8_t *>(item->getValue())); break;
					case MIV_UINT32: val_str = std::to_string(*reinterpret_cast<uint32_t *>(item->getValue())); break;
					case MIV_INT8: val_str = std::to_string(*reinterpret_cast<int8_t *>(item->getValue())); break;
					case MIV_INT32: val_str = std::to_string(*reinterpret_cast<int32_t *>(item->getValue())); break;
					case MIV_FLOAT: val_str = std::to_string(*reinterpret_cast<float *>(item->getValue())); break;
					case MIV_DOUBLE: val_str = std::to_string(*reinterpret_cast<double *>(item->getValue())); break;
				}
			} break;
		}

		// Render text + value
		std::string text = (val_str.size() == 0) ? item->getText() : item->getText() + ":" + val_str;
		DisplayManager::set_text(mif_x, mif_y, mif_w, mif_h, text, 255, 255, 255, font);
	}
}

void Menu::update()
{
	// Update active submenu instead if it's not null
	if (m_child != nullptr)
	{
		m_child->update();
		return;
	}

	// Navigate the menu UP/DOWN
	if (InputManager::KBOARD[SDLK_UP])
	{
		AudioManager::play_audio("MOVEUP.SFX");
		InputManager::KBOARD[SDLK_UP] = false;
		m_active_item = (m_active_item > 0) ? m_active_item - 1 : m_active_item;
	}
	else if (InputManager::KBOARD[SDLK_DOWN])
	{
		AudioManager::play_audio("MOVEDOWN.SFX");
		InputManager::KBOARD[SDLK_DOWN] = false;
		m_active_item = (m_active_item < m_items.size() - 1) ? m_active_item + 1 : m_active_item;
	}

	// Navigate the menu backwards
	if (InputManager::KBOARD[SDLK_ESCAPE])
	{
		AudioManager::play_audio("SELECT.SFX");
		InputManager::KBOARD[SDLK_DOWN] = false;

		if (m_parent != nullptr)
		{
			m_parent->set_child(nullptr);
			m_parent = nullptr;
		}
	}

	// Change value (left & right)
	if (InputManager::KBOARD[SDLK_LEFT] || InputManager::KBOARD[SDLK_RIGHT])
	{
		int key = InputManager::KBOARD[SDLK_LEFT] ? SDLK_LEFT : SDLK_RIGHT;
		AudioManager::play_audio((key == SDLK_LEFT) ? "MOVEDOWN.SFX" : "MOVEUP.SFX");
		InputManager::KBOARD[key] = false;

		// Get active item
		MenuItem * item = m_items[m_active_item];

		// Handle the value change based on item type
		switch (item->getType())
		{
			case MI_NUMERIC:
			{
				switch (item->getDataType())
				{
					case MIV_BOOL:
					{
						*reinterpret_cast<bool *>(item->getValue()) = (key == SDLK_LEFT) ? false : true;
					} break;
					case MIV_UINT8:
					{
						auto value = reinterpret_cast<uint8_t *>(item->getValue());
						*value = (key == SDLK_LEFT) ? *value - 1 : *value + 1;
					} break;
					case MIV_UINT32:
					{
						auto value = reinterpret_cast<uint32_t *>(item->getValue());
						*value = (key == SDLK_LEFT) ? *value - 1 : *value + 1;
					} break;
					case MIV_INT8:
					{
						auto value = reinterpret_cast<int8_t *>(item->getValue());
						*value = (key == SDLK_LEFT) ? *value - 1 : *value + 1;
					} break;
					case MIV_INT32:
					{
						auto value = reinterpret_cast<int32_t *>(item->getValue());
						*value = (key == SDLK_LEFT) ? *value - 1 : *value + 1;
					} break;
					case MIV_FLOAT:
					{
						auto value = reinterpret_cast<float *>(item->getValue());
						*value = (key == SDLK_LEFT) ? *value - 0.1f : *value + 0.1f;
					} break;
					case MIV_DOUBLE:
					{
						auto value = reinterpret_cast<double *>(item->getValue());
						*value = (key == SDLK_LEFT) ? *value - 0.1f : *value + 0.1f;
					} break;
				}
			} break;
		}
	}

	// Select menu item
	if (InputManager::KBOARD[SDLK_RETURN])
	{
		AudioManager::play_audio("SELECT.SFX");
		InputManager::KBOARD[SDLK_RETURN] = false;

		// Get selected item
		MenuItem * item = m_items[m_active_item];

		// Handle the selection based on item type
		switch (item->getType())
		{
			case MI_BUTTON:
			{
				if (item->getAction())
				{
					item->getAction()();
				}
			} break;
			case MI_SUBMENU:
			{
				m_child = static_cast<Menu *>(item->getValue());
				m_child->set_parent(this);
			} break;
		}
	}
}