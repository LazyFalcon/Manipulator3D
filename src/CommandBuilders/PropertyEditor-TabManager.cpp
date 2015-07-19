#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "../IInterpolator.h"
#include "../Editor.h"
#include "PropertyEditor-TabManager.h"

namespace Editor
{
	extern PolylineEditor polylineEditor;
}

extern UI::IMGUI ui;
extern glm::vec2 screenSize;

void CommandEditorTab::run(TabManager &TM){
	getTypeWidget(TM);
	if(commandBuilderWidget){
		commandBuilderWidget->run();
		doneWidget();
	}
	else
		commandBuilderWidget.reset(new MoveCommandBuilderWidget());

}

/// tu przekazujemy do poly edytora interpolator z rozkazu
void CommandEditorTab::enter(TabManager &TM){
	commandBuilderWidget->enter();
}
void CommandEditorTab::getTypeWidget(TabManager &TM){
	/// commandTypeSelection.run([this](CommandEditorTabCommandWidget val){ commandBuilderWidget = buildWidget(val) });
	// createWidgetFromCommandAndSetPtr()
}

// unique_ptr<ICommandBuilderWidget> createWidgetFromCommandAndSetPtr(shared_ptr<ICommand> &ptr){
	// auto type = ptr->type;

// }


void CommandListTab::run(TabManager &TM){
	// for(auto &it : RC.commands){
		// ui.rect(150, 20).text(it->name)()
			// .onrClick(TM.get<CommandEditorTab>().setToEdit(it));
	// }

}

void PathListTab::run(TabManager &TM){
	for(auto &it : getInterpolators()){
		ui.rect(150, 20).text(it->name)();
		ui.rect(150, 1).color(0xFFFFFFFF)(UI::Label);
			// .onrClick(TM.get<CommandEditorTab>().setToEdit(it));
	}

}

void TabManager::run(){

	ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw)
		.style(UI::StyleID::Dark)
		.position(screenSize.x-size, screenSize.y-32)
		.size(size, -screenSize.y+32);
	drawTabs();
	tabs[currentTab]->run(*this);


	ui.endTable();
}

void TabManager::drawTabs(){
	ui.box(UI::LayoutHorizontal);

	for(u32 i=0; i<tabs.size(); i++){
		glm::vec4 rect;
		ui.image("Menu-Property-Tab")
			.switcher(currentTab, i)
			.getRect(rect)
			.color(i==currentTab ? 0xB77F3FFF : 0x808080FF)
			.onlClick([i, this]{
				setTab(i);
			})
			(UI::Button);
	}

	ui.endBox();
}


