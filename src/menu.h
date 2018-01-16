#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <functional>

enum MenuItem_t
{
	MI_EMPTY = 0,
	MI_SUBMENU = 1,
	MI_BUTTON = 2,
	MI_CHECKBOX = 3,
	MI_NUMERIC = 4
};

enum MenuItemValue_t
{
	MIV_EMPTY = 0,
	MIV_BOOL = 1,
	MIV_UINT8 = 2,
	MIV_UINT32 = 3,
	MIV_INT8 = 4,
	MIV_INT32 = 5,
	MIV_FLOAT = 6,
	MIV_DOUBLE = 7
};

class MenuItem
{
public:
	MenuItem(
		const std::string & text,
		MenuItem_t type,
		void * value = nullptr,
		MenuItemValue_t datatype = MIV_EMPTY,
		std::function<void()> action = std::function<void()>()
	);

	const std::string & getText() const;
	MenuItem_t getType() const;
	void * const getValue() const;
	MenuItemValue_t getDataType() const;
	std::function<void()> getAction() const;
private:
	std::string m_text;				// visible text
	MenuItem_t m_type;				// item type (button, submenu, numeric, etc...)
	void * m_value;					// item value
	MenuItemValue_t m_datatype;		// item value data type (uint32_t, uint8_t, etc...)
	std::function<void()> m_action;	// item action (function pointer)
};

class Menu
{
public:
	Menu(
		const std::string & title
	);
	~Menu();

	void set_parent(Menu * parent);
	Menu * get_parent();
	void set_child(Menu * child);
	Menu * get_child();
	void add_item(MenuItem * item);
	void render();
	void update();
private:
	Menu * m_parent;
	Menu * m_child;
	std::string m_title;
	std::vector<MenuItem *> m_items;
	size_t m_active_item;
};

#endif // MENU_H