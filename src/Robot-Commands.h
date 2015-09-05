#pragma once
#include <boost/python/object.hpp>
#include "IInterpolator.h"
#include "JacobianTransposed.h"
class RobotController;
class Scene;

enum CommandType : u32
{
	EMPTY, SINGLEMOVE, MOVE, FOLLOW, WAIT, EXECUTE, EXECUTE_PY
};

namespace CommandReturnAction {
enum CommandReturn : int {
	None = 0,
	GoToNext = 1,
	Delete = 2,
	GoToPrevious = 4,

	DelAndForward = Delete | GoToNext,
	DelAndBack = Delete | GoToPrevious,

};
}
class ICommand
{
	//uint32_t flags;
public:
	ICommand(CommandType type) : type(type), isRuning(false), exitAction(CommandReturnAction::GoToNext){}
	//ICommand(uint32_t f) : flags(f){}
	virtual void init(shared_ptr<RobotController> &rc) = 0;
	virtual int update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt) = 0;
	virtual int exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene) = 0;
	virtual vector<glm::vec4>& getPath() = 0;
	virtual vector<glm::vec4>& getPolyline() = 0;
	virtual ~ICommand(){}
	std::string name = "--empty--";
	CommandType type;
	bool isRuning;
	int exitAction;
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
	void init(shared_ptr<RobotController> &rc);
	int update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt);
	int exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){}
	glm::vec4 calculateNextPoint(float dt);
	double calculateRequiredDistance(float dt);

	glm::quat startOrientation;
	glm::quat endOrientation;
	bool startOrientationEnabled;
	bool endOrientationEnabled;

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
	void init(shared_ptr<RobotController> &rc);
	int update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt);
	int exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
	void set(std::vector<double> &v){
		targetJointPosition = v;
	}
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

	double velocity;
	double jointVelocityModifier {1.0};
	double acceleration;
private:
	std::vector<double> targetJointPosition;
};

/**
 *  Na razie ostre skręty, potem wsadźmy do Robot::Module::goTo jakąś interpolację kubiczną dla kilku poprzednich jPunktów, następnego i docelowego, z jakąś predykcją?
 */
class FollowObject : public ICommand
{
public:
	FollowObject() : ICommand(FOLLOW), target(nullptr){}
	FollowObject(std::vector<double> &v) : ICommand(FOLLOW), targetJointPosition(v), target(nullptr){}
	void init(shared_ptr<RobotController> &rc);
	int update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt);
	int exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
	void set(glm::vec4 &t){
		target = &t;
        pTarget = glm::vec4(0);
	}
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

    glm::vec4 *target;
    glm::vec4 pTarget;
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
	int update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt);

	void init(shared_ptr<RobotController> &rc);
	int exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

	float time = 0.f;
	float releaseTime = 0.f;
	u32 releaseFlag {0};
	std::function<bool(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene)> releaseFuction;
};

class ExecuteCommand : public ICommand
{
public:
    ExecuteCommand() : ICommand(EXECUTE){}
	void init(shared_ptr<RobotController> &rc);
	int update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt);
	int exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

	std::function<void(shared_ptr<RobotController> &rc)> onEnter;
	std::function<bool(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt)> onUpdate;
	std::function<void(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene)> onExit;
};

class ExecutePythonCommand : public ICommand
{
public:
	ExecutePythonCommand() : ICommand(EXECUTE_PY){}
	~ExecutePythonCommand(){
		std::cerr<<"delete py callback"<<std::endl;
	}
	void init(shared_ptr<RobotController> &rc);
	int update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt);
	int exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
	vector<glm::vec4>& getPath();
	vector<glm::vec4>& getPolyline();

	boost::python::object callback;
};


