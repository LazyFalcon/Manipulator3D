#pragma once
#include "../Robot-Commands.h"
class RobotController;

class MoveCommandBuilder
{
public:
	MoveCommandBuilder& init(){
		moveCommand = make_shared<MoveCommand>();
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
	MoveCommandBuilder& jointVelocity(double value){
		moveCommand->jointVelocityModifier = value;
		return *this;
	}
	MoveCommandBuilder& acceleration(double value){
		moveCommand->acceleration = value;
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
		// moveCommand->solver = value;
		moveCommand->solver = make_unique<JT0>();
		return *this;
	}
	MoveCommandBuilder& solver(IIK *value){
		// moveCommand->solver = value;
		moveCommand->solver = make_unique<JT0>();
		return *this;
	}
	MoveCommandBuilder& finish(shared_ptr<RobotController> RC);
	MoveCommandBuilder& finish(RobotController &RC);

	MoveCommandBuilder(){}
	~MoveCommandBuilder(){}
	shared_ptr<MoveCommand> moveCommand;
private:
};

class SingleJointMoveCommandBuilder
{
public:
	SingleJointMoveCommandBuilder& init(){
		moveCommand = make_shared<SingleJointMove>();
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
	SingleJointMoveCommandBuilder& finish(shared_ptr<RobotController> RC);
	SingleJointMoveCommandBuilder& finish(RobotController &RC);

	SingleJointMoveCommandBuilder(){}
	~SingleJointMoveCommandBuilder(){}
	shared_ptr<SingleJointMove> moveCommand;
};

class FollowObjectBuilder
{
public:
	FollowObjectBuilder& init(){
		moveCommand = make_shared<FollowObject>();
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
