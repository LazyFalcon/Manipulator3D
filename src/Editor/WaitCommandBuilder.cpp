#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "WaitCommandBuilder.h"
#include "Editor.h"
#include "../Robot-Commands.h"
#include "../Widgets.h"
#include "../IInterpolator.h"
#include "../RobotController.h"

#define FIELDWITHNAME(name, widget) vertical(ui.rect(120,17).text(name, "ui_10"s)();\
	horizontal(\
	ui.border(2);\
		widget\
	)\
   )

#define INCR(value) \
			ui.rect(15, 22).text("-", UI::CenterText)\
				.onlPressed([this]{incr(value);}, 25u)\
				.onlClick([this]{incr(value);})\
				(UI::Button);

#define DECR(value) \
			ui.rect(15, 22).text("-", UI::CenterText)\
				.onlPressed([this]{decr(value);}, 25u)\
				.onlClick([this]{decr(value);})\
				(UI::Button);

#define EDIT(value) \
			ui.rect(120,22).edit(value)(UI::EditBox);

WaitCommandBuilder& WaitCommandBuilder::finish(shared_ptr<RobotController>& RC){
	RC->commands.push_back(waitCommand);
	init();
	return *this;
}
WaitCommandBuilder& WaitCommandBuilder::insert(shared_ptr<RobotController>& RC, int distance){
	auto iteratorCopy = RC->commandIter;
	std::advance(iteratorCopy, distance);
	RC->commands.insert(iteratorCopy, waitCommand);

	init();
	return *this;
}
WaitCommandBuilder& WaitCommandBuilder::finish(RobotController &RC){
	RC.commands.push_back(waitCommand);
	init();
	return *this;
}
