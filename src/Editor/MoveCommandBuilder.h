#pragma once
#include "../Robot-Commands.h"
class RobotController;

class MoveCommandBuilder
{
public:
	MoveCommandBuilder& init(int commandExitAction = 1){
		moveCommand = make_shared<MoveCommand>();
		moveCommand->solver = make_shared<JT0>();
		moveCommand->exitAction = commandExitAction;
		return *this;
	}
	MoveCommandBuilder& name(const std::string &s){
		moveCommand->name = s;
		return *this;
	}
	MoveCommandBuilder& velocity(double value){
		moveCommand->velocity = value;
		return *this;
	}
	MoveCommandBuilder& startO(glm::quat value){
		moveCommand->startOrientationEnabled = true;
		moveCommand->startOrientation = value;
		return *this;
	}
	MoveCommandBuilder& endO(glm::quat value){
		moveCommand->endOrientationEnabled = true;
		moveCommand->endOrientation = value;
		return *this;
	}
	MoveCommandBuilder& jointVelocity(double value){
		moveCommand->jointVelocityModifier = value;
		return *this;
	}
	MoveCommandBuilder& acceleration(double value){
		moveCommand->acceleration = value;
		return *this;
	}
		MoveCommandBuilder& treshold(double value){
		moveCommand->distanceTreshold = value;
		return *this;
	}
	MoveCommandBuilder& time(float value){
		moveCommand->time = value;
		return *this;
	}
	MoveCommandBuilder& interpolator(shared_ptr<IInterpolator> &value){
		moveCommand->interpolator = value;
		return *this;
	}
	MoveCommandBuilder& interpolator(IInterpolatorContainer &value){
		moveCommand->interpolator = value.interpolator;
		return *this;
	}
	MoveCommandBuilder& interpolator(Interpolator value){
		// build interpolator from enum
		return *this;
	}
	MoveCommandBuilder& interpolator(const std::string& name){
		// build interpolator from enum
		return *this;
	}
	MoveCommandBuilder& solver(const std::string& name){
		if(name == "JT0") moveCommand->solver = make_shared<JT0>();
		else if(name == "JT1") moveCommand->solver = make_shared<JT1>();
		else if(name == "JT2") moveCommand->solver = make_shared<JT2>();
		return *this;
	}
	MoveCommandBuilder& solver(SolverType value){
		if(value == SolverType::JT0) moveCommand->solver = make_shared<JT0>();
		else if(value == SolverType::JT1) moveCommand->solver = make_shared<JT1>();
		else if(value == SolverType::JT2) moveCommand->solver = make_shared<JT2>();
		return *this;
	}
	MoveCommandBuilder& solver(shared_ptr<Solver> &value){
		moveCommand->solver = value;
		return *this;
	}
	/// for goTo
	MoveCommandBuilder& to(glm::vec4 v){
		moveCommand->interpolator = addInterpolator(Interpolator::Simple, {v}, "--");
		return *this;
	}
	MoveCommandBuilder& orientation(glm::vec3 o){
		moveCommand->solver = make_shared<JT2>();
		moveCommand->endOrientationEnabled = true;
		moveCommand->startOrientationEnabled = true;
		moveCommand->startOrientation = glm::quat(1.f, glm::normalize(o));
		moveCommand->endOrientation = glm::quat(1.f, glm::normalize(o));
		return *this;
	}
	MoveCommandBuilder& orientation(glm::quat o){
		moveCommand->solver = make_shared<JT2>();
		moveCommand->startOrientationEnabled = true;
		moveCommand->endOrientationEnabled = true;
		moveCommand->endOrientation = o;
		moveCommand->startOrientation = o;
		return *this;
	}
	MoveCommandBuilder& offset(glm::vec4 v){
		moveCommand->interpolator->points.back() += v;
		return *this;
	}
	MoveCommandBuilder& pid(double k, double kd, double ki){
		moveCommand->regulator = PDreg {k, kd, ki};
		return *this;
	}
	MoveCommandBuilder& finish(shared_ptr<RobotController> RC);
	MoveCommandBuilder& finish(RobotController &RC);
	MoveCommandBuilder& insert(shared_ptr<RobotController> &RC, int distance);

	MoveCommandBuilder(){}
	~MoveCommandBuilder(){}
	shared_ptr<MoveCommand> moveCommand;
private:
};

class SingleJointMoveCommandBuilder
{
public:
	SingleJointMoveCommandBuilder& init(int commandExitAction = 1){
		moveCommand = make_shared<SingleJointMove>();
		moveCommand->exitAction = commandExitAction;
		return *this;
	}
	SingleJointMoveCommandBuilder& name(const std::string &s){
		moveCommand->name = s;
		return *this;
	}
	SingleJointMoveCommandBuilder& set(std::vector<double> &v){
		moveCommand->set(v);
		return *this;
	}
	SingleJointMoveCommandBuilder& velocity(double value){
		moveCommand->velocity = value;
		return *this;
	}
	SingleJointMoveCommandBuilder& jointVelocity(double value){
		moveCommand->jointVelocityModifier = value;
		return *this;
	}
	SingleJointMoveCommandBuilder& acceleration(double value){
		moveCommand->acceleration = value;
		return *this;
	}
	SingleJointMoveCommandBuilder& insert(shared_ptr<RobotController> RC, int distance);
	SingleJointMoveCommandBuilder& finish(shared_ptr<RobotController> RC);
	SingleJointMoveCommandBuilder& finish(RobotController &RC);

	SingleJointMoveCommandBuilder(){}
	~SingleJointMoveCommandBuilder(){}
	shared_ptr<SingleJointMove> moveCommand;
};

class FollowObjectBuilder
{
public:
	FollowObjectBuilder& init(int commandExitAction = 1){
		moveCommand = make_shared<FollowObject>();
		moveCommand->solver = make_shared<JT0>();
		moveCommand->exitAction = commandExitAction;
		return *this;
	}
	FollowObjectBuilder& name(const std::string &s){
		moveCommand->name = s;
		return *this;
	}
	FollowObjectBuilder& target(glm::vec4 &t){
		moveCommand->set(t);
		return *this;
	}
	FollowObjectBuilder& target(shared_ptr<Entity> &obj){
		moveCommand->set(obj->position);
		return *this;
	}
	FollowObjectBuilder& velocity(double value){
		moveCommand->velocity = value;
		return *this;
	}
	FollowObjectBuilder& jointVelocity(double value){
		moveCommand->jointVelocityModifier = value;
		return *this;
	}
	FollowObjectBuilder& acceleration(double value){
		moveCommand->acceleration = value;
		return *this;
	}
	FollowObjectBuilder& insert(shared_ptr<RobotController> RC, int distance);
	FollowObjectBuilder& finish(shared_ptr<RobotController> RC);
	FollowObjectBuilder& finish(RobotController &RC);

	FollowObjectBuilder(){}
	~FollowObjectBuilder(){}
	shared_ptr<FollowObject> moveCommand;
};

namespace Editor NAM_START

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

class MoveCommandBuilderWidget : public ICommandBuilderWidget
{
public:
	MoveCommandBuilderWidget(){
		init();
	}
	MoveCommandBuilderWidget(shared_ptr<ICommand> &ptr){
        std::cerr<<ptr.get()<<std::endl;
		moveCommandBuilder = make_unique<MoveCommandBuilder>();
		moveCommandBuilder->moveCommand = static_pointer_cast<MoveCommand>(ptr);
	}
	void init(){
		moveCommandBuilder = make_unique<MoveCommandBuilder>();
		moveCommandBuilder->init();
	}
	void run();
	void enter();
	void editName();
	void editVelocity();
	void editJointVelocity();
	void editAcceleration();
	void editTime();;
	void editInterpolator();
	void editSolver();
	void finalize();

	shared_ptr<ICommand> getCommand(){
		return static_pointer_cast<ICommand>(moveCommandBuilder->moveCommand);
	}
private:
	std::string fieldValue {""};

	unique_ptr<MoveCommandBuilder> moveCommandBuilder;
};

void MoveCommandBuilderWidget_inits();
void MoveCommandBuilderWidget_terminate();

NAM_END
