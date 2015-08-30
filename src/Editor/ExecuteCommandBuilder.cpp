#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "ExecuteCommandBuilder.h"
#include "Editor.h"
#include "../Robot-Commands.h"
#include "../Widgets.h"
#include "../IInterpolator.h"
#include "../RobotController.h"

ExecuteCommandBuilder& ExecuteCommandBuilder::finish(shared_ptr<RobotController> &RC){
	RC->insertCommand(execCommand);
	init();
	return *this;
}

// ExecutePythonCommandBuilder& ExecutePythonCommandBuilder::finish(RobotController &RC){
	// RC.insertCommand(execCommand);
	// init();
	// return *this;
// }
ExecutePythonCommandBuilder& ExecutePythonCommandBuilder::finish(shared_ptr<RobotController> RC){
	RC->insertCommand(execCommand);
	init();
	return *this;
}

