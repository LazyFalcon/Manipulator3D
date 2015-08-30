#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <Utils/IMGUI_V4.h>
#include <boost/python.hpp>
#include "Robot.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
#include "Robot-Commands.h"
#include "PythonBindings.h"
#include <boost/python/call.hpp>

namespace PythonBindings {
extern bpl::object mainScript;
}
vector <glm::vec4> fakePath;
u32 globalFlags = 0;
u32 lastPathIterationCount;
float lastPathIterationdistance;


void WaitCommand::init(shared_ptr<RobotController> &rc){
	isRuning = true;
};
bool WaitCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	// if(releaseTime <= 0.f || releaseFlag&globalFlags || (releaseFuction && releaseFuction()))
	if(releaseTime <= 0.f || releaseFlag&globalFlags)
		return true;

	releaseTime -= dt;
	time += dt;
	return false;
}
bool WaitCommand::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){}
vector<glm::vec4>& WaitCommand::getPath(){
	return fakePath;
}
vector<glm::vec4>& WaitCommand::getPolyline(){
	return fakePath;
}

void ExecuteCommand::init(shared_ptr<RobotController> &rc){
	isRuning = true;
	if(onEnter) onEnter(rc);
};
bool ExecuteCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	if(onUpdate && onUpdate(rc, scene, dt)) return true;
    else return exit(rc, scene);
    return false;
}
bool ExecuteCommand::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
    if(onExit) onExit(rc, scene);
    return true;
}
vector<glm::vec4>& ExecuteCommand::getPath(){
	return fakePath;
}
vector<glm::vec4>& ExecuteCommand::getPolyline(){
	return fakePath;
}

void ExecutePythonCommand::init(shared_ptr<RobotController> &rc){
	isRuning = true;
	try {
		callback.attr("enter")(rc);
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
	}
};
bool ExecutePythonCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	try {
		return boost::python::extract<bool>(callback.attr("update")(rc, scene, dt));
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
		return true;
	}
}

bool ExecutePythonCommand::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
	try {
		callback.attr("exit")(rc, scene);
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
	}
	return true;
}
vector<glm::vec4>& ExecutePythonCommand::getPath(){
	return fakePath;
}
vector<glm::vec4>& ExecutePythonCommand::getPolyline(){
	return fakePath;
}

void MoveCommand::init(shared_ptr<RobotController> &rc){
	requiredDistance = 0;
	isRuning = true;
	solver->solve(Point{ interpolator->firstPoint(), glm::quat(0, 0, 0, 1) }, *(rc->robot));
	targetJointPosition = solver->result;
	rc->robot->isReady = false;
	// rc->robot->goTo(targetJointPosition);
	std::cout<<"Init command"<<std::endl;

	previousPoint = rc->robot->endEffector.position;
}
double MoveCommand::calculateRequiredDistance(float dt){
	return dt*velocity;
}
glm::vec4 MoveCommand::calculateNextPoint(float dt){
	requiredDistance = calculateRequiredDistance(dt);
	glm::vec4 newTarget;
	glm::vec4 oldTarget = previousPoint;
    u32 i = 0;
	while(requiredDistance > 0.0 && (not interpolator->finished)){
        i++;
		newTarget = interpolator->nextPoint();
		requiredDistance -= glm::distance(previousPoint, newTarget);
		previousPoint = newTarget;
	}
    lastPathIterationdistance = glm::distance(previousPoint, oldTarget);
    lastPathIterationCount = i;

	return newTarget;
}
bool MoveCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
    // rc->robot->isReady = true;
	if(not rc->robot->isReady){
		rc->robot->goTo(dt, jointVelocityModifier);
		previousPoint = rc->robot->endEffector.position;
	}
    if(rc->robot->isReady && interpolator->finished){
		interpolator->reset();
		return true;
	}
	else if(rc->robot->isReady){
		glm::vec4 newTarget = calculateNextPoint(dt);

		solver->solve(Point{ newTarget, glm::quat(0, 0, 0, 1) }, *(rc->robot));
		targetJointPosition = solver->result;
		// rc->robot->insertVariables(targetJointPosition);
		rc->robot->goTo(targetJointPosition);
		rc->robot->goTo(dt, jointVelocityModifier);
		// previousPoint = rc->robot->endEffector.position;
		previousPoint = newTarget;
		return false;
	}
    return false;
}

void SingleJointMove::init(shared_ptr<RobotController> &rc){
	isRuning = true;
	rc->robot->goTo(targetJointPosition);
}
bool SingleJointMove::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	if(not rc->robot->isReady){
		rc->robot->goTo(dt, jointVelocityModifier);
		return false;
	}
	else return exit(rc, scene);
}
bool SingleJointMove::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
	return true;
}
vector<glm::vec4>& SingleJointMove::getPath(){
	return fakePath;
}
vector<glm::vec4>& SingleJointMove::getPolyline(){
	return fakePath;
}

void FollowObject::init(shared_ptr<RobotController> &rc){
	isRuning = true;
	solver->solve(Point{ *target, glm::quat(0, 0, 0, 1) }, *(rc->robot));
	targetJointPosition = solver->result;
	rc->robot->goTo(targetJointPosition);
}
bool FollowObject::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
    if(*target != pTarget){
        solver->solve(Point{ *target, glm::quat(0, 0, 0, 1) }, *(rc->robot));
        targetJointPosition = solver->result;
        rc->robot->goTo(targetJointPosition);
        pTarget = *target;
    }
	if(not rc->robot->isReady){
		rc->robot->goTo(dt, jointVelocityModifier);
		return false;
	}
	else return exit(rc, scene);
}
bool FollowObject::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
	return true;
}
vector<glm::vec4>& FollowObject::getPath(){
	return fakePath;
}
vector<glm::vec4>& FollowObject::getPolyline(){
	return fakePath;
}
