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
		tabs[currentTab]->run(*this);
		
	}
	void keyCallback(){}
	
	template<typename T>
	T& get(){
		return static_pointer_cast<CommandEditorTab> tabs[0];
	}
	
	template<>
	unique_ptr<CommandEditorTab> get<CommandEditorTab>(){
		return static_pointer_cast<CommandEditorTab> tabs[0];
	}
	template<>
	unique_ptr<PathEditorTab> get<PathEditorTab>(){
		return static_pointer_cast<PathEditorTab> tabs[2];
	}
	
	vector<unique_ptr<ITab>> tabs;
	u32 currentTab {0};
};

class ITab
{
public:
	virtual ~ITab(){};
	virtual void run(TabManager &tabManager) = 0;
};

enum CommandEditorTabCommandWidget {};

class CommandEditorTab : public ITab
{
public:
	void run(TabManager &tabManager){
		getTypeWidget(tabManager);
		if(commandEditorWidget){
			commandEditorWidget(tabManager);
			doneWidget();
		}
		
	}
	void getTypeWidget(TabManager &tabManager){
		commandChoice.run([this](CommandEditorTabCommandWidget val){ commandEditorWidget = buildWidget(val) });
	}
	void doneWidget(){}
	void setToEdit(shared_ptr<ICommandEditor> &ptr){
		buildWidget(ptr->type);
		commandEditorWidget->command = ptr;
	}
	unique_ptr<ICommandEditorWidget> buildWidget(CommandTypeEnum val){
		switch(val){
			case : return make_unique<>();
			
		};
		
	}
private:
	unique_ptr<ICommandEditorWidget> commandEditorWidget;
};

// trzeba skdas przechwycić RC
class CommandListTab : public ITab
{
public:
	void run(TabManager &TM){
		for(auto &it : RC.commands){
			/// allow edition, rclick przenosi do edytora komend, tylko trzeba pamitać żeby po done nie dodać nowej komendy
			ui.rect(150, 20).text(it->name)()
				.onrClick(TM.get<CommandEditorTab>().setToEdit(it));
		}
		
	}
	
}
