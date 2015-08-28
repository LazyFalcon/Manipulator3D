#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <Utils/IMGUI_V4.h>
#include "Robot.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
#include "Robot-Commands.h"
vector <glm::vec4> fakePath;
u32 globalFlags = 0;
u32 lastPathIterationCount;
float lastPathIterationdistance;


void WaitCommand::init(RobotController &rc){
	isRuning = true;
};
bool WaitCommand::update(RobotController &rc, float dt){
	// if(releaseTime <= 0.f || releaseFlag&globalFlags || (releaseFuction && releaseFuction()))
	if(releaseTime <= 0.f || releaseFlag&globalFlags)
		return true;

	releaseTime -= dt;
	time += dt;
	return false;
}
bool WaitCommand::exit(RobotController &rc){}
vector<glm::vec4>& WaitCommand::getPath(){
	return fakePath;
}
vector<glm::vec4>& WaitCommand::getPolyline(){
	return fakePath;
}

void ExecuteCommand::init(RobotController &rc){
	isRuning = true;
	if(onEnter) onEnter(rc);
};
bool ExecuteCommand::update(RobotController &rc, float dt){
	if(onUpdate) onUpdate(rc);
    else return exit(rc);
    return false;
}
bool ExecuteCommand::exit(RobotController &rc){
	isRuning = false;
    if(onExit) onExit(rc);
    return true;
}
vector<glm::vec4>& ExecuteCommand::getPath(){
	return fakePath;
}
vector<glm::vec4>& ExecuteCommand::getPolyline(){
	return fakePath;
}

void MoveCommand::init(RobotController &rc){
	requiredDistance = 0;
	isRuning = true;
	solver->solve(Point{ interpolator->firstPoint(), glm::quat(0, 0, 0, 1) }, *rc.robot);
	targetJointPosition = solver->result;
	rc.robot->isReady = false;
	// rc.robot->goTo(targetJointPosition);
	std::cout<<"Init command"<<std::endl;

	previousPoint = rc.robot->endEffector.position;
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
bool MoveCommand::update(RobotController &rc, float dt){
    // rc.robot->endEffectorVelocity = lastPathIterationdistance;
    // rc.robot->isReady = true;
	if(not rc.robot->isReady){
		rc.robot->goTo(dt, jointVelocityModifier);
		previousPoint = rc.robot->endEffector.position;
	}
    if(rc.robot->isReady && interpolator->finished){
		interpolator->reset();
		return true;
	}
    else if(rc.robot->isReady){
        glm::vec4 newTarget = calculateNextPoint(dt);

        solver->solve(Point{ newTarget, glm::quat(0, 0, 0, 1) }, *rc.robot);
        targetJointPosition = solver->result;
        rc.robot->insertVariables(targetJointPosition);
        rc.robot->goTo(targetJointPosition);
        rc.robot->goTo(dt, jointVelocityModifier);
        previousPoint = rc.robot->endEffector.position;
        return false;
    }
    return false;
}

void SingleJointMove::init(RobotController &rc){
	isRuning = true;
	rc.robot->goTo(targetJointPosition);
}
bool SingleJointMove::update(RobotController &rc, float dt){
	if(not rc.robot->isReady){
		rc.robot->goTo(dt, jointVelocityModifier);
		return false;
	}
	else return exit(rc);
}
bool SingleJointMove::exit(RobotController &rc){
	isRuning = false;
	return true;
}
vector<glm::vec4>& SingleJointMove::getPath(){
	return fakePath;
}
vector<glm::vec4>& SingleJointMove::getPolyline(){
	return fakePath;
}

void FollowObject::init(RobotController &rc){
	isRuning = true;
	solver->solve(Point{ *target, glm::quat(0, 0, 0, 1) }, *rc.robot);
	targetJointPosition = solver->result;
	rc.robot->goTo(targetJointPosition);
}
bool FollowObject::update(RobotController &rc, float dt){
    if(*target != pTarget){
        solver->solve(Point{ *target, glm::quat(0, 0, 0, 1) }, *rc.robot);
        targetJointPosition = solver->result;
        rc.robot->goTo(targetJointPosition);
        pTarget = *target;
    }
	if(not rc.robot->isReady){
		rc.robot->goTo(dt, jointVelocityModifier);
		return false;
	}
	else return exit(rc);
}
bool FollowObject::exit(RobotController &rc){
	isRuning = false;
	return true;
}
vector<glm::vec4>& FollowObject::getPath(){
	return fakePath;
}
vector<glm::vec4>& FollowObject::getPolyline(){
	return fakePath;
}
