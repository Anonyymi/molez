#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

enum MenuItem_t
{
	MI_EMPTY = 0,
	MI_BUTTON = 1,
	MI_CHECKBOX = 2,
	MI_NUMERIC = 3
};

struct MenuItem
{
	std::string text;				// visible text
	MenuItem_t type;				// type
	void * value;					// value
	void(*action) (void);			// action (function pointer)

	MenuItem(
		const std::string & text,
		MenuItem_t type,
		void * value = nullptr,
		void(*action) (void) = nullptr
	) :
		text(text),
		type(type),
		value(value),
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

	void add_item(MenuItem * item);
	void render();
	void update();
private:
	std::string m_title;
	std::vector<MenuItem *> m_items;
	size_t m_selected_idx;
};

#endif // MENU_H