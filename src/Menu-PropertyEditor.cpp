#include <Utils/includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "RobotCommands.h"
#include "Menu-PropertyEditor.h"
#include "Menu-CommandBuilder.h"
#include "Menu-PathEditor.h"

extern UI::IMGUI ui;
extern glm::vec2 screenSize;
extern unique_ptr<ICommandBuilder> CommandBuilders[];

void MenuPropertyEditor::initTabs(){
	menuTabs.push_back(make_unique<CommandBuilderTab>("empty"));
	menuTabs.push_back(make_unique<CommanListTab>("empty"));
	menuTabs.push_back(make_unique<PathListTab>("empty"));
	menuTabs.push_back(make_unique<PathEditorTab>("empty"));
	menuTabs.push_back(make_unique<PlotTab>("empty"));


	CommandBuilders[0] = make_unique<MoveBuilder>();
}

void MenuPropertyEditor::run(){
	// if(minimized) return;
	ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw)
		.position(screenSize.x-size, screenSize.y-50)
		.size(size, -screenSize.y+50);

	drawTabs();
	menuTabs[currentMenuTab]->run();

	ui.endTable();
}

void MenuPropertyEditor::drawTabs(){
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
	CommandBuilders[0]->widget();

	ui.endBox();
}


