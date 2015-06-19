#include <Utils/includes.h>
#include "IInterpolator.h"
#include "JacobianTransposed.h"
#include "RobotCommands.h"

class ICommandBuilder
{
public:
	virtual unique_ptr<ICommand> operator()() = 0;
	virtual ~ICommandBuilder(){}
	virtual void widget() = 0;
};

/**
 *  Code:
 *  MoveBuilder mv;
 *  mv.interpolator(Linear).solver(JT0).velocity("3m/s")();
 *
 *
 */
class MoveBuilder : public ICommandBuilder
{
public:
	unique_ptr<ICommand> operator()(){
		// moveCommand->process();
		// return moveCommand;
	}
	MoveBuilder(): moveCommand(make_unique<MoveCommand>()){}

	MoveBuilder& velocity(double value){
		moveCommand->velocity = value;
		return *this;
	}
	MoveBuilder& velocity(std::string value){
		// moveCommand->velocity = parse<double>(value);
		return *this;
	}
	MoveBuilder& acceleration(double value){
		moveCommand->acceleration = value;
		return *this;
	}
	MoveBuilder& time(float value){
		moveCommand->time = value;
		return *this;
	}
	MoveBuilder& interpolator(Interpolator value){
		// build interpolator from enum
		return *this;
	}
	MoveBuilder& interpolator(IInterpolator *value){
		moveCommand->interpolator = value;
		return *this;
	}
	MoveBuilder& solver(IIK *value){
		moveCommand->solver = value;
		return *this;
	}
	void widget();
private:
	unique_ptr<MoveCommand> moveCommand;
};



extern CommandType CommandTypeArray[];
extern std::string CommandTypeName[];
extern unique_ptr<ICommandBuilder> CommandBuilders[];

