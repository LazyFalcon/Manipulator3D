#pragma once
#include "IInterpolator.h"
#include "Robot-Commands.h"

enum class CommandStatus : int {


};

enum class RCStates {
	Run, Pause, Stop,

};

class RobotController {
public:
	shared_ptr<Robot> robot;
	std::list<std::shared_ptr<ICommand>> commands;
	std::list<std::shared_ptr<ICommand>>::iterator commandIter;
	RCStates state = RCStates::Pause;

	~RobotController(){
		std::cerr<<"delete RobotController"<<std::endl;
	}
	RobotController(){
		commandIter = commands.end();
	}

	bool update(float dt);

	MoveCommand& move(shared_ptr<IInterpolator> interpolator, const std::string &name);
	WaitCommand& wait(float time);

	std::shared_ptr<ICommand>& getCommand(){
		return *commandIter;
	}

	void run();
	void pause();
	void stop();
	void next();
	void prev();

};

void RCTest(RobotController &rc);


