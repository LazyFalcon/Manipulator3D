#pragma once
#include "MoveCommandBuilder.h"
#include "../IInterpolator.h"

namespace Editor NAM_START

class TabManager;

class ITab
{
public:
	virtual ~ITab(){};
	virtual void run(TabManager &tabManager) = 0;
	virtual void onEnter(TabManager &tabManager) = 0;
};

/**
 *  Zakładka do edycji rozkazów, co ma robić?
 *  - móc stworzyć nowy rozkazów
 *  	- tworzy builder, edytuje, woła builder::done
 *  - móc edytować istniejacy rozkaz
 *		- tworzy nowy builder na podstawie typu wybranego rozkazu
			- przypisuje mu istniejacy rozkaz, pamiętać by nie wołać done

 */
class CommandEditorTab : public ITab
{
public:
	CommandEditorTab(){
		// reset();
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

	void doneWidget(){}
	/**
	 *  for command creates suitable builder and widget
	 */
	void setToEdit(shared_ptr<ICommand> &ptr);
private:
	unique_ptr<ICommandBuilderWidget> commandBuilderWidget;
};

// trzeba skdas przechwycić RC
class CommandListTab : public ITab
{
public:
	void run(TabManager &TM);
	void onEnter(TabManager &TM){}
	~CommandListTab(){
		std::cerr<<"delete CommandListTab"<<std::endl;
	}
};

class PathEditorTab : public ITab
{
public:
	void run(TabManager &TM){}
	void onEnter(TabManager &TM){}
	void reset(shared_ptr<ICommand> &ptr){
		if(ptr->type == MOVE)
			reset(static_pointer_cast<MoveCommand>(ptr)->interpolator);
		// else

	}
	void reset(shared_ptr<IInterpolator> &ptr){

	}
	void reset(){}

};

class PathListTab : public ITab
{
public:
	void run(TabManager &TM);
	void onEnter(TabManager &TM){}
};

/// -------- TAB MANAGER -------
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
		// if(tabs.size() < 4){
			// tabs.resize(4);
		// }
		if(!tabs[0]) tabs[0].reset(new CommandEditorTab());
		if(!tabs[1]) tabs[1].reset(new CommandListTab());
		if(!tabs[2]) tabs[2].reset(new PathEditorTab());
		if(!tabs[3]) tabs[3].reset(new PathListTab());
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
	unique_ptr<ITab> tabs[4];
	u32 currentTab {0};
	u32 size {200};
};

NAM_END


