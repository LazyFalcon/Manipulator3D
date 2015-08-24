#pragma once
#include "IInterpolator.h"
#include "JacobianTransposed.h"
class RobotController;

enum CommandType : u32
{
	EMPTY, SINGLEMOVE, MOVE, WAIT, EXECUTE,
};

class ICommand
{
	//uint32_t flags;
public:
	ICommand(CommandType type) : type(type), isRuning(false){}
	//ICommand(uint32_t f) : flags(f){}
	virtual void init(RobotController &rc) = 0;
	virtual bool update(RobotController &rc, float dt) = 0;
	virtual bool exit(RobotController &rc) = 0;
	virtual vector<glm::vec4>& getPath() = 0;
	virtual vector<glm::vec4>& getPolyline() = 0;
	virtual ~ICommand(){}
	std::string name = "--empty--";
	CommandType type;
	bool isRuning;
};

class ICommandBuilderWidget
{
public:
	virtual ~ICommandBuilderWidget(){}
	virtual void run() = 0;
	virtual void enter() = 0;
	virtual void init() = 0;
	virtual shared_ptr<ICommand> getCommand() = 0;
	// virtual void setCommand() = 0;
};

class MoveCommand : public ICommand
{
public:
	MoveCommand() : ICommand(MOVE), interpolator(make_shared<Linear>(std::vector<glm::vec4> {glm::vec4(0,0,0,1)})) {}
	// MoveCommand(IInterpolator *interpolator) : ICommand(MOVE), interpolator(interpolator){}
	MoveCommand(shared_ptr<IInterpolator> interpolator) : ICommand(MOVE), interpolator(interpolator){}
	~MoveCommand(){
		std::cerr<<"delete MoveCommand: "+name<<std::endl;
	}

	vector<glm::vec4>& getPath(){
		return interpolator->visualisation;
	}
	vector<glm::vec4>& getPolyline(){
		return interpolator->points;
	}
	void init(RobotController &rc);
	bool update(RobotController &rc, float dt);
	bool exit(RobotController &rc){}
	glm::vec4 calculateNextPoint(float dt);
	double calculateRequiredDistance(float dt);

	double velocity;
	double jointVelocityModifier {1.0};
	double acceleration;
	double requiredDistance {0.0};
	float time;
	float inTime;
	float outTime;
	bool useOrientation {false};

	shared_ptr<IInterpolator> interpolator;
	shared_ptr<IIK> solver;

private:
	glm::vec4 previousPoint;
	std::vector<double> targetJointPosition;
};

class SingleJointMove : public ICommand
{
public:
	SingleJointMove() : ICommand(SINGLEMOVE){}
	SingleJointMove(std::vector<double> &v) : ICommand(SINGLEMOVE), targetJointPosition(v){}
	void init(RobotController &rc);
	bool update(RobotController &rc, float dt);
	bool exit(RobotController &rc);
	void set(std::vector<double> &v){
		targetJointPosition = v;
	}
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

	double velocity;
	double jointVelocityModifier {1.0};
	double acceleration;
	shared_ptr<IIK> solver;
private:
	std::vector<double> targetJointPosition;
};

class WaitCommand : public ICommand
{
public:
	~WaitCommand(){
		std::cerr << "delete Wait command\n";
	}
	WaitCommand() : ICommand(WAIT), releaseTime(0){}
	WaitCommand(float time) : ICommand(WAIT), releaseTime(time){}
	bool update(RobotController &rc, float dt);

	void init(RobotController &rc);
	bool exit(RobotController &rc);
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

	float time = 0.f;
	float releaseTime = 0.f;
	u32 releaseFlag {0};
	std::function<bool(RobotController &rc)> releaseFuction;
};

class ExecuteCommand : public ICommand
{
public:
    ExecuteCommand() : ICommand(EXECUTE){}
	void init(RobotController &rc);
	bool update(RobotController &rc, float dt);
	bool exit(RobotController &rc);
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

	std::function<void(RobotController &rc)> onEnter;
	std::function<void(RobotController &rc)> onUpdate;
	std::function<void(RobotController &rc)> onExit;
};


