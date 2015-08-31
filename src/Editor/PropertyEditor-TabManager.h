#pragma once
#include "MoveCommandBuilder.h"
#include "../IInterpolator.h"

namespace Editor NAM_START

class TabManager;

class ITab
{
public:
	ITab(const std::string &s) : name(s){}
	virtual ~ITab(){};
	virtual void run(TabManager &tabManager) = 0;
	virtual void onEnter(TabManager &tabManager) = 0;

	std::string name;
};

/**
 *  Zakładka do edycji rozkazów, co ma robić?
 *  - móc stworzyć nowy rozkazów
 *  	- tworzy builder, edytuje, woła builder::done
 *  - móc edytować istniejacy rozkaz
 *		- tworzy nowy builder na podstawie typu wybranego rozkazu
			- przypisuje mu istniejacy rozkaz, pamiętać by nie wołać done

 */

/// --------------------------------------------- COMMAND EDITOR TAB ---------------------------------------------
class CommandEditorTab : public ITab
{
public:
	CommandEditorTab() : ITab("COMMAND EDITOR"){
	}
	~CommandEditorTab(){
		std::cerr<<"delete CommandEditorTab"<<std::endl;
	}
	void run(TabManager &tabManager);
	/**
	 *  called when user switch to this tab
	 */
	void onEnter(TabManager &tabManager);
	void getTypeWidget(TabManager &tabManager);
	/**
	 *  Creates new widget->builder->command
	 */
	void reset(shared_ptr<ICommand> &ptr);
	void reset(){
		commandBuilderWidget.reset();
	}
	shared_ptr<ICommand> getCommand(){
		return commandBuilderWidget->getCommand();
	}
	void applyCommand();
	/**
	 *  for command creates suitable builder and widget
	 */
	void setToEdit(shared_ptr<ICommand> &ptr);
private:
	unique_ptr<ICommandBuilderWidget> commandBuilderWidget;
};

/// --------------------------------------------- COMMAND LIST TAB ---------------------------------------------
class CommandListTab : public ITab
{
public:
	CommandListTab() : ITab("COMMAND LIST"){}
	void run(TabManager &TM);
	void onEnter(TabManager &TM){}
	~CommandListTab(){
		std::cerr<<"delete CommandListTab"<<std::endl;
	}
};

/// --------------------------------------------- PATH EDITOR TAB ---------------------------------------------
class PathEditorTab : public ITab
{
public:
	PathEditorTab() : ITab("PATH EDITOR"){}
	void run(TabManager &TM);
	void onEnter(TabManager &TM){}
	void reset(shared_ptr<ICommand> &ptr){
		if(ptr->type == MOVE)
			reset(static_pointer_cast<MoveCommand>(ptr)->interpolator);
		else
			reset();
	}
	void reset(shared_ptr<IInterpolator> &ptr){
		interpolator = ptr;
	}
	void reset(){
		interpolator.reset();
	}
	~PathEditorTab(){
		std::cerr<<"delete PathEditorTab"<<std::endl;
	}
private:
	shared_ptr<IInterpolator> interpolator;
};

/// --------------------------------------------- PATH LIST TAB ---------------------------------------------
class PathListTab : public ITab
{
public:
	PathListTab() : ITab("PATH LIST"){}
	void run(TabManager &TM);
	void onEnter(TabManager &TM){}
	~PathListTab(){
		std::cerr<<"delete PathListTab"<<std::endl;
	}
};

/// --------------------------------------------- GROUP LIST TAB ---------------------------------------------
class GroupListTab : public ITab
{
public:
	GroupListTab() : ITab("GROUP LIST"){}
	void run(TabManager &TM);
	void onEnter(TabManager &TM){}
	~GroupListTab(){
		std::cerr<<"delete GroupListTab"<<std::endl;
	}
};

/// --------------------------------------------- POINT LIST TAB ---------------------------------------------
class PointListTab : public ITab
{
public:
	PointListTab() : ITab("POINT LIST"){}
	void run(TabManager &TM);
	void onEnter(TabManager &TM){}
	~PointListTab(){
		std::cerr<<"delete PointListTab"<<std::endl;
	}
};

/// --------------------------------------------- TAB MANAGER -------
class TabManager
{
public:
	TabManager(){}
	~TabManager(){
		std::cerr<<"delete TabManager"<<std::endl;
	}
	void clearTab(u32 index){
		tabs[index].reset();
		initTabs();
	}
	void initTabs(){
		if(!tabs[0]) tabs[0].reset(new CommandEditorTab());
		if(!tabs[1]) tabs[1].reset(new CommandListTab());
		if(!tabs[2]) tabs[2].reset(new PathEditorTab());
		if(!tabs[3]) tabs[3].reset(new PathListTab());
		if(!tabs[4]) tabs[4].reset(new GroupListTab());
		if(!tabs[5]) tabs[5].reset(new PointListTab());
	}
	void setTab(u32 index){
		currentTab = index;
		tabs[currentTab]->onEnter(*this);
	}
	void run();
	void drawTabs();
	void keyCallback(){}
	void onEnter(){}

	template<typename Type>
	Type& get();

	// vector<unique_ptr<ITab>> tabs;
	unique_ptr<ITab> tabs[6];
	u32 currentTab {0};
	u32 size {200};
};

NAM_END
