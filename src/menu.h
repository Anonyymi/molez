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

struct MenuItem
{
	std::string text;				// visible text
	MenuItem_t type;				// type
	void * value;					// value
	MenuItemValue_t datatype;		// value data type
	std::function<void()> action;	// action (function pointer)

	MenuItem(
		const std::string & text,
		MenuItem_t type,
		void * value = nullptr,
		MenuItemValue_t datatype = MIV_EMPTY,
		std::function<void()> action = std::function<void()>()
	) :
		text(text),
		type(type),
		value(value),
		datatype(datatype),
		action(action)
	{

	}
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