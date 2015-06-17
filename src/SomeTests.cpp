
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
static float window_width = 1400, window_height = 700;
#include "Widgets.h"
#include "SomeTests.h"
extern UI::IMGUI ui;
extern glm::vec2 mousePosition;

const std::u16string Arrows = u"Arrows: \ue016\ue017\ue018\ue019\n";

const std::u16string ArrowsInCircle = u"ArrowsInCircle: \ue095\ue0c0\n";

const std::u16string ArrowDown = u"\ue019";
const std::u16string ArrowUp = u"\ue018";
const std::u16string ArrowRight = u"\ue017";
const std::u16string ArrowLeft = u"\ue016";

const std::u16string ArrowOLeft = u"\ue0c0";
const std::u16string ArrowORight = u"\ue095";
const std::u16string Radio = u"Radio: \ue0e6\ue0a8\n";
const std::u16string Check = u"Check: \ue002\ue005\ue0a4\n";
const std::u16string totalString = Arrows+ArrowsInCircle+Radio+Check;

// const std::u16string RadioF = u"\ue0e6";
// const std::u16string RadioT = u"\ue0e6\ue0a8";
const std::u16string RadioF = u"\u25cb";
const std::u16string RadioT = u"\u25c9";
const std::u16string CheckSym[] = {u"\u2610", u"\u2611", u"\u2612"};


std::vector<std::pair<std::u16string, bool>> radioValues {
	std::pair<std::u16string, bool>(u"First", false),
	std::pair<std::u16string, bool>(u"Second", false),
	std::pair<std::u16string, bool>(u"Third", false)
};

enum CheckValues {
	Empty=0, Good=1, Bad=2
};
std::vector<std::pair<std::u16string, uint8_t>> checkValues {
	std::pair<std::u16string, bool>(u"First", 0),
	std::pair<std::u16string, bool>(u"Second", 0),
	std::pair<std::u16string, bool>(u"Third", 0)
};
/////////////////////////////////////
class FloatingWindow {
public:
	UI::StyleID style;
	glm::vec4 boxPosition;
	wxg::Movable movable;
	wxg::MinimizableRight minimizable;
	wxg::DropDownList list;
	wxg::ScrollList scrollList;
	std::string dupa;

	FloatingWindow(UI::StyleID style) : style(style), movable(boxPosition), boxPosition(200, 500, 0, 0),
		minimizable(&boxPosition, glm::vec4(window_width-25, window_height-170,0,0), "PathCreator"s, 200)
		{
		list = wxg::DropDownList{"CurveType"s, {"BSpline"s, "NURBS"s, "Hermite"s}, 200};
		scrollList = wxg::ScrollList {5, {"Firebal"s, "Lighting"s, "Slag"s, "Kaboom"s, "Igyfytru"s, "Rrrroar!"s, "Lorem Ipsum"s, "Rrrroar!"s, "Lorem Ipsum"s}, 200};
	}

	void drawWidget(){
		if(minimizable.draw())
			return;
		ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw)
			.position(boxPosition)
			.size(200,0)
			.style(style);
			ui.box(UI::LayoutHorizontal | UI::Draw).style(UI::StyleID::Label);
				ui.rect(24,24)(UI::Hoverable);
				ui.rect(200-24,24).text("Sample Widget")();
			ui.endBox();
			ui.box(UI::LayoutVertical | UI::Draw);
			list.draw();
			radioButton();
			checkButton();
			ui.box(UI::LayoutHorizontal | UI::AlignLeft);
				ui.rect(100, 22).text("dupa: ")();
				ui.rect(100, 22).edit(dupa)(UI::EditBox);
			ui.endBox();
			scrollList.draw();
		ui.endBox();
		movable.move();
		ui.endTable();

	}
	void radioButton(){
		ui.box(UI::LayoutVertical).style(style);
		for(auto &it : radioValues){
			ui.box(UI::LayoutHorizontal).size(200, 2);
			ui.rect(100,22).text(it.first, "ui_12")(UI::Button);
			ui.rect(20,22).offset(-25,0).text(it.second ? RadioT : RadioF, "ui_17")()
				.onlClick([&it]{
					for(auto &rad : radioValues)
						rad.second = false;
					it.second = true;
				});

			ui.endBox();
		}
		ui.endBox();
}
	void checkButton(){
		ui.box(UI::LayoutVertical).style(style);
		for(auto &it : checkValues){
			ui.box(UI::LayoutHorizontal).size(200, 2);
			ui.rect(100,22).text(it.first, "ui_12")(UI::Button);
			ui.rect(20,22).offset(-25,0).text(CheckSym[it.second], "ui_17")()
				.onlClick([&it]{
					it.second = (it.second+1)%3;
				});

			ui.endBox();
		}
		ui.endBox();
	}

};

FloatingWindow floatingWindowB(UI::StyleID::Basic);



void loremIpsum(){
	// ui.rect(200, 70, 600, 20).text("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.", "ui_17")();
	// ui.rect(200, 50, 600, 20).text("Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.", "ui_17")();
	// ui.rect(200, 10, 600, 40).text("Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu \nfugiat nulla pariatur..", "ui_17")();
}



void SomeTests(){
	loremIpsum();

	floatingWindowB.drawWidget();

}