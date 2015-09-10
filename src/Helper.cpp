/**
 *  Różnego rodzaju funkcje pomocnicze, zapewniają acces do zupełnie dziwnych/innych rzeczy
 *  Odpowiadają za różne skomplikowane akcje, rozszerzenie edytora
 *
 */
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <Utils/Camera.h>
#include <Utils/IMGUI_V4.h>
#include "IInterpolator.h"
#include "Robot.h"
#include "Robot-Commands.h"
#include "RobotController.h"
#include "CFGparser.h"
#include "ResourceLoader.h"
#include "Editor/MoveCommandBuilder.h"
#include "Editor/ExecuteCommandBuilder.h"
#include "BulletWorld.h"
#include "PythonBindings.h"
#include <boost/filesystem.hpp>

#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";
#include "Helper.h"
extern UI::IMGUI ui;
namespace UI {
extern int g_UILayer;
};

extern shared_ptr<Scene> scene;
extern BulletWorld bulletWorld;
extern shared_ptr<RobotController> RC;
extern const float pi;
extern glm::vec2 screenSize;
extern glm::vec3 camPosition;
extern GLFWwindow *window;

namespace PythonBindings {
extern boost::python::object mainScript;
}
namespace Helper NAM_START
using namespace boost::filesystem;

float cameraStep = 5*pi/180;

int currentModifierKey;
DataUnderMouse dataUnderMouse {0.f, {}, {}, 1};
DataUnderMouse oldDataUnderMouse {0.f, {}, {}, 1};
glm::vec4 getPositionUnderMouse(){
	return dataUnderMouse.position;
}
glm::vec4 getNormalUnderMouse(){
	return dataUnderMouse.normal;
}
shared_ptr<Entity> getObjectUnderMouse(){
	if(dataUnderMouse.objID == 0 || dataUnderMouse.objID > scene->units_ptrs.size()-1) return shared_ptr<Entity>(nullptr);
	return (scene->units_ptrs[dataUnderMouse.objID]);
}
u32 getIDUnderMouse(){
	return dataUnderMouse.objID;
}

bool pinCursorToScreen(glm::vec2 screen, glm::vec2 &mouse){
	bool b = false;
	if(mouse.x >= screen.x){b = true; mouse.x = 0;}
	else if(mouse.x <= 0.f){b = true; mouse.x = screen.x;}
	if(mouse.y >= screen.y){b = true; mouse.y = 0;}
	else if(mouse.y <= 0.f){b = true; mouse.y = screen.y;}

	if(b)
		glfwSetCursorPos(window, mouse.x, mouse.y);

	return b;
}

void moveCameraByKeys(Camera &camera, int key, int action, int mods){
	currentModifierKey = mods;

	if(action == GLFW_PRESS || action == GLFW_REPEAT){ /// 2,4,6,8
		if(key == GLFW_KEY_KP_2){
			camera.rot_x += cameraStep;
		}
		else if(key == GLFW_KEY_KP_4){
			camera.rot_z += cameraStep;
		}
		else if(key == GLFW_KEY_KP_6){
			camera.rot_z -= cameraStep;
		}
		else if(key == GLFW_KEY_KP_8){
			camera.rot_x -= cameraStep;
		}
	}
	if(action == GLFW_PRESS){ /// 1,3,5,7
		if(key == GLFW_KEY_KP_1 && mods == GLFW_MOD_CONTROL)
			camera.setOrientation(pi/2, pi);
		else if(key == GLFW_KEY_KP_1&& mods != GLFW_MOD_CONTROL)
			camera.setOrientation(pi/2, 0);

		else if(key == GLFW_KEY_KP_3 && mods == GLFW_MOD_CONTROL)
			camera.setOrientation(pi/2, pi+pi/2);
		else if(key == GLFW_KEY_KP_3 && mods != GLFW_MOD_CONTROL)
			camera.setOrientation(pi/2, pi/2);

		else if(key == GLFW_KEY_KP_7 && mods == GLFW_MOD_CONTROL)
			camera.setOrientation(pi, 0);
		else if(key == GLFW_KEY_KP_7 && mods != GLFW_MOD_CONTROL)
			camera.setOrientation(0, 0);
		else if(key == GLFW_KEY_KP_5){
			if(camera.cameraProjection == PERSPECTIVE_PROJECTION)
					camera.cameraProjection = ORTHO_PROJECTION;
			else
				camera.cameraProjection = PERSPECTIVE_PROJECTION;
			camera.setProjection();
		}
	}
}
void moveCameraByMouse(Camera &camera, glm::vec2 mousePos, glm::vec2 mouseMov, bool pressed){
	if(not pressed) return;
	if(currentModifierKey&GLFW_MOD_SHIFT && currentModifierKey&GLFW_MOD_CONTROL){
		camPosition -= camera.Normal.xyz()*mouseMov.y/screenSize.y*10.f;
	}
	else if(currentModifierKey & GLFW_MOD_SHIFT){
		glm::vec3 up = camera.Up.xyz();
		glm::vec3 right = camera.Right.xyz();
		glm::vec3 &pos = camPosition;

		if(pinCursorToScreen(screenSize, mousePos)) return;

		mouseMov = mouseMov/screenSize*30.f;
		pos += up*mouseMov.y + right*mouseMov.x;

	}
	else if(currentModifierKey & GLFW_MOD_CONTROL){
		camera.zoomStep(-mouseMov.y/screenSize.y*15.f);
	}
	else {
		if(ui.hasHover(glm::vec4(0,0,screenSize.x, screenSize.y))){
			float dx = mouseMov.x/screenSize.x*1.3f;
			float dy = mouseMov.y/screenSize.y*1.3f;
			camera.setMouseMov(dx, -dy);
	}
	}
}

/// shift przesuwa w poziomie do przodu kamery, ctrl w bok(center kamery)
void moveCameraByScroll(Camera &camera, double xOff, double yOff){
	if(currentModifierKey & GLFW_MOD_SHIFT){
	}
	if(currentModifierKey & GLFW_MOD_CONTROL){
	}


}

/// ----------------------------------------------------- ALERTS
class Alert
{
	bool active = false;
	string text = "";
public:
	~Alert(){
		// cerr<<"~Alert"<<endl;
	}
	void operator()(const std::string &s){
		active = true;
		text = s;
	}
	bool show(){
		if(!active)
			return false;
        ui.beginLayer();
		ui.box(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::FixedPos | UI::NewLayer | UI::Draw)
            .overridePosition(screenSize.x / 2 - 100, screenSize.y / 2);
			ui.rect(200, 30).color(0xFF2020FF).text("Warning!", "ui_14"s, UI::CenterText)();
			ui.rect(200, 100).text(text, "ui_12"s, UI::CenterText)();
			ui.rect(200, 30).text("OK", "ui_14"s, UI::CenterText).button(active)(UI::Hoverable);

        if(ui.lClick() && ui.outOfTable()){
			active = false;
		}
		ui.endBox();
        ui.endLayer();
        return true;
	}

} g_alert;

void alert(const std::string &s){
    g_alert(s);
}
/// ----------------------------------------------------- POPUPS
struct Popup
{
	std::string title;
	std::function<bool(void)> fun;
};

std::vector<Popup> g_popups;

void popup(const Popup &p){
	g_popups.push_back(p);
}
void showPopups(){
    if(g_alert.show()) return;
	if(g_popups.size() > 0){
		auto pop = g_popups.back();
		ui.beginLayer();
		ui.box(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::FixedPos | UI::NewLayer | UI::Draw)
			.overridePosition(screenSize.x / 2 - 100, screenSize.y / 2);

		ui.rect(200, 50).text(pop.title, "ui_14"s, UI::CenterText)(UI::CaptureMouse);
		if(pop.fun()){
			g_popups.pop_back();

		}

		ui.mouseOverButton = true;
		// if(ui.outOfTable()) ui.mouseOverButton = true;
		if(ui.lClick() && ui.outOfTable()){
			g_popups.pop_back();
		}
		ui.endBox();
		ui.endLayer();

	}
}

/// ----------------------------------------------------- CURSOR

std::map<std::string, glm::vec4> g_pointList;
std::string newPointName = "";
glm::vec4 cursor(0,0,0,1);

u32 groupPointCount;
void savePoint(const std::string &s, const glm::vec4 &p){
	g_pointList[s] = p;
}
void getPoint(const std::string &name){}
void deletePoint(Point *p){}
void deletePoint(const std::string &name){
	g_pointList.erase(name);
}
std::string generatePointName(){
	char buff[4] = "000";
	sprintf(buff, "%.3u", groupPointCount++);
	return "Point."s+buff;
}

glm::vec4& getCursor(){
	return cursor;
}
glm::vec2 getScreenCursor(Camera &camera){
	auto pos = glm::project(cursor.xyz(), camera.ViewMatrix, camera.ProjectionMatrix, glm::vec4(0,0,camera.m_width, camera.m_height));
	return glm::ceil(pos.xy() - glm::vec2(15,15));
}

void setCursor(glm::vec4 v){
	cursor = v;
}
void moveCursor(glm::vec4 v){
	cursor = v;
}
void restoreCursorPos(const std::string &name){
	setCursor(g_pointList[name]);
}
std::map<std::string, glm::vec4>& pointList(){
	return g_pointList;
}
void cursorWidgetHorizontal(glm::vec2 pos){
	ui.box(UI::LayoutHorizontal);
	// ui.rect(pos.x-70, pos.y, 70, 20).font("ui_12"s).edit(cursor.x)(UI::CaptureMouse);
	// ui.rect(pos.x, pos.y, 70, 20).font("ui_12"s).edit(cursor.y)(UI::CaptureMouse);
	// ui.rect(pos.x+70, pos.y, 70, 20).font("ui_12"s).edit(cursor.z)(UI::CaptureMouse);
	// ui.rect(pos.x+140, pos.y, 70, 20).font("ui_12"s).text("save")(UI::Button).onlClick([]{
	ui.rect(70, 20).font("ui_12"s).edit(cursor.x)(UI::Hoverable);
	ui.rect(70, 20).font("ui_12"s).edit(cursor.y)(UI::Hoverable);
	ui.rect(70, 20).font("ui_12"s).edit(cursor.z)(UI::Hoverable);
	ui.rect(70, 20).font("ui_12"s).text("save")(UI::Button).onlClick([]{
		newPointName = generatePointName();
		popup({"Set name", []()->bool{
			bool out = false;
			auto fun = [&out]{out = true; savePoint(newPointName, cursor); std::cout<<"Point " +newPointName+ " saved"<<std::endl;};
			ui.rect(200,30).edit(newPointName, fun)(UI::CaptureMouse);
			ui.onEnter(fun);
			return out;
		}});
	});
	ui.endBox();
}

/// ----------------------------------------------------- SELECTION
std::vector<shared_ptr<Entity>> g_currentSelection;
std::map<std::string, std::vector<shared_ptr<Entity>>> g_groupList;
u32 groupNameCount;
bool modifyingExistingGroup = false;
std::string existingName;

std::map<std::string, std::vector<shared_ptr<Entity>>>& groupList(){
	return g_groupList;
}
void deleteGroup(const std::string &name){
	g_groupList.erase(name);
}
void appendToSelection(const std::string &name){
	g_currentSelection.insert(g_currentSelection.end(), g_groupList[name].begin(), g_groupList[name].end());
}
void restoreSelection(const std::string &name){
	modifyingExistingGroup = true;
	existingName = name;
	g_currentSelection = g_groupList[name];
}
bool processMouse(int key, int action, int mods){
	if(ui.mouseOverButton) return false;
	if(key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		setCursor(dataUnderMouse.position);
		std::cout<<"Cursor!"<<std::endl;
	}
	if((mods & GLFW_MOD_CONTROL) && key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		auto &&obj = getObjectUnderMouse();
		if(obj){
			for(u32 i=0; i<g_currentSelection.size(); i++){
				if(g_currentSelection[i]->ID == obj->ID){
					g_currentSelection[i] = g_currentSelection.back();
					g_currentSelection.pop_back();
					return false;
				}

			}
			g_currentSelection.push_back(obj);
		}
	}
	else if(key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		std::cout<<"Marked!"<<std::endl;
		auto &&obj = getObjectUnderMouse();
		if(obj){
			if(not g_currentSelection.empty() && g_currentSelection[0] == obj){
				g_currentSelection.clear();
				return false;
			}
			g_currentSelection.clear();
			g_currentSelection.push_back(obj);
		}
	}
	return false;
}
bool processKeys(int key, int action, int mods){
	if(action == GLFW_PRESS){
		if(key == 'S' && mods == GLFW_MOD_CONTROL && !g_currentSelection.empty()){
			newPointName = generatePointName();
			popup({"Set name", []()->bool{
				bool out = false;
				auto fun = [&out]{out = true; saveGroup(g_currentSelection); std::cout<<"Group " +newPointName+ " saved"<<std::endl;};
				ui.rect(200,30).edit(newPointName, fun)(UI::CaptureMouse);
				ui.onEnter(fun);
				return out;
			}});
		}
	}
}
void saveGroup(std::vector<shared_ptr<Entity>> &s){
	if(modifyingExistingGroup){
		g_groupList[existingName] = s;
		modifyingExistingGroup = false;
	}
	else
		g_groupList[generateGroupName()] = s;
	s.clear();
}
void unselect(){
	g_currentSelection.clear();
}
std::vector<shared_ptr<Entity>>& getSelection(){
	return g_currentSelection;
}
std::vector<shared_ptr<Entity>>& getGroup(const std::string &name);
std::string generateGroupName(){
	char buff[4] = "000";
	sprintf(buff, "%.3u", groupNameCount);
	groupNameCount++;
	return "Group."s+buff;
}


/// ----------------------------------------------------- FILESYSTEM
void reloadScene(const std::string &sceneName, shared_ptr<RobotController> &RC, shared_ptr<Scene> &scene, BulletWorld &bulletWorld){
	bulletWorld.clear();

	scene = make_shared<Scene>();
	RC = make_shared<RobotController>();
	bulletWorld.init();

	ResourceLoader loader(scene->resources);
	auto &&resources = CFG::Load(sceneName);
	loader.loadScene(*scene, bulletWorld, resources);

	RC->robot = scene->robot;

  PythonBindings::reloadAndInitMainScript(RC, scene);
}

/// http://stackoverflow.com/questions/9285384/how-does-import-work-with-boost-python-from-inside-python-files

vector<string> listFilesInDirectory(const string &dir, const string &ext){
	vector <string> out;
	try {
		path p(dir);
		if(is_directory(p)){
			auto dir_it = directory_iterator(p);
			for(dir_it; dir_it != directory_iterator(); dir_it++){
				if((*dir_it).path().extension().string() == ext)
					out.push_back((*dir_it).path().stem().string());
			}
		}
	}
	catch (const filesystem_error& ex){
		cout << ex.what() << '\n';
	}
	return out;
}
void handleYamlFileDrop(const string &path){
	auto &&yamlFile = CFG::Load(path);
	if(yamlFile.has("Robot") && yamlFile.has("Meshes"))
		reloadScene(path, RC, scene, bulletWorld);
}
void handlePythonFileDrop(const string &path){
	boost::filesystem::path p(path);
	std::string s = p.stem().string();
	PythonBindings::executeSubScript(p.stem().string());
}

void handleDrop(const string &path){
	boost::filesystem::path p(path);
	const string ext = p.extension().string();
	if(ext == ".yml") handleYamlFileDrop(path);
	else if(ext == ".py") handlePythonFileDrop(path);
	else cout<<"Unknown file type."<<endl;
}

std::string getClipboard(){}
void dropCallback(int count, const char** paths){
	for(u32 i=0; i<count; i++){
		handleDrop(paths[i]);
	}
}

/// ----------------------------------------------------- DIRECT ROBOT CONTROL

class DirectControlWidget
{
public:
	i32 moduleUnderEdition = -1;
	bool directControlEnabled = true;
	enum ControlType : u32 {
		None = 0, Joint, Cartesian
	} controlType;
	enum SolverTarget {
		OnlyPosition=0, OnlyOrientation, PositionAndOrientation
	} solverTarget;
	i32 getMarkedModule(){
		return moduleUnderEdition;
	}

	std::vector<double> history;

	glm::vec3 eulerAngles;
	glm::vec3 axis;
	glm::vec4 position;
	bool automaticInsert = true;

	void extendedEditButton(float &v, float incrVal = 0.01f){
		ui.rect(70, 20).font("ui_12"s);
		// if(currentModifierKey == GLFW_MOD_CONTROL){
			ui.text(to_string(v))(UI::Hoverable)
			.onlPressed([&v, incrVal]{v += incrVal;})
			.onrPressed([&v, incrVal]{v-= incrVal;});
		// }
		// else {
			// ui.edit(v)(UI::Hoverable);
		// }
	}
	void extendedEditButton(double &v, double incrVal = 0.01){
		ui.rect(70, 20).font("ui_12"s);
		if(currentModifierKey == GLFW_MOD_CONTROL){
			ui.text(to_string(v))(UI::Hoverable)
			.onlPressed([&v, incrVal]{v += incrVal;})
			.onrPressed([&v, incrVal]{v-= incrVal;});
		}
		else {
			ui.edit(v)(UI::Hoverable);
		}
	}
	void pushCanges(RobotController &RC){
		if(solverTarget == OnlyPosition){
			JT0 solver;
			solver.solve(Point{ position, glm::quat(eulerAngles) }, *(RC.robot));
			if(solver.succes) RC.robot->insertVariables(solver.result);
			// else alert("Solver failed");
		}
		else if(solverTarget == OnlyOrientation){
			JT2 solver;
			solver.solve(Point{ RC.robot->endEffector.position, glm::quat(eulerAngles) }, *(RC.robot));
			if(solver.succes) RC.robot->insertVariables(solver.result);
			// else alert("Solver failed");
		}
		else {
			JT2 solver;
			// solver.solve(Point{ position, glm::quat(eulerAngles) }, *(RC.robot));
			solver.solve(Point{ position, glm::quat(1.f, axis) }, *(RC.robot));
			// if(solver.succes) RC.robot->insertVariables(solver.result);
			RC.robot->insertVariables(solver.result);
			// else alert("Solver failed");
		}

		// auto q = RC.robot->endEffector.quat;
		// axis = glm::axis(q);
		// eulerAngles = glm::eulerAngles(q);

	}
	void run(u32 x, u32 y, glm::vec2 mousePos, RobotController &RC){
		ui.table(UI::LayoutVertical | UI::AlignLeft | UI::AlignBottom )
			.overridePosition(x, y);

			cursorWidgetHorizontal({x,y});

			if(not directControlEnabled){
				ui.rect(200, 20).text("Show panel").button(directControlEnabled)();
				ui.endTable(); return;
			}

			ui.box(UI::LayoutHorizontal);{
				ui.rect(100, 20).color(controlType == Joint?0xFFA000FF : 0xA0A0A0FF).text("Joint control").radio(controlType, Joint)(UI::CaptureMouse);
				ui.rect(100, 20).color(controlType == Cartesian?0xFFA000FF : 0xA0A0A0FF).text("Cartesian control").radio(controlType, Cartesian)(UI::CaptureMouse).onlClick([this, &RC]{
                    position = RC.robot->endEffector.position;
                    auto q = RC.robot->endEffector.quat;
                    axis = glm::axis(q);
                    eulerAngles = glm::eulerAngles(q);
										history = RC.robot->getVariables();
										moduleUnderEdition = -1;
                });}
			ui.endBox();

			if(controlType == Joint){
				for(i32 i=0; i<RC.robot->chain.size(); ++i){

					auto &module = *RC.robot->chain[i];

					horizontal(
						ui.rect(25, 22).text("-", "ui_17", UI::CenterText)
							// .onlPressed([&module]{module.decr();}, 5u)
							.onlPressed([&module]{module.decr();})
							.onlClick([&module]{module.decr();}) (UI::Button);
						// ui.rect(150,22).edit(module.value)(UI::Hoverable);

						extendedEditButton(module.value);

						ui.rect(25, 22).text("+", "ui_17", UI::CenterText)
							.onlPressed([&module]{module.incr();})
							.onlClick([&module]{module.incr();}) (UI::Button);
					);
					if(not ui.outOfTable()){
						moduleUnderEdition = i;
						UI::g_UILayer++;
						ui.rect(glm::vec4(mousePos+glm::vec2(5, 15), 100, 20)).text(module.name)(UI::Label);
						UI::g_UILayer--;
					}
				}
			}
			else if(controlType == Cartesian){
				horizontal(
					extendedEditButton(position.x);
					extendedEditButton(position.y);
					extendedEditButton(position.z);
				);
				if(not ui.outOfTable() && (ui.mouseLPressed || ui.mouseRPressed)){
					if(automaticInsert){
						pushCanges(RC);
						/// update rest of..
						auto q = RC.robot->endEffector.quat;
						axis = glm::axis(q);
						eulerAngles = glm::eulerAngles(q);
					}
				}
				ui.rect(70, 15).font("ui_10"s).text("Position (X,Y,Z):")(UI::CaptureMouse);
				horizontal(
					extendedEditButton(axis.x);
					extendedEditButton(axis.y);
					extendedEditButton(axis.z);
				);
				if(not ui.outOfTable() && (ui.mouseLPressed || ui.mouseRPressed)){
					axis = glm::normalize(axis);
					if(automaticInsert) pushCanges(RC);
					eulerAngles = glm::eulerAngles(glm::quat(1, axis));
				}
				ui.rect(70, 15).font("ui_10"s).text("Axis (X,Y,Z):")(UI::CaptureMouse);
				horizontal(
					extendedEditButton(eulerAngles.x);
					extendedEditButton(eulerAngles.y);
					extendedEditButton(eulerAngles.z);
				);
				if(not ui.outOfTable() && (ui.mouseLPressed || ui.mouseRPressed)){
					if(automaticInsert) pushCanges(RC);
					axis = glm::axis(glm::quat(eulerAngles));
				}
				ui.rect(70, 15).font("ui_10"s).text("Euler:")(UI::CaptureMouse);

				horizontal(
					ui.rect(70, 20).color(solverTarget == OnlyPosition?0xFFA000FF : 0xA0A0A0FF).text("Position").radio(solverTarget, OnlyPosition)(UI::Hoverable);
					ui.rect(70, 20).color(solverTarget == OnlyOrientation?0xFFA000FF : 0xA0A0A0FF).text("Orientation").radio(solverTarget, OnlyOrientation)(UI::Hoverable);
					ui.rect(70, 20).color(solverTarget == PositionAndOrientation?0xFFA000FF : 0xA0A0A0FF).text("Both").radio(solverTarget, PositionAndOrientation)(UI::Hoverable);
				);
				ui.rect(70, 15).font("ui_10"s).text("What to insert:")(UI::CaptureMouse);

				horizontal(
					ui.rect(70, 20).color(0xA0A0A0FF).text("Read")(UI::Hoverable)
						.onlClick([&RC, this]{
							position = RC.robot->endEffector.position;
							auto q = RC.robot->endEffector.quat;
							axis = glm::axis(q);
							eulerAngles = glm::eulerAngles(q);
						});
					ui.rect(70, 20).color(0xA0A0A0FF).text("Insert")(UI::Hoverable)
						.onlClick([&RC, this]{
							pushCanges(RC);
							});
					ui.rect(70, 20).color(automaticInsert ? 0xFFA000FF : 0xA0A0A0FF).text("Automatic").button(automaticInsert)(UI::Hoverable);
					);
			}

			ui.box(UI::LayoutHorizontal);{
			ui.rect(100, 20).color( 0xA0A0A0FF).text("Restore")(UI::Hoverable).onlClick([this, &RC]{
					RC.robot->insertVariables(history);
				});
			ui.rect(100, 20).color(0xA0A0A0FF).text("Save")(UI::Hoverable).onlClick([this, &RC]{
					history = RC.robot->getVariables();
				});
			ui.endBox();}
			ui.endTable();
	}
} robotDirectControl;

void directControlWidget(u32 x, u32 y, glm::vec2 mousePos, RobotController &RC){
	robotDirectControl.run(x,y, mousePos, RC);
}
i32 getMarkedModule(){
	robotDirectControl.getMarkedModule();
}

/// ----------------------------------------------------- SAVE STATE

void saveState(const std::string &fileName){}
void loadState(const std::string &fileName){}
// void saveState(){}
// void loadState(){}0x

std::map<std::string, std::vector<float>> g_floats;
std::map<std::string, std::vector<double>> g_doubles;
std::map<std::string, std::vector<glm::vec4>> g_vec4;

void storeFloat(const std::string &s, std::vector<float>&v){
    g_floats[s] = v;
}
void storeDouble(const std::string &s, std::vector<double>&v){
    g_doubles[s] = v;
}
void storeVec4(const std::string &s, std::vector<glm::vec4>&v){
    g_vec4[s] = v;
}

std::vector<float>& storeFloat(const std::string &s){
    return g_floats[s];
}
std::vector<double>& storeDouble(const std::string &s){
    return g_doubles[s];
}
std::vector<glm::vec4>& storeVec4(const std::string &s){
    return g_vec4[s];
}

/// ----------------------------------------------------- 	COLLISIONS
std::pair<shared_ptr<Entity> , shared_ptr<Entity> > g_pairInCollision;
void collidingPair(shared_ptr<Entity> &a, shared_ptr<Entity> &b){

}

/// ----------------------------------------------------- DATA RECORDING
FrameRecordedData g_record;
FrameRecordedData& record(){
    return g_record;
}



NAM_END