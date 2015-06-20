#pragma once
#include "IInterpolator.h"
#include "JacobianTransposed.h"
class RobotController;

enum CommandType : int
{
	Empty = 0, Move, Wait, Conditional, ConditionalCall,
};

class ICommand
{
	//uint32_t flags;
public:
	//ICommand() : flags(0){}
	//ICommand(uint32_t f) : flags(f){}
	virtual bool update(RobotController &rc, float dt) = 0;
	virtual vector<glm::vec4>& getPath() = 0;
	virtual ~ICommand(){
	}

	std::string name = "--empty--";
};

class MoveCommand : public ICommand
{
public:

	MoveCommand(){}
	MoveCommand(IInterpolator *interpolator) : interpolator(interpolator){}

	bool update(RobotController &rc, float dt);
	vector<glm::vec4>& getPath() {
		return interpolator->visualisation;
	}
	~MoveCommand(){
		std::cerr << "delete move command\n";
	}

	double velocity;
	double acceleration;
	float time;
	IInterpolator *interpolator;
	IIK *solver;

};

class WaitCommand : public ICommand
{
	vector <glm::vec4> fakePath{};
	float timeLeft = 0.f;
public:
	WaitCommand(float time) : timeLeft(time){}

	bool update(RobotController &rc, float dt);
	vector<glm::vec4>& getPath() {
		return fakePath;
	}
};
class UseEffectorCommand : public ICommand {};
class ConditionalCommand : public ICommand {};
class FunctionCommand : public ICommand {};
