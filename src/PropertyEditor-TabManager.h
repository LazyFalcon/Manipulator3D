class ITab;
class CommandEditorTab;
class CommandListTab;
class PathEditorTab;
class PathListTab;
class TabManager
{
	void clearTab(u32 index){
		tabs[index].reset();
		initTabs();
	}
	void initTabs(){
		if(tabs.size() < 4){
			tabs.resize(4);
		}
		else {
			if(!tabs[0]) tabs[0] = make_uniqe<CommandEditorTab>();
			if(!tabs[1]) tabs[1] = make_uniqe<CommandListTab>();
			if(!tabs[2]) tabs[2] = make_uniqe<PathEditorTab>();
			if(!tabs[3]) tabs[3] = make_uniqe<PathListTab>();
		}
	}
	void setTab(u32 index){
		currentTab = index;
	}
	void run(){
		tabs[currentTab]->run();
		
	}
	void keyCallback(){}

	vector<ITab> tabs;
	u32 currentTab {0};
};

class ITab
{
public:
	virtual ~ITab(){};
	virtual void run() = 0;
};

class CommandEditorTab : public ITab
{
public:
	void run(){
		getTypeWidget();
		if(commandEditorWidget){
			commandEditorWidget();
			doneWidget();
		}
		
	}
	void getTypeWidget(){
		commandChoice.run([this](..commandEnum..){  });
	}
	void doneWidget(){}
	
	
	unique_ptr<ICommandEditorWidget> commandEditorWidget;
};

