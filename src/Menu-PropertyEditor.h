extern glm::vec2 screenSize;

class IMenuPropertyTab
{
public:
	virtual void run() = 0;

	~IMenuPropertyTab(){}
	IMenuPropertyTab(const std::string &icon) :
		icon(icon)
		{}

	const std::string icon;
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

void MenuPropertyEditor::run(){
	// if(minimized) return;
	ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft)
		.position(glm::vec4(window_width/2-240, window_height-50,480,-600))
		.size(glm::vec4(window_width/2-240, window_height-50,480,-600));

	drawTabs();
	menuTabs[currentMenuTab].run();

	ui.endTable();
}

void MenuPropertyEditor::drawTabs(){
	ui.box(UI::LayoutHorizontal);

	for(int i=0; i<menuTabs.size(); i++){
		glm::vec4 rect;
		ui.image(currentMenuTab==i ? "inactiveTab" : "activeTab")
			.switcher(currentMenuTab, i)
			.getRect(rect)
			(UI::Button);

		ui.image(rect.x, rect.y, menuTabs[i].icon)();
	}

	ui.endBox();
}


