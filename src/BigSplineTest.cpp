
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "Engine-DrawUtils.h"
#include "BigSplineTest.h"
#include "IInterpolator.h"
#include "Widgets.h"
#include "Editor/Editor.h"
#include "Graph.h"

extern UI::IMGUI ui;
extern glm::vec2 mousePosition;
extern std::list<Plot*> plotList;
Graph mousePositionGraphX("mousePosX", Graph::LastX, 0xFFFF00FF, 250);
Graph mousePositionGraphY("mousePosY", Graph::LastX, 0x00FFFFFF, 250);
Plot mainPlot("Main plot", {1000,5,395,250}, 0x10141280);

/**
 *  Ok, na początek zróbmy wyświetlanie tego co jest w płaszczyźnie XY
 */

namespace Editor
{
	extern PolylineEditor polylineEditor;
}

namespace BigSplineTest NAM_START
const std::u16string RadioF = u"\u25cb";
const std::u16string RadioT = u"\u25c9";

static float window_width = 1400, window_height = 700;

vector<shared_ptr<IInterpolator>> interpolators;
vector<shared_ptr<IInterpolator>>::iterator iit;
vector<shared_ptr<IInterpolator>>::iterator itToEdit;

class BigSplineWidget
{
	UI::StyleID style;
	glm::vec4 boxPosition;
	wxg::Movable movable;
	wxg::MinimizableRight minimizable;
	float lenght = 130;
public:

	BigSplineWidget():
		style(UI::StyleID::Basic),
		boxPosition(0, 250, 0, 0),
		movable(boxPosition),
		minimizable(&boxPosition, glm::vec4(window_width-25, window_height-lenght+30,0,0), "BigSplineWidget"s, lenght)
		{}

	void run();
	void doTitle(){
		ui.box(UI::LayoutHorizontal | UI::Draw).style(UI::StyleID::Label);
			ui.rect(24,24)(UI::Hoverable);
			ui.rect(lenght-24,24).text("BigSplineWidget")();
		ui.endBox();
	}
	void doRadioButtons();
	void doEditBox();


};

void BigSplineWidget::run(){
	// if(minimizable.draw())
		// return;

	// ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw | UI::FixedPos)
	ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::FixedPos)
		.position(boxPosition)
		// .size(lenght,0)
		.style(style);

	doTitle();
	doRadioButtons();

	movable.move();
	ui.endTable();
}
void BigSplineWidget::doRadioButtons(){
	for(auto it=interpolators.begin(); it!=interpolators.end(); it++){
		ui.box(UI::LayoutHorizontal).size(lenght, 2);
			ui.rect(100,22).text(InterpolatorTranslate((*it)->type))(UI::CaptureMouse);
			ui.box(UI::LayoutHorizontal).size(50, 22).offset(-50, 0);
				ui.rect(20,22).text(it == iit ? RadioT : RadioF, "ui_17")(UI::CaptureMouse)
					.onlClick([&it]{ iit = it;});
				ui.rect(20,22).color(it == iit ? colorRed : colorWhite80).text(u"\uE15E", "ui_17")(UI::CaptureMouse)
					.onlClick([&it]{
						iit = it;
						itToEdit = it;
						Editor::polylineEditor.set(*it);
						});
			ui.endBox();
		ui.endBox();
	}

}
void BigSplineWidget::doEditBox(){
	if(itToEdit == interpolators.end())
		return;


}

/**
 *  To już na poważnie, okno do edycji krzywych
 *  obejmuje również tworzenie(ale później)
 */
class InterpolatorEditBoxWidget
{
	UI::StyleID style;
	glm::vec4 boxPosition;
	wxg::Movable movable;
	wxg::MinimizableRight minimizable;
	float lenght = 130;
public:

	InterpolatorEditBoxWidget():
		style(UI::StyleID::Basic),
		boxPosition(0, 500, 0, 0),
		movable(boxPosition),
		minimizable(&boxPosition, glm::vec4(window_width-25, window_height-lenght+30,0,0), "InterpolatorEditBoxWidget"s, lenght)
		{}

	void run();
};

void InterpolatorEditBoxWidget::run(){}


BigSplineWidget bigSplineWidget;


/**
 *  Utworzenie wszystkich możliwych interpolatorów, przygortowanie edytora
 *  Od razu przypiac interpolators do ich edytorów, i operować na nich
 */
void init(){
	vector<glm::vec4> set_1{
    {-3.1567 ,  -8.3333, 0, 1},
    {-1.2673,   -2.5439, 0, 1},
    {-7.1659,   -0.1462, 0, 1},
    {-2.4194,    7.5731, 0, 1},
    { 2.6498,    1.3158, 0, 1},
    { 7.1659,    1.2573, 0, 1},
    { 7.1659,   -7.7485, 0, 1},
    { 1.8203,   -8.8012, 0, 1}
	};
	vector<glm::vec4> set_2{
			{1.3594 ,  -5.7018, 0, 1},
			{-6.0138,  -4.0058, 0, 1},
			{-6.9816,   3.7135, 0, 1},
			{0.3917 ,   6.7544, 0, 1},
			{5.4147 ,   3.0702, 0, 1},
			{6.7512 ,  -3.4795, 0, 1},
			{-1.8664,   1.6082, 0, 1},
			{7.3041 ,   5.8187, 0, 1},
			{0.1613 ,   0.1462, 0, 1},
			{9.6544 ,  -9.7368, 0, 1}
	};
	InterpolatorFactory iFactory;
	// interpolators.push_back(iFactory.build(Interpolator::Linear, set_1));
	// interpolators.push_back(iFactory.build(Interpolator::BezierCurve, set_1));
	// interpolators.push_back(iFactory.build(Interpolator::BSpline, set_1));
	// interpolators.push_back(iFactory.build(Interpolator::HermiteCardinal, set_1));
	// interpolators.push_back(iFactory.build(Interpolator::HermiteFiniteDifference, set_1));
	// interpolators.push_back(iFactory.build(Interpolator::HermiteFiniteDifferenceClosed, set_1));

	// interpolators.push_back(iFactory.build(Interpolator::Linear, set_2));
	// interpolators.push_back(iFactory.build(Interpolator::BSpline, set_2));
	// interpolators.push_back(iFactory.build(Interpolator::HermiteCardinal, set_2));
	// interpolators.push_back(iFactory.build(Interpolator::HermiteFiniteDifference, set_2));
	// interpolators.push_back(iFactory.build(Interpolator::HermiteFiniteDifferenceClosed, set_2));

	iit = interpolators.begin();
	itToEdit = interpolators.end();
	Editor::init();
	mousePositionGraphX.setBouds({0,250,0,1400});
	mousePositionGraphY.setBouds({0,250,0,700});
	// mainPlot.push(&mousePositionGraphX);
	// mainPlot.push(&mousePositionGraphY);
	plotList.push_front(&mainPlot);
}

/**
 *  Obsługa inputu, menusów, edytorów, też apdejt testowego gówienka które popierdala po krzwej
 */
void update(float dt){
	bigSplineWidget.run();
	ui.rect(window_width/2-50, window_height-30, 100, 30).color(colorOrange).text(InterpolatorTranslate((*iit)->type), "ui_32", UI::CenterText)();
	Editor::update();
	mousePositionGraphX.push(mousePosition.x);
	mousePositionGraphY.push(mousePosition.y);
}
void draw(){

	Engine::drawLineStrip((*iit)->visualisation, colorYellow, 2);
	Engine::drawLineStrip((*iit)->points, colorOrange, 1);

}

void next(){
	iit++;
	if(iit == interpolators.end())
		iit = interpolators.begin();
}
void prev(){
	if(iit == interpolators.begin())
		iit = interpolators.end();
	iit--;
}
void run(){}
void pause(){}
void stop(){}









NAM_END
