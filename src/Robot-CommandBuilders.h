#pragma once
#include <Utils/includes.h>
#include "IInterpolator.h"
#include "JacobianTransposed.h"
#include "Robot-Commands.h"

double parseVelocity(const std::string &word);
double parseAcceleration(const std::string &word);
float    parseTime(const std::string &word);

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
	MoveBuilder(): moveCommand(make_unique<MoveCommand>()){
		init();
	}

	MoveBuilder& velocity(double value){
		moveCommand->velocity = value;
		return *this;
	}
	MoveBuilder& velocity(std::string value){
		moveCommand->velocity = parseVelocity(value);
		return *this;
	}
	MoveBuilder& acceleration(std::string value){
		moveCommand->acceleration = parseAcceleration(value);
		return *this;
	}
	MoveBuilder& acceleration(double value){
		moveCommand->acceleration = value;
		return *this;
	}
	MoveBuilder& time(std::string value){
		moveCommand->time = parseTime(value);
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
	void reset(){moveCommand = make_unique<MoveCommand>();}
	void init();
private:
	unique_ptr<MoveCommand> moveCommand;
};

class WaitBuilder
{};

extern CommandType CommandTypeArray[];
extern std::string CommandTypeName[];
extern unique_ptr<ICommandBuilder> CommandBuilders[];

