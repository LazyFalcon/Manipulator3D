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
	~RobotController(){
		std::cerr<<"delete RobotController"<<std::endl;
	}
	RobotController() : grabbedObject(nullptr){
		commandIter = commands.end();
	}

	bool update(float dt);

	MoveCommand& move(shared_ptr<IInterpolator> interpolator, const std::string &name);
	WaitCommand& wait(float time);
	WaitCommand& execute(float time){}
	void grabObject(Entity *obj);
	/// znaleźć obiekt, jego orientację, wyrównać robota do osi/płaszczyzny, przypiąć pozycję
	/// w update przenosić obiekt


	void insertCommand(shared_ptr<ICommand> &ptr){
		commands.push_back(ptr);
	}
	void insertCommand(shared_ptr<ICommand> ptr){
		commands.push_back(ptr);
	}
	std::shared_ptr<ICommand>& getCommand(){
		return *commandIter;
	}

	void run();
	void pause();
	void stop();
	void next();
	void prev();

	shared_ptr<Robot> robot;
	std::list<std::shared_ptr<ICommand>> commands;
	std::list<std::shared_ptr<ICommand>>::iterator commandIter;
	RCStates state = RCStates::Pause;
	Entity *grabbedObject;
};

void RCTest(RobotController &rc);


