#pragma once
#include <Utils/includes.h>
#include <Utils/Utils.h>

#include "Menu-ITab.h"

extern glm::vec2 screenSize;

class MenuPropertyEditor
{
public:
	void run();
	void init();
	MenuPropertyEditor() :
		size(200),
		box(screenSize.x - size, screenSize.y, size, screenSize.y), ///
		flags(UI::LayoutVertical | UI::AlignRight | UI::AlignTop | UI::FixedSize | UI::Draw),
		active(true),
		currentMenuTab(0)
		{
			initTabs();
		}


private:
	void initTabs();
	void drawTabs();

	std::vector<unique_ptr<ITab>> menuTabs;
	glm::vec4 box;
	int currentMenuTab;
	int flags;
	float size;
	bool active;
};
