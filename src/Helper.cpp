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

#include "Helper.h"
extern UI::IMGUI ui;
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

/// --------------------------------
std::map<std::string, Point> pointList;
u32 groupPointCount;
void savePoint(){}
void getPoint(const std::string &name){}
void deletePoint(Point *p){}
void deletePoint(const std::string &name){}
std::string generatePointName(){
	char buff[4] = "000";
	sprintf(buff, "%.3u", groupPointCount);
	return "Point."s+buff;
}
/// -------------------------------- CURSOR

glm::vec4 cursor(0,0,0,1);

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

/// --------------------------------
std::vector<shared_ptr<Entity>> currentSelection;
std::map<std::string, std::vector<shared_ptr<Entity>>> groupList;
u32 groupNameCount;

std::map<std::string, std::vector<shared_ptr<Entity>>>& listOfGroups(){
    return groupList;
}
std::vector<shared_ptr<Entity>>& getCurrentSelection(){
	return currentSelection;
}
bool processMouse(int key, int action, int mods){
	if(key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		setCursor(dataUnderMouse.position);
		std::cout<<"Cursor!"<<std::endl;
	}
	if((mods & GLFW_MOD_CONTROL) && key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		auto &&obj = getObjectUnderMouse();
		if(obj)
			currentSelection.push_back(obj);
	}
	else if(key == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
		std::cout<<"Marked!"<<std::endl;
		auto &&obj = getObjectUnderMouse();
		if(obj){
			currentSelection.clear();
			currentSelection.push_back(obj);
		}
	}

}
bool processKeys(int key, int action, int mods){
	if(action == GLFW_PRESS){
		// if(key == 'W' && mods == GLFW_MOD_CONTROL){
			// PythonBindings::mainScript.attr("plotData")(RC, scene);
		// }
		if((key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_ENTER ) && !currentSelection.empty()){
			saveGroup(currentSelection);
		}
	}
}
void saveGroup(std::vector<shared_ptr<Entity>> &s){
	groupList[generateGroupName()] = s;
	s.clear();
}
std::vector<shared_ptr<Entity>>& getGroup(const std::string &name);
std::string generateGroupName(){
	char buff[4] = "000";
	sprintf(buff, "%.3u", groupNameCount);
	groupNameCount++;
	return "Group."s+buff;
}


/// -------------------------------- FILESYSTEM
void reloadScene(const std::string &sceneName, shared_ptr<RobotController> &RC, shared_ptr<Scene> &scene, BulletWorld &bulletWorld){
	bulletWorld.clear();

	scene = make_shared<Scene>();
	RC = make_shared<RobotController>();
	bulletWorld.init();

	ResourceLoader loader(scene->resources);
	auto &&resources = CFG::Load(sceneName);
	loader.loadScene(*scene, bulletWorld, resources);

	RC->robot = scene->robot;
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
	PythonBindings::loadMainScript(s.c_str(), RC, scene);
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


NAM_END