#include "Utils/Includes.h"
#include "Utils/Utils.h"
#include "Utils/BaseStructs.h"
#include "Utils/IMGUI_V4.h"
#include "Robot.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
#include "Robot-Commands.h"
#include "PythonBindings.h"
#include <boost/python.hpp>
#include <boost/python/call.hpp>
#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";

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
int WaitCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	// if(releaseTime <= 0.f || releaseFlag&globalFlags || (releaseFuction && releaseFuction()))
	if(releaseTime <= 0.f || releaseFlag&globalFlags)
		return exitAction;

	releaseTime -= dt;
	time += dt;
	return 0;
}
int WaitCommand::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){}
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
int ExecuteCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	if(onUpdate && onUpdate(rc, scene, dt)) return exitAction;
    else return exit(rc, scene);
    return 0;
}
int ExecuteCommand::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
    if(onExit) onExit(rc, scene);
    return exitAction;
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
		callback.attr("onEnter")(rc);
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
	}
};
int ExecutePythonCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	try {
		if( (boost::python::extract<bool>( callback.attr("onUpdate")(rc, scene, dt) ))() ){
			return exit(rc, scene);
		}
		return 0;
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
		return exitAction;
	}
}
int ExecutePythonCommand::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
	try {
		callback.attr("onExit")(rc, scene);
	}
	catch (boost::python::error_already_set) {
		PyErr_Print();
	}
	return exitAction;
}

vector<glm::vec4>& ExecutePythonCommand::getPath(){
	return fakePath;
}
vector<glm::vec4>& ExecutePythonCommand::getPolyline(){
	return fakePath;
}
double lastAdd;
void MoveCommand::init(shared_ptr<RobotController> &rc){
	requiredDistance = 0;
	currentVelocity = 0;

	isRuning = true;
	if(not startOrientationEnabled)
		startOrientation = rc->robot->endEffector.quat;
	if(not endOrientationEnabled)
		endOrientation = glm::quat(0,0,0,0);

	solver->solve(Point{ interpolator->firstPoint(), startOrientation }, *(rc->robot));

	targetJointPosition = solver->result;
	// rc->robot->insertVariables(targetJointPosition);
	rc->robot->goTo(targetJointPosition);
	rc->robot->isReady = false;
	previousPoint = rc->robot->endEffector.position;
}
double MoveCommand::calculateRequiredDistance(float dt){
	// return dt*velocity;
	auto targetVelocity = velocity;
	float distance = interpolator->distanceToEnd();
	if(distance < distanceTreshold){
		// velocity = 0.f;
		// velocity = distance/dt - acceleration*dt*0.5;
		// velocity = acceleration*dt*0.5; /// OK
		// velocity = sqrt(acceleration*dt*2);
		targetVelocity = glm::mix(0.0, velocity, distance/distanceTreshold );
	}
	// currentVelocity += (glm::clamp((targetVelocity - currentVelocity)/dt, - acceleration*5, acceleration)*dt + lastAdd)*0.5;
	// lastAdd = glm::clamp((targetVelocity - currentVelocity)/dt, - acceleration*5, acceleration)*dt;
	currentVelocity += glm::clamp(regulator(targetVelocity, currentVelocity, dt)/dt, -acceleration*5, acceleration)*dt;
	return dt*currentVelocity;
}
glm::vec4 MoveCommand::calculateNextPoint(float dt){
	requiredDistance = calculateRequiredDistance(dt);
	glm::vec4 newTarget = previousPoint;
	glm::vec4 oldTarget = previousPoint;
	u32 i = 0;
	while(requiredDistance > 0.0 && (not interpolator->finished)){
	// while(requiredDistance > glm::distance(oldTarget, newTarget) && (not interpolator->finished)){
		i++;
		newTarget = interpolator->nextPoint();
		requiredDistance -= glm::distance(previousPoint, newTarget);
		previousPoint = newTarget;
	}
	lastPathIterationdistance = glm::distance(previousPoint, oldTarget);
	lastPathIterationCount = i;

	return newTarget;
}
int MoveCommand::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
	return exitAction;
}
int MoveCommand::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	if(not rc->robot->isReady){
		rc->robot->goTo(dt, jointVelocityModifier);
		previousPoint = rc->robot->endEffector.position;
	}
	if(rc->robot->isReady && interpolator->finished){
		interpolator->reset();
		return exit(rc, scene);
	}
	else if(rc->robot->isReady){
		glm::vec4 newTarget = calculateNextPoint(dt);

		auto newO = glm::slerp(startOrientation, endOrientation, interpolator->getNormalizedPosition());

		solver->solve(Point{ newTarget, newO }, *(rc->robot));
		targetJointPosition = solver->result;
		// rc->robot->insertVariables(targetJointPosition);
		if(solver->succes) rc->robot->goTo(targetJointPosition);
		if(solver->succes) rc->robot->goTo(dt, jointVelocityModifier);
		// previousPoint = rc->robot->endEffector.position;
		// previousPoint = newTarget;
		return 0;
	}
	return 0;
}

void SingleJointMove::init(shared_ptr<RobotController> &rc){
	isRuning = true;
	rc->robot->goTo(targetJointPosition);
}
int SingleJointMove::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	if(not rc->robot->isReady){
		rc->robot->goTo(dt, jointVelocityModifier);
		return 0;
	}
	else return exit(rc, scene);
}
int SingleJointMove::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
	return exitAction;
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
int FollowObject::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){
	if(*target != pTarget){
		solver->solve(Point{ *target, glm::quat(0, 0, 0, 1) }, *(rc->robot));
		targetJointPosition = solver->result;
		rc->robot->goTo(targetJointPosition);
		pTarget = *target;
	}
	if(not rc->robot->isReady){
		rc->robot->goTo(dt, jointVelocityModifier);
		return 0;
	}
	else return exit(rc, scene);
}
int FollowObject::exit(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	isRuning = false;
	return exitAction;
}
vector<glm::vec4>& FollowObject::getPath(){
	return fakePath;
}
vector<glm::vec4>& FollowObject::getPolyline(){
	return fakePath;
}
