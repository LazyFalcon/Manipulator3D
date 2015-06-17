extern glm::vec2 screenSize;

class IMenuPropertyTab
{
public:
	virtual void run() = 0;
};
class MenuPropertyEditor
{
public:
	void run();
	void init();
	MenuPropertyEditor()
		size(200),
		box(screenSize.x - size, screenSize.y, size, screenSize.y), /// 
		flags(UI::Vertical | UI::AlignRight | UI::AlignTop | UI::FixedSize | UI::Draw),
		active(true),
		currentMenuTab(0)
		{
			initTabs();
		}
	
	
private:
	initTabs();
	void drawTabs();

	std::vector<unique_ptr<IMenuPropertyTab>> menuTabs;
	glm::vec4 box;
	int currentMenuTab;
	int flags;
	float size;
	bool active;
};



void MenuPropertyEditor::initTabs(){
	menuTabs.push_back(make_unique<CommanBuilderTab>());
	menuTabs.push_back(make_unique<CommanListTab>());
	menuTabs.push_back(make_unique<PathListTab>());
	menuTabs.push_back(make_unique<PathEditorTab>());
	menuTabs.push_back(make_unique<PlotTab>());
	
}
