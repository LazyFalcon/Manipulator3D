#pragma once
#include "../Robot-Commands.h"


class ExecuteCommandBuilder
{
public:
	ExecuteCommandBuilder& init(int commandExitAction = 1){
		execCommand = make_shared<ExecuteCommand>();
		execCommand->exitAction = commandExitAction;
		return *this;
	}
	ExecuteCommandBuilder& name(const std::string &s){
		execCommand->name = s;
		return *this;
	}
	ExecuteCommandBuilder& onEnter(std::function<void(shared_ptr<RobotController> &rc)> fun){
		execCommand->onEnter = fun;
		return *this;
}
	ExecuteCommandBuilder& onUpdate(std::function<bool(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt)> fun){
		execCommand->onUpdate = fun;
		return *this;
}
	ExecuteCommandBuilder& onExit(std::function<void(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene)> fun){
		execCommand->onExit = fun;
		return *this;
}

	ExecuteCommandBuilder& finish(shared_ptr<RobotController> &rc);
	ExecuteCommandBuilder& insert(shared_ptr<RobotController> &rc, int distance);

	ExecuteCommandBuilder(){}
	~ExecuteCommandBuilder(){}
	shared_ptr<ExecuteCommand> execCommand;
private:
};


class ExecutePythonCommandBuilder
{
public:
	ExecutePythonCommandBuilder& init(int commandExitAction = 1){
		execCommand = make_shared<ExecutePythonCommand>();
		execCommand->exitAction = commandExitAction;
		return *this;
	}
	ExecutePythonCommandBuilder& name(const std::string s){
		execCommand->name = s;
		return *this;
	}
	ExecutePythonCommandBuilder& callback(boost::python::object &fun){
		execCommand->callback = fun;
		return *this;
	}

	// ExecutePythonCommandBuilder& finish(shared_ptr<RobotController> &rc);
	ExecutePythonCommandBuilder& finish(shared_ptr<RobotController> RC);
	ExecutePythonCommandBuilder& insert(shared_ptr<RobotController> RC, int distance);

	ExecutePythonCommandBuilder(){}
	~ExecutePythonCommandBuilder(){}
	shared_ptr<ExecutePythonCommand> execCommand;
private:
};

