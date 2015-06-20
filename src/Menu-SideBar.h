#pragma once
#include <Utils/includes.h>
#include <Utils/Utils.h>



extern glm::vec2 screenSize;

class ITab
{
public:
	virtual void run() = 0;

	virtual ~ITab(){}
	ITab(const std::string &iconName) :
		icon(iconName)
		{}

	const std::string icon;
};

class ManuSideBar
{
public:
	void run();
	ManuSideBar() :
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

