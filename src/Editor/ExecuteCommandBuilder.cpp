#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "ExecuteCommandBuilder.h"
#include "Editor.h"
#include "../Robot-Commands.h"
#include "../Widgets.h"
#include "../IInterpolator.h"
#include "../RobotController.h"

ExecuteCommandBuilder& ExecuteCommandBuilder::insert(shared_ptr<RobotController> &RC){
	RC->commandIter++;
	RC->commands.insert(RC->commandIter, execCommand);
	RC->commandIter--;
	init();
	return *this;
}

ExecuteCommandBuilder& ExecuteCommandBuilder::finish(shared_ptr<RobotController> &RC){
	RC->pushCommand(execCommand);
	init();
	return *this;
}

// ExecutePythonCommandBuilder& ExecutePythonCommandBuilder::finish(RobotController &RC){
	// RC.pushCommand(execCommand);
	// init();
	// return *this;
// }
ExecutePythonCommandBuilder& ExecutePythonCommandBuilder::finish(shared_ptr<RobotController> RC){
	RC->pushCommand(execCommand);
	init();
	return *this;
}

ExecutePythonCommandBuilder& ExecutePythonCommandBuilder::insert(shared_ptr<RobotController> RC){
	RC->commandIter++;
	RC->commands.insert(RC->commandIter, execCommand);
	RC->commandIter--;
	init();
	return *this;
}

