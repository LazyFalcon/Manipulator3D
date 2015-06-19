#pragma once
#include "RobotController.h"

class CommandBuilderTab : public ITab
{
public:
	void run();
	CommandBuilderTab(const std::string &icon) : ITab(icon){}

private:
	// DropDownEnumList dropDownCommandList;
};

class CommanListTab : public ITab
{
public:
	void run(){}
	CommanListTab(const std::string &icon) : ITab(icon){}
};

void CommandBuilderTab::run(){
	// if(not dropDownCommandList) return;

	/// list of build options, flat

}






