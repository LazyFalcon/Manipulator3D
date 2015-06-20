#include <Utils/includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "Robot-Commands.h"
#include "Menu-SideBar.h"
#include "Menu-Tabs.cpp"
#include "Robot-CommandBuilders.h"

extern UI::IMGUI ui;
extern glm::vec2 screenSize;
extern unique_ptr<ICommandBuilder> CommandBuilders[];

void ManuSideBar::initTabs(){
	menuTabs.push_back(make_unique<CommandBuilderTab>("empty"));
	menuTabs.push_back(make_unique<CommanListTab>("empty"));
	menuTabs.push_back(make_unique<PolylineBuilderTab>("empty"));
	menuTabs.push_back(make_unique<PolylineListTab>("empty"));
	menuTabs.push_back(make_unique<PlotPropertyListTab>("empty"));

}

void ManuSideBar::run(){
	// if(minimized) return;
	ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw)
		.position(screenSize.x-size, screenSize.y-50)
		.size(size, -screenSize.y+50);

	drawTabs();
	menuTabs[currentMenuTab]->run();

	ui.endTable();
}

void ManuSideBar::drawTabs(){
	ui.box(UI::LayoutHorizontal);

	for(int i=0; i<menuTabs.size(); i++){
		glm::vec4 rect;
		ui.image("Menu-Property-Tab")
			.switcher(currentMenuTab, i)
			.getRect(rect)
			.color(i==currentMenuTab ? 0xA0A0A0FF : 0x808080FF)
			(UI::Button);

		// ui.image(rect.x, rect.y, menuTabs[i]->icon)();
	}

	ui.endBox();
}


void CommandBuilderTab::run(){
	/// okienko z wyborem komendy
	moveBuilder.widget();


}

