#pragma once
#include "../Robot-Commands.h"


class WaitCommandBuilder
{
public:
	// unique_ptr<ICommand> get(){
		// return moveCommand.release();
	// }
	WaitCommandBuilder& init(){
		moveCommand = make_shared<WaitCommand>();
		return *this;
	}
	WaitCommandBuilder& time(float releaseTime){
		waitCommand->releaseTime = releaseTime;
		return *this;
	}
	WaitCommandBuilder& time(u32 releaseFlag){
		waitCommand->releaseFlag = releaseFlag;
		return *this;
	}
	WaitCommandBuilder& time(std::function<bool(RobotController &rc)> releaseFuction){
		waitCommand->releaseFuction = releaseFuction;
		return *this;
	}
	
	WaitCommandBuilder& finish();

	WaitCommandBuilder(){}
	~WaitCommandBuilder(){}
	shared_ptr<WaitCommand> waitCommand;
private:
};


namespace Editor NAM_START

class WaitCommandBuilderWidget : public ICommandBuilderWidget
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
	void editTime();
	void editFlag();
	void editLambda();
	void finalize();

	shared_ptr<ICommand> getCommand(){
		return waitCommandBuilder->waitCommand;
	}
private:
	std::string fieldValue {""};

	unique_ptr<WaitCommandBuilder> waitCommandBuilder;
};

void WaitCommandBuilderWidget_inits();

NAM_END
