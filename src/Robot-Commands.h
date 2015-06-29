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
	// ICommand() : isRuning(false){}
	//ICommand(uint32_t f) : flags(f){}
	virtual void init(RobotController &rc) = 0;
	virtual bool update(RobotController &rc, float dt) = 0;
	virtual vector<glm::vec4>& getPath() = 0;
	virtual vector<glm::vec4>& getPolyline() = 0;
	virtual ~ICommand(){}
	std::string name = "--empty--";
	bool isRuning;
};

class MoveCommand : public ICommand
{
public:

	MoveCommand(){}
	MoveCommand(IInterpolator *interpolator) : interpolator(interpolator){}
	~MoveCommand(){
		// std::cerr << "delete move command\n";
		delete interpolator;
		delete solver;
	}

	vector<glm::vec4>& getPath(){
		return interpolator->visualisation;
	}
	vector<glm::vec4>& getPolyline(){
		return interpolator->points;
	}
	void init(RobotController &rc);
	bool update(RobotController &rc, float dt);
	glm::vec4 calculateNextPoint(float dt);
	double calculateRequiredDistance(float dt);

	double velocity;
	double acceleration;
	double requiredDistance {0.0};
	float time;
	IInterpolator *interpolator;
	IIK *solver;

private:
	glm::vec4 previousPoint;
	std::vector<double> targetJointPosition;

};

class WaitCommand : public ICommand
{
	vector <glm::vec4> fakePath{};
	float timeLeft = 0.f;
public:
	~WaitCommand(){
		// std::cerr << "delete wait command\n";
	}
	WaitCommand(float time) : timeLeft(time){}
	void init(RobotController &rc){
		isRuning = true;
	};
	bool update(RobotController &rc, float dt);
	vector<glm::vec4>& getPath(){
		return fakePath;
	}
	vector<glm::vec4>& getPolyline(){
		return fakePath;
	}


};
class UseEffectorCommand : public ICommand {};
class ConditionalCommand : public ICommand {};
class FunctionCommand : public ICommand {};
