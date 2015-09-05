#pragma once
#include "../Robot-Commands.h"

class WaitCommandBuilder
{
public:
	WaitCommandBuilder& init(int commandExitAction = 1){
		waitCommand = make_shared<WaitCommand>();
		waitCommand->exitAction = commandExitAction;
		return *this;
	}
	WaitCommandBuilder& name(const std::string &name){
		waitCommand->name = name;
		return *this;
	}
	WaitCommandBuilder& time(float releaseTime){
		waitCommand->releaseTime = releaseTime;
		return *this;
	}
	// WaitCommandBuilder& time(u32 releaseFlag){
		// waitCommand->releaseFlag = releaseFlag;
		// return *this;
	// }
	// WaitCommandBuilder& time(std::function<bool(RobotController &rc)> releaseFuction){
		// waitCommand->releaseFuction = releaseFuction;
		// return *this;
	// }

	WaitCommandBuilder& finish(shared_ptr<RobotController>& RC);
	WaitCommandBuilder& finish(RobotController &RC);

	WaitCommandBuilder(){}
	~WaitCommandBuilder(){}
	shared_ptr<WaitCommand> waitCommand;
};


namespace Editor NAM_START

class WaitCommandBuilderWidget : public ICommandBuilderWidget
{
public:
	WaitCommandBuilderWidget(){
		init();
	}
	WaitCommandBuilderWidget(shared_ptr<ICommand> &ptr){
		waitCommandBuilder = make_unique<WaitCommandBuilder>();
		waitCommandBuilder->waitCommand = static_pointer_cast<WaitCommand>(ptr);
	}
	void init(){
		waitCommandBuilder = make_unique<WaitCommandBuilder>();
		waitCommandBuilder->init();
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
