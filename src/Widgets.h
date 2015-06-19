#pragma once
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#define NAM_START {
#define NAM_END }
extern float g_scrollDel;
extern float g_scrollPos;

extern const std::u16string Arrows;

extern const std::u16string ArrowsInCircle;

extern const std::u16string ArrowDown;
extern const std::u16string ArrowUp;
extern const std::u16string ArrowRight;
extern const std::u16string ArrowLeft;

extern const std::u16string ArrowOLeft;
extern const std::u16string ArrowORight;
extern const std::u16string Radio;
extern const std::u16string Check;
extern const std::u16string totalString;



extern const std::u16string RadioF;
extern const std::u16string RadioT;
extern const std::u16string CheckSym[3];

extern UI::IMGUI ui;
extern glm::vec2 mousePosition;
// static float window_width = 1400, window_height = 700;


namespace wxg NAM_START

class Movable {
public:
	glm::vec4 *position;
	glm::vec2 distance;
	bool isMoving = false;
	Movable(glm::vec4 &pos): position(&pos){}
	~Movable(){}
	void move(){
		isMoving = ui.onGroupGrab([this](glm::vec4){
			if(isMoving){
				position->x = mousePosition.x - distance.x;
				position->y = mousePosition.y - distance.y;
			}
			else {
				isMoving = true;
				distance = mousePosition - (*position).xy();
			}
			ui.captureMouse = true;
		});
	 }

};

class RadioButtons {
public:


};
const int buttonS = 6;
class ScrollList {
public:
	int maxItems;
	std::vector<std::string> values;
	int widgetWidth;
	glm::vec4 scrollBar;
	int scrollPos;
	int draw(){
		ui.box(UI::LayoutHorizontal | UI::AlignTop );
		ui.box(UI::LayoutVertical | UI::AlignLeft);
				ui.rect(widgetWidth-buttonS+1, 1)();
				for(int i=scrollPos; i<values.size() && i<maxItems+scrollPos; ++i){
					ui.rect(widgetWidth-buttonS, 22).text(values[i])(UI::Hoverable);
				}
			ui.endBox();
			ui.box(UI::LayoutVertical);
				auto &colors = ui.getStyle();
				int tootalHeigth = 22*values.size();
				int visibleHeigth = 22*maxItems;

				ui.rect(6,visibleHeigth, colors.fancy[2]).getRect(scrollBar)();

				float scrollSize = visibleHeigth*visibleHeigth/tootalHeigth;
				scrollBar.y += (maxItems - scrollPos-1)/(float)(maxItems - 1)* (scrollBar.w - scrollSize);
				scrollBar.w = scrollSize;

				ui.rect(scrollBar, colors.fancy[0])();

			ui.endBox();
				ui.onGroupHover([this](glm::vec4 r){
					scrollPos = glm::clamp(scrollPos-g_scrollDel, 0.f, maxItems-1.f);
					ui.keyAction(GLFW_KEY_HOME, [this]{
							scrollPos = 0;
						});
					ui.keyAction(GLFW_KEY_END, [this]{
							scrollPos = maxItems-1;
						});
				});
		ui.endBox();
	};

};


class DropDownList {
public:
	std::string name;
	std::vector<std::string> values;
	int widgetWidth;
	int currPosition;
	bool dropped;
	int draw(){
	/* 	glm::vec4 listStart;
		ui.box(UI::LayoutHorizontal | UI::AlignLeft );
			ui.rect(widgetWidth/2, 20).text(name)();
			ui.rect(widgetWidth/2, 20).text(values[currPosition]).text(dropped ? u"\ue08d":u"\ue00d","sym_12", TextToRight).getRect(listStart)(UI::Hoverable).button(dropped);
		ui.endBox();

		if(dropped){
			ui.beginLayer();
			ui.box(listStart, UI::LayoutVertical | UI::AlignLeft | UI::FixedPos | UI::NewLayer | UI::Draw);
			for(int i=0; i<values.size(); i++)
				// ui.rect(listStart-glm::vec4(0,20*i+20,0,0)).text(values[i])(false)
				ui.rect(widgetWidth/2, 20).text(values[i])(UI::Hoverable)
					.switcher(currPosition, i)
					.button(dropped);
				if(ui.outOfTable())
					dropped = false;
			ui.endBox();
			ui.endLayer();
		} */
	}

};

template<typename EnumType>
class DropDownEnumList
{
public:
	int run();


};


// :P
class Minimizable {
public:
	Minimizable(glm::vec4 &a1) :
		maximizedPosition(a1), minimizedPosition(0,500,0,0), minimized(false)
		{}
	glm::vec4 maximizedPosition;
	glm::vec4 minimizedPosition;
	bool minimized;
	bool draw(){

		ui.rect(
				minimized ? minimizedPosition.x : maximizedPosition.x+200-24,
				minimized ? minimizedPosition.y : maximizedPosition.y-24,24,24)
			.text(minimized ? ArrowLeft:ArrowRight, "ui_17")()
			.button(minimized);
		return minimized;
	}
	void button(int x, int y){
		ui.rect(x,y).button(minimized)();
	}

};
class IMinimizable {
public:
	IMinimizable(glm::vec4 *a1, glm::vec4 a2, const std::string &name, int len) :
		maximizedPosition(a1), minimizedPosition(a2), name(name), length(len)
	{}
	glm::vec4 *maximizedPosition;
	glm::vec4 minimizedPosition;
	std::string name;
	int length;
	bool minimized;
	virtual bool draw() = 0;

};
class MinimizableRight : public IMinimizable {
public:
	MinimizableRight(glm::vec4 *a1, glm::vec4 a2, const std::string &name, int len)
	: IMinimizable(a1,a2,name,len){}
	bool draw(){
		// ui.rect(
				// minimized ? minimizedPosition.x : maximizedPosition->x+length-24,
				// minimized ? minimizedPosition.y : maximizedPosition->y-24,24,24)
			// .text(minimized ? ArrowLeft:ArrowRight, "ui_17")()
			// .onlClick([this]{
				// minimized = !minimized;
			// });
		return minimized;
	}
};
class MinimizableLeft : public IMinimizable {
public:
	MinimizableLeft(glm::vec4 *a1, glm::vec4 a2, const std::string &name, int len)
	: IMinimizable(a1,a2,name,len){}
	bool draw(){
		// ui.rect(
				// minimized ? minimizedPosition.x : maximizedPosition->x+length-24,
				// minimized ? minimizedPosition.y : maximizedPosition->y-24,24,24)
			// .text(minimized ? ArrowRight:ArrowLeft, "ui_17")()
			// .onlClick([this]{
				// minimized = !minimized;
			// });
		return minimized;
	}
};
class MinimizableDown : public IMinimizable {
public:
	MinimizableDown(glm::vec4 *a1, glm::vec4 a2, const std::string &name, int len)
	: IMinimizable(a1,a2,name,len){}
	bool draw(){
		// ui.rect(
				// minimized ? minimizedPosition.x : maximizedPosition->x+length-24,
				// minimized ? minimizedPosition.y : maximizedPosition->y-24,24,24)
			// .text(minimized ? ArrowUp:ArrowDown, "ui_17")()
			// .onlClick([this]{
				// minimized = !minimized;
			// });
		return minimized;
	}
};

void editBox(const std::string &text, float &value, int len);

/// po window dziedziczymy, sorry
class Window
{
public:
	void run();
	virtual void mainBody() = 0;
	bool beginWindow();
	void endWindow();
	virtual ~Window(){}
	Window(const std::string &_title, const glm::vec4 &_position, float size) :
		position(_position),
		minimizable(position),
		movable(position),
		title(_title),
		size(size)
		{}

private:
	Minimizable minimizable;
	Movable movable;
	glm::vec4 position;
	float size;
	std::string title;
	UI::StyleID style;
};


NAM_END
