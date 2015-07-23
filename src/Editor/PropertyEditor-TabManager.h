#pragma once
#include "MoveCommandBuilder.h"

namespace Editor NAM_START

class TabManager;

class ITab
{
public:
	virtual ~ITab(){};
	virtual void run(TabManager &tabManager) = 0;
	virtual void enter(TabManager &tabManager) = 0;
};

// unique_ptr<ICommandBuilderWidget> createWidgetFromCommandAndSetPtr(shared_ptr<ICommand> &ptr);

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
	~CommandEditorTab(){
		std::cerr<<"delete CommandEditorTab"<<std::endl;
	}
	void run(TabManager &tabManager);
	void enter(TabManager &tabManager);
	void getTypeWidget(TabManager &tabManager);

	void doneWidget(){}
	void setToEdit(shared_ptr<ICommand> &ptr){
		// buildWidget(ptr->type);
		// commandBuilderWidget->moveCommand = ptr;
	}
private:
	unique_ptr<ICommandBuilderWidget> commandBuilderWidget;
};

// trzeba skdas przechwycić RC
class CommandListTab : public ITab
{
public:
	void run(TabManager &TM);
	void enter(TabManager &TM){}
	~CommandListTab(){
		std::cerr<<"delete CommandListTab"<<std::endl;
	}
};

class PathEditorTab : public ITab
{
public:
	void run(TabManager &TM){}
	void enter(TabManager &TM){}

};

class PathListTab : public ITab
{
public:
	void run(TabManager &TM);
	void enter(TabManager &TM){}
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
		if(tabs.size() < 4){
			tabs.resize(4);
		}
		if(!tabs[0]) tabs[0].reset(new CommandEditorTab());
		if(!tabs[1]) tabs[1].reset(new CommandListTab());
		if(!tabs[2]) tabs[2].reset(new PathEditorTab());
		if(!tabs[3]) tabs[3].reset(new PathListTab());
	}
	void setTab(u32 index){
		currentTab = index;
		tabs[currentTab]->enter(*this);
	}
	void run();
	void drawTabs();
	void keyCallback(){}

	// template<typename T>
	// T& get(){
		// return static_pointer_cast<CommandEditorTab> tabs[0];
	// }


	vector<unique_ptr<ITab>> tabs;
	u32 currentTab {0};
	u32 size {200};
};

// template<>
// unique_ptr<CommandEditorTab> TabManager::get<CommandEditorTab>(){
	// return static_pointer_cast<CommandEditorTab> tabs[0];
// }
// template<>
// unique_ptr<PathEditorTab> TabManager::get<PathEditorTab>(){
	// return static_pointer_cast<PathEditorTab> tabs[2];
// }


NAM_END


