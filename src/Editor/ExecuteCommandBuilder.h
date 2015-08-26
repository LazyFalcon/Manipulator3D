#pragma once
#include "../Robot-Commands.h"


class ExecuteCommandBuilder
{
public:
	ExecuteCommandBuilder& init(){
		execCommand = make_shared<ExecuteCommand>();
		return *this;
	}
    ExecuteCommandBuilder& name(const std::string &s){
		execCommand->name = s;
		return *this;
    }
    ExecuteCommandBuilder& onEnter(std::function<bool(RobotController &rc)> fun){
		execCommand->onEnter = fun;
		return *this;
	}
    ExecuteCommandBuilder& onUpdate(std::function<bool(RobotController &rc)> fun){
		execCommand->onUpdate = fun;
		return *this;
	}
    ExecuteCommandBuilder& onExit(std::function<bool(RobotController &rc)> fun){
		execCommand->onExit = fun;
		return *this;
	}

	ExecuteCommandBuilder& finish(RobotController &rc);

	ExecuteCommandBuilder(){}
	~ExecuteCommandBuilder(){}
	shared_ptr<ExecuteCommand> execCommand;
private:
};

