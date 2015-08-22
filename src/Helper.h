﻿#pragma once
#include <Utils/Camera.h>

#define NAM_END }
#define NAM_START {

namespace Helper NAM_START

struct DataUnderMouse
{
    float depth;
    glm::vec4 position;
    glm::vec4 normal;
    u16 objID;
};

glm::vec4 getPositionUnderMouse();
glm::vec4 getNormalUnderMouse();
Entity* getObjectUnderMouse();

void moveCameraByKeys(Camera &camera, int key, int action, int mods);
void moveCameraByMouse(Camera &camera, int key, int action, int mods);
void moveCameraByScroll(Camera &camera, double xOff, double yOff);

void savePoint();
void getPoint(const std::string &name);
void deletePoint(Point *p);
void deletePoint(const std::string &name);
std::string generatePointName();

bool processMouse(int key, int action, int mods);
bool processKeys(int key, int action, int mods);
void saveGroup(std::vector<Entity*>&);
std::vector<Entity*>& getGroup(const std::string &name);
std::string generateGroupName();


NAM_END