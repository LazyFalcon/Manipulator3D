﻿#define _DebugLine_  std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";
#include "Editor.h"
#include "../Helper.h"

extern glm::vec2 screenSize;
extern glm::vec2 mousePosition;
extern glm::vec2 mouseTranslation;
extern glm::vec2 mouseMoveVector;
extern float mouseMoveLen;

extern UI::IMGUI ui;
namespace UI {
extern int g_UILayer;
};

/// ---------------------------------------------
namespace Editor NAM_START

u32 EditorMode;
enum EditorModes : u32 {
	Enabled = 0b1,
	NewCommand = 0b10,
	EditCommand = 0b100,
	NewPath = 0b1000,
	EditPath = 0b10000,

	DefaultMode = EditCommand | EditPath,

};

unique_ptr<TabManager> tabBar; /// tabbar też raczej będzie żył przez cały program, ew do pełnego przeładowania, ale to nie problem

namespace EditorAxes
{
	const glm::vec4 globalAxes[] = {
		glm::vec4(1,0,0,0),
		glm::vec4(0,1,0,0),
		glm::vec4(0,0,1,0)
	};
	glm::vec4 normalAxes[] = {
		glm::vec4(1,0,0,0),
		glm::vec4(0,1,0,0),
		glm::vec4(0,0,1,0)
	};
	const glm::vec4 *cameraAxes[] = {
		&CameraUp,
		&CameraRight,
		&CameraNormal
	};
}

glm::vec4 MultiPointController::move(){
	auto oldCenter = center;
	center = camera.eyePlaneIntersection(camera.surfaceNormalToCamera(center), -mouseOffset);

	auto delta = center - oldCenter;
	for(auto it : points)
		*it += delta;

	return delta;
}
glm::vec4 MultiPointController::slide(){
	auto newCenter = camera.eyePlaneIntersection(camera.surfaceNormalToCamera(center), -mouseOffset);

	auto delta = newCenter - center;

	glm::vec4 *o = *(points.begin());

	if(preferedDirection == 0){
		auto n1 = glm::normalize(*(o+1) - *o);
		auto n2 = glm::normalize(*(o-1) - *o);

		auto d1 = glm::dot(glm::normalize(delta), n1);
		auto d2 = glm::dot(glm::normalize(delta), n2);

		preferedDirection = d1 > d2 ? 1 : -1;
	}

	auto normal = glm::normalize(*(o + preferedDirection) - *o);
	auto dot = glm::dot(glm::normalize(delta), normal);

	glm::vec4 finalSlide = dot * glm::length(delta) * normal;

	center += finalSlide;
	*o += finalSlide;

	return finalSlide;
}
glm::vec4 MultiPointController::moveAlongGlobalAxes(){
	using namespace EditorAxes;

	auto newCenter = camera.eyePlaneIntersection(camera.surfaceNormalToCamera(center), -mouseOffset);

	auto delta = glm::clamp(newCenter - center, -1.f, 1.f);
	float deltaLen = glm::length(delta);
	delta /= deltaLen;

	if(preferedDirection == 0){
		float d1 = abs(glm::dot(delta, globalAxes[0]));
		float d2 = abs(glm::dot(delta, globalAxes[1]));
		float d3 = abs(glm::dot(delta, globalAxes[2]));
		if(d1 == std::max({d1,d2,d3})) preferedDirection = 1;
		else if(d2 == std::max({d1,d2,d3})) preferedDirection = 2;
		else if(d3 == std::max({d1,d2,d3})) preferedDirection = 3;
	}
	const glm::vec4 &axis = globalAxes[preferedDirection-1];
	const float dot = glm::dot(delta, axis);

	glm::vec4 finalSlide = dot * deltaLen * axis;
	center += finalSlide;

	for(auto it : points)
		*it += finalSlide;

	return finalSlide;
}
glm::vec4 MultiPointController::moveAlongNormalAxes(){
	using namespace EditorAxes;

	auto newCenter = camera.eyePlaneIntersection(camera.surfaceNormalToCamera(center), -mouseOffset);

	auto delta = newCenter - center;
	float deltaLen = glm::length(delta);
	delta /= deltaLen;

	glm::vec4 *o = *(points.begin());
	if(preferedDirection == 0){

		normalAxes[0] = glm::normalize(*(o+1) - *o);
		normalAxes[1] = glm::normalize(*(o-1) - *o);
		normalAxes[2] = glm::vec4(glm::cross(normalAxes[0].xyz(), normalAxes[1].xyz()),0);

		float d1 = abs(glm::dot(delta, normalAxes[0]));
		float d2 = abs(glm::dot(delta, normalAxes[1]));
		float d3 = abs(glm::dot(delta, normalAxes[2]));
		if(d1 == std::max({d1,d2,d3})) preferedDirection = 1;
		else if(d2 == std::max({d1,d2,d3})) preferedDirection = 2;
		else if(d3 == std::max({d1,d2,d3})) preferedDirection = 3;
	}
	const glm::vec4 &axis = normalAxes[preferedDirection-1];
	const float dot = glm::dot(delta, axis);

	glm::vec4 finalSlide = dot * deltaLen * axis;
	center += finalSlide;

	for(auto it : points)
		*it += finalSlide;

	return finalSlide;
}
glm::vec4 MultiPointController::moveAlongCameraAxes(){
	using namespace EditorAxes;

	auto newCenter = camera.eyePlaneIntersection(camera.surfaceNormalToCamera(center), -mouseOffset);

	auto delta = newCenter - center;
	float deltaLen = glm::length(delta);
	delta /= deltaLen;


	if(preferedDirection == 0){
		float d1 = abs(glm::dot(delta, *cameraAxes[0]));
		float d2 = abs(glm::dot(delta, *cameraAxes[1]));
		if(d1 == std::max({d1,d2})) preferedDirection = 1;
		else if(d2 == std::max({d1,d2})) preferedDirection = 2;
	}
	const glm::vec4 &axis = *cameraAxes[preferedDirection-1];
	const float dot = glm::dot(delta, axis);

	glm::vec4 finalSlide = dot * deltaLen * axis;
	center += finalSlide;

	for(auto it : points)
		*it += finalSlide;

	return finalSlide;
}

unique_ptr<PolylineEditor> polylineEditor;

void set(shared_ptr<ICommand> &command){
	if(command->type == MOVE && tabBar){
		tabBar->get<PathEditorTab>().reset(static_pointer_cast<MoveCommand>(command)->interpolator);
		polylineEditor->set(static_pointer_cast<MoveCommand>(command)->interpolator);
	}
}
void set(shared_ptr<IInterpolator> &p){
	// polylineEditor->set(p);
}

void init(){
	polylineEditor = make_unique<PolylineEditor>();
	tabBar = make_unique<TabManager>();
	tabBar->initTabs();
	EditorMode |= EditCommand;
}
void clear(){
	tabBar.reset(nullptr);
	polylineEditor.reset(nullptr);
}
/**
 *  Inicjalizacja TabBar, przypisanie interpolatora i komendy do edycji(w // tabBarze)
 *  polyEditor jest ustawiany chyba przez taba,
 *
 *  jeśli EditCommand == true
 *  	to ustawiana jest aktualna komenda do edycji oraz jej interpolator
 *  jeśli nie
 *  	to tworzona jest nowa
 *
 */
void EnterEditor(RobotController &RC){
	if(EditorMode & EditCommand){
		auto &command = RC.getCommand();
		tabBar->get<CommandEditorTab>().reset(command);
		set(command);
	}
	else {
		tabBar->get<CommandEditorTab>().reset();
		tabBar->get<PathEditorTab>().reset();
	}
}
void ExitEditor(RobotController &RC){
		polylineEditor->reset();
		tabBar->get<CommandEditorTab>().reset();
		tabBar->get<PathEditorTab>().reset();
}

void update(RobotController &RC){
	if(EditorMode & Enabled){
		polylineEditor->run();
		polylineEditor->processAll();
		tabBar->run();
	}
}

void processMouse(int button, int action, int modifier){
	polylineEditor->processMouse(button, action, modifier);
}
void processKeys(int key, int action, int modifier, RobotController &RC){
	polylineEditor->processKeys(key, action, modifier);
	if(action == GLFW_PRESS){
		if(EditorMode & Enabled){

			if(key == GLFW_KEY_TAB){
				EditorMode &= ~Enabled;
				ExitEditor(RC);
			}
		}
		else {
			if(key == GLFW_KEY_TAB){
				EditorMode |= Enabled;
				EnterEditor(RC);

			}
		}
	}
}
PolylineEditor& getPolyline(){
	return *polylineEditor;
}

/// ------- POLYLINE EDITOR ---------------------------------------------

void PolylineEditor::mainBody(){
	if(not polyline) return;
	ui.image("Refresh").onlClick([this]{polyline->generatePath();})();
	snapModeDropDown();
}

void PolylineEditor::snapModeDropDown(){
	glm::vec4 listStart;
	ui.box(UI::LayoutHorizontal | UI::AlignLeft );
		// ui.rect(lenght/2, 20).text(name)();
		ui.rect((lenght*4)/5, 20)
			.text(EditorFlags::SnapModeNames[editorSnapMode])
			.text(snapModeDropped ? u"\ue08d":u"\ue00d","sym_12", UI::TextToRight)
			.getRect(listStart)
			(UI::Hoverable)
			.button(snapModeDropped);
	ui.endBox();

	if(snapModeDropped){
        ui.beginLayer();
        ui.box(UI::LayoutVertical | UI::AlignLeft | UI::FixedPos | UI::NewLayer | UI::Draw);

        for(int i=0; i<4; i++){
            ui.rect((lenght*4)/5, 20)
                .text(EditorFlags::SnapModeNames[i])
                (UI::Hoverable)
                .switcher(editorSnapMode, EditorFlags::SnapModes[i])
                .button(snapModeDropped);
        }
        if(ui.lClick() && ui.outOfTable())
            snapModeDropped = false;

        ui.endBox();
        ui.endLayer();
	}
}

void PolylineEditor::processAll(){
	if(not polyline) return;
	if(ui.lClick() && (editorState & EditorFlags::PinSelectedToMouse)){
		editorState &= ~EditorFlags::PinSelectedToMouse;
	}
	if(ui.lClick() && (editorState & EditorFlags::Slide)){
		editorState &= ~EditorFlags::Slide;
		markedNodes.preferedDirection = 0;
	}
	processPoints();
    for(auto it : markedNodes.points){
		auto r = project(*it);
		ui.image(r.x-2.f, r.y-2.f, "Point")
			.color(0xFFFFFFFF)
			();
	}
	processControls();
}
void PolylineEditor::processPoints(){
	for(auto &node : polyline->points){
		auto r = project(node);
		ui.image(r.x-2.f, r.y-2.f, "Point")
			.circleShape({r.x, r.y, 30, 0})
			.color(0x656060FF)
			.onrClick([this, &node, &r]{
				if(editorState & EditorFlags::MultiSelection){
					markedNodes.push(node);
				}
				else {
					markedNodes.points.clear();
					markedNodes.push(node);
					markedNodes.setOffset(mousePosition - r);
				}
			})
			// (UI::CaptureMouse);
			();
	}
}
void PolylineEditor::processControls(){
	if(markedNodes.points.size() == 0)
		return;
	ui.mouseOverButton = editorState & EditorFlags::PinSelectedToMouse;

	if(editorState & EditorFlags::Slide and editorState & EditorFlags::PinSelectedToMouse){
		markedNodes.slide();
		polyline->generatePath();
	}
	else if(editorState & EditorFlags::PinSelectedToMouse){
		if(editorState & EditorFlags::SnapToAxes)
			markedNodes.move(editorSnapMode);
		else
			markedNodes.move();
		polyline->generatePath();
	}
	else {
		auto r = project(markedNodes.center);
		ui.image(r.x-15.f, r.y-15.f, "Circle30")
			.circleShape({r.x, r.y, 30, 0})
			.onrClick([this, &r]{
				editorState |= EditorFlags::PinSelectedToMouse;
				if(editorState & EditorFlags::MultiSelection)
					markedNodes.setOffset(mousePosition - r);
				else
					markedNodes.setOffset(mousePosition - r);
			})
			(UI::CaptureMouse);
	}
}

void PolylineEditor::insertAtEnd(glm::vec4 p){
	polyline->points.push_back(p);
}
void PolylineEditor::extrude(){
    polylineEditor->save();
	if(markedNodes.points.size() != 1) return;
	for(u32 i=0; i<polyline->points.size(); ++i){
		if(&polyline->points[i] == *markedNodes.points.begin()){
			markedNodes.clear();
			auto inserted = polyline->points.insert(polyline->points.begin()+i+1, polyline->points[i]);
			markedNodes.push(*inserted);
		}
	}

	auto r = project(markedNodes.center);
	markedNodes.setOffset(mousePosition - r);
}
void PolylineEditor::slide(){
    polylineEditor->save();
	if(markedNodes.points.empty()) return;

	editorState |= EditorFlags::PinSelectedToMouse; /// removed on rClick on empty or esc
	editorState |= EditorFlags::Slide; /// removed on rClick on empty or esc

	glm::vec4 *one = *(markedNodes.points.begin());
	markedNodes.clear();
	markedNodes.push(*one);

	if(one == &(polyline->points.front())) markedNodes.preferedDirection = +1;
	else if(one == &(polyline->points.back())) markedNodes.preferedDirection = -1;

}
void PolylineEditor::subdivide(){
    polylineEditor->save();
	if(markedNodes.points.empty()) return;
	markedNodes.prepareToDelete();
	markedNodes.clear();

	auto &vec = polyline->points;
	for(u32 i=1; i<vec.size(); i++){
		if(vec[i].w == -1 && vec[i-1].w == -1){
			auto inserted = vec.insert(vec.begin() + i, (vec[i] + vec[i-1])/2.f);
			i++;
			markedNodes.push(*inserted);
		}
	}

	cleanUpNodes();
	polyline->generatePath();
}
void PolylineEditor::cleanUpNodes(){
		for(auto &it : polyline->points)
				it.w = 1;
}
void PolylineEditor::removeMarked(){
	markedNodes.prepareToDelete();

	for(auto it = polyline->points.begin(); it != polyline->points.end(); ){
		if(it->w == -1)
			it = polyline->points.erase(it);
		else
			it++;
	}
	markedNodes.clear();
	polyline->generatePath();
}
void PolylineEditor::removeWhenWlowerThanZero(){

	for(auto it = polyline->points.begin(); it != polyline->points.end(); ){
		if(it->w < 0.f)
			it = polyline->points.erase(it);
		else
			it++;
	}
	markedNodes.clear();
	if(not polyline->generatePath()){
        undo();
        // Helper::alert("Zbyt malo punktow");
    }
}
void PolylineEditor::mergePoints(){
    polylineEditor->save();
	markedNodes.prepareToDelete();
	glm::vec4 newPoint(0,0,0,0);
	int firstIdx = -1;

	for(u32 i=0; i<polyline->points.size(); i++){
		if(polyline->points[i].w == -1.f){
			newPoint += polyline->points[i];
			if(firstIdx == -1)
                firstIdx = i;
		}
	}
	newPoint /= abs(newPoint.w);
    newPoint.w = 1.f;
	polyline->points[firstIdx] = newPoint;

	removeWhenWlowerThanZero();
}

void PolylineEditor::processMouse(int button, int action, int modifier){
	if(button == GLFW_MOUSE_BUTTON_RIGHT and action == GLFW_RELEASE){
		editorState &= ~EditorFlags::PinSelectedToMouse;
        markedNodes.setOffset({0,0});
	}
}
void PolylineEditor::processKeys(int key, int action, int modifier){
	if(not polyline) return;
	if(action == GLFW_PRESS){
		if(key == 'C' and (editorState & EditorFlags::PinSelectedToMouse) != EditorFlags::PinSelectedToMouse){
			markedNodes.preferedDirection = 0;
			editorState |= EditorFlags::SnapToAxes;
		}
		else if(modifier & GLFW_MOD_CONTROL){
			editorState |= EditorFlags::MultiSelection;
		}
		else if(key == GLFW_KEY_DELETE){
			removeMarked();
		}
		else if(key == 'W'){
			subdivide();
		}
		else if(key == 'E' and modifier & GLFW_MOD_CONTROL){
			slide();
		}
		else if(key == 'E'){
			editorState |= EditorFlags::PinSelectedToMouse; /// removed on rClick on empty or esc
			extrude();
		}
		else if(key == 'Z' and modifier & GLFW_MOD_CONTROL){
            polylineEditor->undoAndClearSelection();
        }
		else if(key == 'M' and modifier & GLFW_MOD_ALT){
			mergePoints();
		}
		else if(key == 'X' and (editorState & EditorFlags::PinSelectedToMouse) != EditorFlags::PinSelectedToMouse){
			markedNodes.preferedDirection = 1;
			editorState |= EditorFlags::SnapToAxes;
		}
		else if(key == 'Y' and (editorState & EditorFlags::PinSelectedToMouse) != EditorFlags::PinSelectedToMouse){
			markedNodes.preferedDirection = 2;
			editorState |= EditorFlags::SnapToAxes;
		}
		else if(key == 'Z' and (editorState & EditorFlags::PinSelectedToMouse) != EditorFlags::PinSelectedToMouse){
			markedNodes.preferedDirection = 3;
			editorState |= EditorFlags::SnapToAxes;
		}
	}
	if(action == GLFW_RELEASE){
		if(key == 'C'){
            editorState &= ~EditorFlags::PinSelectedToMouse;
			markedNodes.preferedDirection = 0;
			editorState &= ~EditorFlags::SnapToAxes;
		}
		else if(key == 'X'){
            editorState &= ~EditorFlags::PinSelectedToMouse;
			markedNodes.preferedDirection = 0;
			editorState &= ~EditorFlags::SnapToAxes;
		}
		else if(key == 'Y'){
            editorState &= ~EditorFlags::PinSelectedToMouse;
			markedNodes.preferedDirection = 0;
			editorState &= ~EditorFlags::SnapToAxes;
		}
		else if(key == 'Z'){
            editorState &= ~EditorFlags::PinSelectedToMouse;
			markedNodes.preferedDirection = 0;
			editorState &= ~EditorFlags::SnapToAxes;
		}
		else if(not (modifier & GLFW_MOD_CONTROL)){
			editorState &= ~EditorFlags::MultiSelection;
		}

	}
}

NAM_END




