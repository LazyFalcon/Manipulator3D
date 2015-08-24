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

extern shared_ptr<Scene> scene;
extern const float pi;

namespace Helper NAM_START

float cameraStep = 5*pi/180;

int currentModifierKey;
DataUnderMouse dataUnderMouse {0.f, {}, {}, 1};
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
	if((mods & GLFW_MOD_CONTROL) && key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		auto &&obj = getObjectUnderMouse();
		if(obj)
			currentSelection.push_back(obj);
	}
	else if(key == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
		auto &&obj = getObjectUnderMouse();
		if(obj){
			currentSelection.clear();
			currentSelection.push_back(obj);
		}
	}

}
bool processKeys(int key, int action, int mods){
	if(action == GLFW_PRESS){
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



NAM_END