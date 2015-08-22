/**
 *  Różnego rodzaju funkcje pomocnicze, zapewniają acces do zupełnie dziwnych/innych rzeczy
 *  Odpowiadają za różne skomplikowane akcje, rozszerzenie edytora
 *
 */
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <Utils/Camera.h>
#include "IInterpolator.h"
#include "Robot.h"
#include "Robot-Commands.h"
#include "RobotController.h"
#include "CFGparser.h"
#include "ResourceLoader.h"
#include "Editor/MoveCommandBuilder.h"
#include "Editor/ExecuteCommandBuilder.h"

#include "Helper.h"

extern Scene scene;
extern const float pi;

namespace Helper NAM_START

int currentModifierKey;
DataUnderMouse dataUnderMouse;
glm::vec4 getPositionUnderMouse(){
	return dataUnderMouse.position;
}
glm::vec4 getNormalUnderMouse(){
	return dataUnderMouse.normal;
}
Entity* getObjectUnderMouse(){
	return (scene.units_ptrs[dataUnderMouse.objID]);
}

void moveCameraByKeys(Camera &camera, int key, int action, int mods){
	currentModifierKey = mods;

	if(action != GLFW_PRESS) return;

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
void moveCameraByMouse(Camera &camera, int key, int action, int mods){}

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


/// --------------------------------
std::vector<Entity*> currentSelection;
std::map<std::string, std::vector<Entity*>> groupList;
u32 groupNameCount;

bool processMouse(int key, int action, int mods){
	if((mods & GLFW_MOD_CONTROL) && key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		currentSelection.push_back(getObjectUnderMouse());
	}
	else if(key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		currentSelection.clear();
		currentSelection.push_back(getObjectUnderMouse());
	}

}
bool processKeys(int key, int action, int mods){
	if(action == GLFW_PRESS){
		if((key == GLFW_KEY_KP_ENTER || key == GLFW_KEY_ENTER ) && !currentSelection.empty()){
			saveGroup(currentSelection);
		}
	}
}
void saveGroup(std::vector<Entity*> &s){
	groupList[generateGroupName()] = s;
	s.clear();
}
std::vector<Entity*>& getGroup(const std::string &name);
std::string generateGroupName(){
	char buff[4] = "000";
	sprintf(buff, "%.3u", groupNameCount);
	return "Group."s+buff;
}



NAM_END