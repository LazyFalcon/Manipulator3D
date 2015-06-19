#pragma once
/* flow
	┌──────────────────┐
	├	move             │
	├	move             │
	├	waitOnKey        │
	├	conditional(key) │
	│	 ├ move          │
	│	 └ useGripper    │
	├	move             │
	├ useGripper       │
	└──────────────────┘
*/
#include "RobotCommands.h"

enum class CommandStatus : int {


};

enum class RCStates {
	Run, Pause, Stop,

};

class RobotController {
public:
	Robot *robot;
	IIK *solver;
	std::list<std::unique_ptr<ICommand>> commands;
	std::list<std::unique_ptr<ICommand>>::iterator commandIter;
	RCStates state = RCStates::Pause;

	RobotController(){
		commandIter = commands.end();
	}

	bool update(float dt);

	MoveCommand& move(IInterpolator *interpolator, const std::string &name);
	WaitCommand& wait(float time);

	void run();
	void pause();
	void stop();
	void next();
	void prev();

};

void RCTest(RobotController &rc);


