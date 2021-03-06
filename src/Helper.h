﻿#pragma once
#include "RobotController.h"

#define NAM_END }
#define NAM_START {

struct Point;
class Camera;
class Scene;
class BulletWorld;
namespace Helper NAM_START
struct DataUnderMouse
{
	float depth;
	glm::vec4 position;
	glm::vec4 normal;
	u16 objID;
		glm::vec2 uv;
};

glm::vec4 getPositionUnderMouse();
glm::vec4 getNormalUnderMouse();
shared_ptr<Entity> getObjectUnderMouse();
u32 getIDUnderMouse();

void moveCameraByKeys(Camera &camera, int key, int action, int mods);
void moveCameraByMouse(Camera &camera, glm::vec2 mousePos, glm::vec2 mouseMov, bool pressed);
void moveCameraByScroll(Camera &camera, double xOff, double yOff);

void savePoint(const std::string &s, const glm::vec4 &p);
void getPoint(const std::string &name);
void deletePoint(Point *p);
void deletePoint(const std::string &name);
std::string generatePointName();

std::map<std::string, std::vector<shared_ptr<Entity>>>& groupList();
bool processMouse(int key, int action, int mods);
bool processKeys(int key, int action, int mods);

void saveGroup(std::vector<shared_ptr<Entity>>&);
void restoreSelection(const std::string &name);
void deleteGroup(const std::string &name);
void appendToSelection(const std::string &name);
void unselect();
std::vector<shared_ptr<Entity>>& getSelection();
std::vector<shared_ptr<Entity>>& getGroup(const std::string &name);
std::string generateGroupName();
///
void alert(const std::string &s);
void showPopups();

///
glm::vec4& getCursor();

std::map<std::string, glm::vec4>& pointList();
void setCursor(glm::vec4 v);
void moveCursor(glm::vec4 v);
void restoreCursorPos(const std::string &name);
glm::vec2 getScreenCursor(Camera &camera);
void cursorVidgetHorizontal(glm::vec2 pos);

/// -------------------------------- FILESYSTEM
/**
 *  Obejmuje:
 *  * przeładowanie meshy
 *  * przeładowanie robota
 *  * przeładowanie RC
 *  *
 *
 */
void reloadScene(const std::string &sceneName, shared_ptr<RobotController> &RC, shared_ptr<Scene> &scene, BulletWorld &bulletWorld);

vector<string> listFilesInDirectory(const string &dir, const string &ext);
std::string getClipboard();
void dropCallback(int count, const char** paths);

i32 getMarkedModule();
void directControlWidget(u32 x, u32 y, glm::vec2 mousePos, RobotController &RC);
void saveRobot(shared_ptr<RobotController> &RC);
void loadRobot(shared_ptr<RobotController> &RC);
/// -------------------------------- SAVE STATE
/// Zapisuje stan, po co w sumie? akcji raczej zapisać się nie da
void saveState(const std::string &fileName);
void loadState(const std::string &fileName);
// void saveState();
// void loadState();storeDouble
void getFloat(const std::string &s, std::vector<float>&v);
void storeDouble(const std::string &s, std::vector<double>&v);
void getVec4(const std::string &s, std::vector<glm::vec4>&v);

std::vector<float>& storeFloat(const std::string &s);
std::vector<double>& getDouble(const std::string &s);
std::vector<glm::vec4>& storeVec4(const std::string &s);

/// ----------------------------------------------------- DATA RECORDING

struct FrameRecordedData
{
    double IKIterationTime;
    double IKIterarationCount;
    double IKPositionError;
    double IKOrientationError;

    double EffectorDelta;
    double EffectorVelocity;
    double EffectorAcceleration;

    double FrameTime;

    glm::vec4 EffectorPosition;
    glm::quat EffectorOrientation;

    std::vector<double> RobotJoints {-1,-1,-1,-1,-1,-1};

};

FrameRecordedData& record();

NAM_END