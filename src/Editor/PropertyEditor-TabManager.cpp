#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "../IInterpolator.h"
#include "Editor.h"
#include "PropertyEditor-TabManager.h"

extern UI::IMGUI ui;
extern glm::vec2 screenSize;
namespace Editor NAM_START

extern PolylineEditor polylineEditor;
unique_ptr<ICommandBuilderWidget> createWidgetFromCommandAndSetPtr(shared_ptr<ICommand> &ptr){
	unique_ptr<ICommandBuilderWidget> out;
	if(ptr->type == MOVE){
		// out = make_unique<MoveCommandBuilderWidget>(ptr);
		out.reset(new MoveCommandBuilderWidget(ptr));
	}

	return out;
}

/// ----- COMMAND EDITOR TAB ---------------------------------------------
void CommandEditorTab::run(TabManager &TM){
	getTypeWidget(TM);
	if(commandBuilderWidget){
		commandBuilderWidget->run();
		doneWidget();
	}
	// else
		// commandBuilderWidget.reset(new MoveCommandBuilderWidget());

}
void CommandEditorTab::reset(shared_ptr<ICommand> &ptr){
	commandBuilderWidget = createWidgetFromCommandAndSetPtr(ptr);
}
void CommandEditorTab::onEnter(TabManager &TM){
	// commandBuilderWidget->onEnter();
}
void CommandEditorTab::getTypeWidget(TabManager &TM){
	/// commandTypeSelection.run([this](CommandEditorTabCommandWidget val){ commandBuilderWidget = buildWidget(val) });
	// createWidgetFromCommandAndSetPtr()
}


/// ----- COMMAND LIST TAB ---------------------------------------------
void CommandListTab::run(TabManager &TM){
	// for(auto &it : RC.commands){
		// ui.rect(150, 20).text(it->name)()
			// .onrClick(TM.get<CommandEditorTab>().setToEdit(it));
	// }

}

/// ----- PATH EDITOR TAB ---------------------------------------------
void PathListTab::run(TabManager &TM){
	for(auto &it : getInterpolators()){
		ui.rect(150, 20).text(it->name)();
		ui.rect(150, 1).color(0xFFFFFFFF)(UI::Label);
			// .onrClick(TM.get<CommandEditorTab>().setToEdit(it));
	}

}



/// ----- TAB MANAGER ---------------------------------------------
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

	// for(u32 i=0; i<tabs.size(); i++){
	for(u32 i=0; i<4; i++){
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

template<>
CommandEditorTab& TabManager::get<CommandEditorTab>(){
	return static_cast<CommandEditorTab&>(*tabs[0]);
}
template<>
CommandListTab& TabManager::get<CommandListTab>(){
	return static_cast<CommandListTab&>(*tabs[0]);
}
template<>
PathEditorTab& TabManager::get<PathEditorTab>(){
	return static_cast<PathEditorTab&>(*tabs[0]);
}
template<>
PathListTab& TabManager::get<PathListTab>(){
	return static_cast<PathListTab&>(*tabs[0]);
}


NAM_END
