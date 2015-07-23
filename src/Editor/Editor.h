#pragma once
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <set>
#include <Utils/IMGUI_V4.h>
#include <Utils/Camera.h>
#include "PropertyEditor-TabManager.h"
#include "../Widgets.h"
#include "../IInterpolator.h"
#include "../RobotController.h"
extern Camera camera;
extern glm::vec4 CameraUp;
extern glm::vec4 CameraRight;
extern glm::vec4 CameraNormal;
extern float window_width, window_height;
extern UI::IMGUI ui;

namespace Editor NAM_START
// static float pointSize = 16.f;
// #define NAM_START {
// #define NAM_END }

extern unique_ptr<TabManager> menuSideBar;

namespace EditorFlags
{
enum EditorMode : uint32_t
{
	GlobalSpace     = 0b1,
	ViewSpace       = 0b10,
	SnapToGrid      = 0b100, /// tyczy się myszy
	SnapToAxes      = 0b1000,
	MultiSelection  = 0b10000,
	PinSelectedToMouse  = 0b100000,
	Slide = 0b1000000,
};
enum SnapMode : uint32_t
{
	SnapToNone = 0,
	SnapToGlobal,
	SnapToNormal,
	SnapToCamera,
};

const uint32_t SnapModes[] = {
	SnapToNone,
	SnapToGlobal,
	SnapToNormal,
	SnapToCamera
};
const std::string SnapModeNames[] = {
	"SnapToNone",
	"SnapToGlobal",
	"SnapToNormal",
	"SnapToCamera"
};

}

namespace EditorAxes
{
	extern const glm::vec4 globalAxes[];
	extern glm::vec4 normalAxes[];
	extern const glm::vec4 *cameraAxes[];
}

class MultiPointController
{
public:
	std::set<glm::vec4*> points;
	std::set<glm::vec4> pointsOriginal;
	glm::vec2 mouseOffset {};
	glm::vec4 center;
	int preferedDirection = {0};

	MultiPointController() : center(0,0,0,1){}

	void setOffset(const glm::vec2 &mousePos){
		mouseOffset = mousePos;
	}
	glm::vec4 move(uint32_t snapFlags){
		if(snapFlags == EditorFlags::SnapToGlobal) moveAlongGlobalAxes();
		else if(snapFlags == EditorFlags::SnapToNormal) moveAlongNormalAxes();
		else if(snapFlags == EditorFlags::SnapToCamera) moveAlongCameraAxes();
		return glm::vec4(0);
	}
	glm::vec4 move();
	glm::vec4 slide();
	glm::vec4 moveAlongGlobalAxes();
	glm::vec4 moveAlongNormalAxes();
	glm::vec4 moveAlongCameraAxes();
	void clear(){
		preferedDirection = 0;
		center = glm::vec4(0,0,0,1);
		points.clear();
	}
	void push(glm::vec4 &node){
		if(points.empty())
			center = glm::vec4(0,0,0,1);
		if(points.insert(&node).second){ /// new point
			// pointsOriginal.push_back(node);
			center = (center*float(points.size()-1) + node)/float(points.size());
			center.w = 1;
		}
		else { /// remove point
			points.erase(&node);
			center = (center*float(points.size()+1) - node)/float(points.size());
		}
	}
	void applyTransform(const glm::vec4 &transform){
			for(auto &it : points)
					*it += transform;
	}
	void prepareToDelete(){
			for(auto &it : points)
			(*it).w = -1;
	}

};

class MainEditorWidget
{
public:
	glm::vec4 boxPosition;
	UI::StyleID style;
	std::string title;
	float lenght = 130;
	wxg::Movable movable;
	wxg::MinimizableRight minimizable;
	virtual ~MainEditorWidget(){}
	MainEditorWidget(glm::vec4 _boxPosition, float lenght, float height, std::string title):
		boxPosition(_boxPosition),
		style(UI::StyleID::Dark),
		title(title),
		lenght(lenght),
		movable(_boxPosition),
		minimizable(&boxPosition, glm::vec4(window_width-25, height,0,0), title, lenght)
		{}

	void run(){
		beginwidget();
		doTitle();
		mainBody();

		endWidget();
	}
	virtual void mainBody() = 0;
	void beginwidget(){
		// if(minimizable.draw())
			// return;

		// ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw)
		ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::FixedPos)
			.position(boxPosition)
			// .size(lenght,0)
			.style(style);
	}
	void endWidget(){
		movable.move();
		ui.endTable();
	}
	void doTitle(){
		ui.box(UI::LayoutHorizontal | UI::Draw).style(UI::StyleID::Label);
			ui.rect(24,24)(UI::Hoverable);
			ui.rect(lenght-24,24).text(title)();
		ui.endBox();
	}


};

void set(shared_ptr<IInterpolator> &p);

/**
 *  Okołoglobalne jestestwo do edycji polylini, nic wiecej, niech działa zawsze bądź gdy jest enabled
 */
class PolylineEditor : public MainEditorWidget
{
public:
	shared_ptr<IInterpolator> polyline;
	MultiPointController markedNodes;
	uint32_t editorState;
	uint32_t editorSnapMode;
	bool snapModeDropped;

	PolylineEditor() :
		MainEditorWidget(glm::vec4(0,600,0,0), 150.f, 150.f, "Line"),
        editorState(0),
        editorSnapMode(EditorFlags::SnapToGlobal),
				snapModeDropped(false)
		{}

	void set(shared_ptr<IInterpolator> &p){
		polyline = p;
		// nodes.clear();
		// for(auto &point : polyline->points)
			// nodes.emplace_back(point);
	}
	void clear(){
		polyline.reset();
	}
	void mainBody();
	void snapModeDropDown();
	glm::vec2 project(const glm::vec4 &point){
        glm::vec3 screenPos = glm::project(point.xyz(), camera.ViewMatrix, camera.ProjectionMatrix, glm::vec4(0,0,camera.m_width, camera.m_height));
        return glm::vec2(screenPos.x, screenPos.y);
    }
	void processAll();
	void processPoints();
	void processControls();

	void extrude();
	void slide();
	void subdivide();
	void cleanUpNodes();
	void removeSelected();
	void undo();
	void mergePoints();
	void processMouse(int key, int action, int modifier);
	void processKeys(int key, int action, int modifier);
};

extern PolylineEditor polylineEditor;

void init();
void update(RobotController &RC);
void processKeys(int key, int action, int modifier, RobotController &RC);
void processMouse(int key, int action, int modifier);

NAM_END

