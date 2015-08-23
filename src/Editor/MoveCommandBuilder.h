#pragma once
#include "../Robot-Commands.h"
class RobotController;

class MoveCommandBuilder
{
public:
	// unique_ptr<ICommand> get(){
		// return moveCommand.release();
	// }
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
		moveCommand->solver = make_unique<JT1>();
		return *this;
	}
	MoveCommandBuilder& solver(IIK *value){
		// moveCommand->solver = value;
		moveCommand->solver = make_unique<JT1>();
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
	// unique_ptr<ICommand> get(){
		// return moveCommand.release();
	// }
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
private:
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

NAM_END
