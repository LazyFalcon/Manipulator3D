#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "../IInterpolator.h"
#include "../Widgets.h"
#include "../Helper.h"
#include "Editor.h"
#include "PropertyEditor-TabManager.h"
#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";
// FHDJB-J8KMP-RFV8D-FX3FT-YDBCW
extern UI::IMGUI ui;
extern glm::vec2 screenSize;
namespace Editor NAM_START

/// ----- TAB MANAGER ---------------------------------------------
void TabManager::run(){

	ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw)
		.style(UI::StyleID::Dark)
		.position(screenSize.x-size, screenSize.y-32)
		.size(size, -screenSize.y+32);
	drawTabs();
	ui.rect(10, 20).text(tabs[currentTab]->name)();
	tabs[currentTab]->run(*this);
	if(!ui.outOfTable()) ui.mouseOverButton = true;
	ui.endTable();
}

void TabManager::drawTabs(){
	ui.box(UI::LayoutHorizontal);

	// for(u32 i=0; i<tabs.size(); i++){
	for(u32 i=0; i<6; i++){
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
	return static_cast<CommandListTab&>(*tabs[1]);
}
template<>
PathEditorTab& TabManager::get<PathEditorTab>(){
	return static_cast<PathEditorTab&>(*tabs[2]);
}
template<>
PathListTab& TabManager::get<PathListTab>(){
	return static_cast<PathListTab&>(*tabs[3]);
}
template<>
GroupListTab& TabManager::get<GroupListTab>(){
	return static_cast<GroupListTab&>(*tabs[4]);
}
template<>
PointListTab& TabManager::get<PointListTab>(){
	return static_cast<PointListTab&>(*tabs[5]);
}

unique_ptr<ICommandBuilderWidget> createWidgetFromCommandType(CommandType type){
	unique_ptr<ICommandBuilderWidget> out;
	if(type == MOVE){
		out.reset(new MoveCommandBuilderWidget());
	}
	else if(type == WAIT){
		// out.reset(new WaitCommandBuilderWidget());
	}
	else if(type == EXECUTE){
		// out.reset(new ExecuteCommandBuilderWidget());
	}

	return out;
}
unique_ptr<ICommandBuilderWidget> createWidgetFromCommandAndSetPtr(shared_ptr<ICommand> &ptr){
	unique_ptr<ICommandBuilderWidget> out;
	auto type =ptr->type;
	if(type == MOVE){
		out.reset(new MoveCommandBuilderWidget(ptr));
	}
	else if(type == WAIT){
		// out.reset(new WaitCommandBuilderWidget(ptr));
	}
	else if(type == EXECUTE){
		// out.reset(new ExecuteCommandBuilderWidget(ptr));
	}

	return out;
}

/// ----- COMMAND EDITOR TAB ---------------------------------------------
wxg::DropdownPairWithCallback<CommandType> CommandTypes (UI::AlignTop, 100, std::vector <pair<string, CommandType>>{
	{"----", EMPTY},
	{"Move", MOVE},
	{"Wait", WAIT},
	{"Execute", EXECUTE},
});


void CommandEditorTab::run(TabManager &TM){
	getTypeWidget(TM);
	if(commandBuilderWidget){
		commandBuilderWidget->run();
		applyCommand();
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
	CommandTypes.run([this](CommandType val){
		commandBuilderWidget = createWidgetFromCommandType(val);
	});
}
void CommandEditorTab::applyCommand(){
	ui.rect(120, 30).text("Apply")(UI::Label)
		.onlClick([this]{
			// getRC().insertCommand(commandBuilderWidget->getCommand());
			commandBuilderWidget = createWidgetFromCommandType(EMPTY);
		});
}


/// ----- COMMAND LIST TAB ---------------------------------------------
void CommandListTab::run(TabManager &TM){
	// for(auto &it : RC.commands){
		// ui.rect(150, 20).text(it->name)()
			// .onrClick(TM.get<CommandEditorTab>().setToEdit(it));
	// }

}

/// ----- PATH EDITOR TAB ---------------------------------------------
void PathEditorTab::run(TabManager &TM){
	if(interpolator){
		interpolator->drawParams();
	}
}

/// ----- PATH LIST TAB ---------------------------------------------
void PathListTab::run(TabManager &TM){
	for(auto &it : getInterpolators()){
		ui.rect(150, 20).text(it->name)()
			.onlClick([&TM, &it]{TM.get<PathEditorTab>().reset(it);});
		ui.rect(150, 1).color(0xFFFFFFFF)(UI::Label);
	}

}


/// ----- GROUP LIST TAB ---------------------------------------------
void GroupListTab::run(TabManager &TM){
	auto &list = Helper::groupList();
	for(auto &it : list){
		ui.rect(150, 20).text(it.first)();
		ui.rect(150, 1).color(0xFFFFFFFF)(UI::Label)
			// .onlClick([&TM, &it]{TM.get<PathEditorTab>().reset(it);})
			;
	}

}

/// ----- POINT LIST TAB ---------------------------------------------
void PointListTab::run(TabManager &TM){
	auto &list = Helper::pointList();
	for(auto it = list.begin(); it!=list.end(); it++){
		horizontal(
			ui.rect(120, 20).text(it->first)(UI::CaptureMouse)
				.onHover([&it](glm::vec4 v){
					v.x += 10;
					v.y -= 20;
					v.z = 120;

					ui.beginLayer();
					ui.rect(v, 0xFF8080FF).text(to_string(it->second))(UI::Label);
					ui.endLayer();
				})
				;
			ui.rect(20, 20).text(">")(UI::CaptureMouse).onlClick([&it]{ Helper::setCursor(it->second); });
			ui.rect(20, 20).text(">>")(UI::CaptureMouse).onlClick([&it, &TM]{
				if(TM.get<CommandEditorTab>().getCommand()->type == MOVE){
					Editor::getPolyline().insertAtEnd(it->second);
					Editor::getPolyline().polyline->generatePath();
				}
			});
			ui.rect(20, 20).text("X")(UI::CaptureMouse).onlClick([&it]{ Helper::deletePoint(it->first); });
			);
		// ui.rect(150, 1).color(0xFFFFFFFF)(UI::Label);
	}

}


NAM_END
