#pragma once
#include "../Robot-Commands.h"


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
	MoveCommandBuilder& velocity(double value){
		moveCommand->velocity = value;
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
	MoveCommandBuilder& interpolator(Interpolator value){
		// build interpolator from enum
		return *this;
	}
	MoveCommandBuilder& solver(IIK *value){
		// moveCommand->solver = value;
		moveCommand->solver = make_unique<JT1>();
		return *this;
	}
	MoveCommandBuilder& finish();

	MoveCommandBuilder(){}
	~MoveCommandBuilder(){}
	shared_ptr<MoveCommand> moveCommand;
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
	void editAcceleration();
	void editTime();;
	void editInterpolator();
	void editSolver();
	void finalize();

	shared_ptr<ICommand> getCommand(){
		return moveCommandBuilder->moveCommand;
	}
private:
	std::string fieldValue {""};

	unique_ptr<MoveCommandBuilder> moveCommandBuilder;
};

void MoveCommandBuilderWidget_inits();

NAM_END
