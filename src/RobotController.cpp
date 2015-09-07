#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <Utils/IMGUI_V4.h>
#include "Robot.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
#include "Graph.h"
#include "Editor/Editor.h"
#include "Editor/MoveCommandBuilder.h"
#include "Editor/ExecuteCommandBuilder.h"
#include "CFGParser.h"
#include "ResourceLoader.h"

#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";
#define NAM_END }
#define NAM_START {

extern UI::IMGUI ui;
extern shared_ptr<RobotController> RC;

extern float g_timeStep;

// static const float pi = 3.141592f;
static const float hpi = 0.5f * 3.141592f;
static const float pi2 = 2.f * 3.141592f;
// static const double dpi = 3.141592653589793;
static const double hdpi = 0.5 * 3.141592653589793;
static const double sqdpi = 3.141592653589793 * 3.141592653589793;
static const double dpi2 = 2.0 * 3.141592653589793;

void RCTest(RobotController &rc){}

void RobotController::run(){
	if (!commands.empty() && commandIter != commands.end()){
		state = RCStates::Run;
		cout<<endl;
		//commandIter = commands.begin();
		if(!(*commandIter)->isRuning){
			cout<<"------- NEW ACTION: "+(*commandIter)->name<<endl;
			(*commandIter)->init(RC);
			Editor::set(*commandIter);
		}
	}
	else stop();
}
void RobotController::pause(){
	state = RCStates::Pause;
}
void RobotController::stop(){
	state = RCStates::Stop;
	commandIter = commands.begin();
	pause();
}
void RobotController::next(){
	//if (std::next(commandIter) != commands.end() && commandIter != commands.end())
	commandIter++;
	if(commandIter == commands.end()){
		// commandIter--;
		stop();
	}
	else {
		Editor::set(*commandIter);
	}
}
void RobotController::prev(){
	if(commandIter != commands.begin()){
		commandIter--;
		Editor::set(*commandIter);
	}
}

bool RobotController::update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, float dt){

	if(commandIter == commands.end()){
		stop();
	}

	if(state == RCStates::Pause)
		return false;

	auto returnedAction = (*commandIter)->update(rc, scene, dt);
	if(returnedAction){
		cout<<"------- END OF ACTION: "+(*commandIter)->name<<endl;
		if(returnedAction == CommandReturnAction::DelAndForward){
			cout<<"------ DEL & FORWARD"<<endl;
			commandIter = commands.erase(commandIter);
		}
		else if(returnedAction == CommandReturnAction::DelAndBack){
			cout<<"------ DEL & BACK"<<endl;
			commandIter = commands.erase(commandIter);
			commandIter--;
			// commandIter++;
		}
		else {
			// next();
			commandIter++;
		}
		run();

		return true;
	}
	return false;
}

void RobotController::releaseObject(){}

void RobotController::grabObject(shared_ptr<Entity> &obj){
	MoveCommandBuilder moveBuilder;
	ExecuteCommandBuilder executeBuilder;
	auto interpolator = addInterpolator(Interpolator::Simple, {obj->position}, "Grab");
	auto target = addInterpolator(Interpolator::Simple, {glm::rotate(2.f, glm::vec3(0,0,1)) * obj->position}, "Grab2");
	std::cout<<"One wants grab object."<<std::endl;
	moveBuilder
		.init()
		.name("Move to target")
		.velocity(1.0)
		.jointVelocity(0.5)
		.acceleration(0.2)
		.solver(nullptr)
		.interpolator(interpolator)
		.finish(*this);

	executeBuilder
        .init()
        .name("Grab target")
		.onEnter([&obj, this](shared_ptr<RobotController> &rc){
			RCUtils::pinObjectToEffector(obj, rc->robot->chain.back()->entity);
            return true;
		})
		.finish(RC);

	moveBuilder
		.init()
        .name("Move target")
		.velocity(1.0)
		.jointVelocity(0.5)
		.acceleration(0.2)
		.solver(nullptr)
		.interpolator(target)
		.finish(*this);

    executeBuilder
        .init()
        .name("Release target")
		.onEnter([obj](shared_ptr<RobotController> &rc){
			RCUtils::releaseObjects();
            return true;
		})
		.finish(RC);

}

void RobotController::savePosition(){
	exec().name("Save position").onEnter([](shared_ptr<RobotController> &rc){
		auto &&vec = rc->robot->getVariables();
		rc->positionCache.push({"--", rc->robot->endEffector.position, rc->robot->endEffector.quat, rc->robot->getVariables()});
	}).finish(RC);

}
void RobotController::peekPosition(){
	exec().name("Peek position").onEnter([](shared_ptr<RobotController> &rc){
		if(rc->positionCache.empty()) return true;
		SingleJointMoveCommandBuilder sjmcb;
		sjmcb.init().name("Peek position").set(rc->positionCache.top().joints).jointVelocity(0.6).finish(rc);
	}).finish(RC);
}
void RobotController::popPosition(){
	exec().name("Pop position").onEnter([](shared_ptr<RobotController> &rc){
		if(rc->positionCache.empty()) return true;
		SingleJointMoveCommandBuilder sjmcb;
		sjmcb.init().name("Peek position").set(rc->positionCache.top().joints).jointVelocity(0.6).finish(rc);
		rc->positionCache.pop();
	}).finish(RC);
}

ExecuteCommandBuilder&  RobotController::grab(shared_ptr<Entity> &obj, int commandExitAction){
	return exec(commandExitAction).onEnter([obj, this](shared_ptr<RobotController> &rc){
		RCUtils::pinObjectToEffector(obj, rc->robot->chain.back()->entity);
	});
}
ExecuteCommandBuilder&  RobotController::release(int commandExitAction){
	return exec(commandExitAction).onEnter([](shared_ptr<RobotController> &rc){
		RCUtils::releaseObjects();
	});
}


namespace RCUtils NAM_START

/// musimy zapamiętać pozycję i orientację względem efektora
std::pair<shared_ptr<Entity>, shared_ptr<Entity>> pairedObjects;
Point effectorToPairedRelation;

void pinObjectToEffector(shared_ptr<Entity> obj, shared_ptr<Entity> &effector){
    if(obj && effector){
        pairedObjects = std::make_pair(obj, effector);
        effectorToPairedRelation.position = obj->position - effector->position;
        effectorToPairedRelation.quat = glm::inverse(effector->quat) * obj->quat;
    }
}
shared_ptr<Entity>& releaseObjects(){
    auto &out = pairedObjects.first;
    pairedObjects = make_pair<shared_ptr<Entity>, shared_ptr<Entity>>(nullptr, nullptr);
    return out;
}
void update(){
    if(pairedObjects.first && pairedObjects.second){
        auto obj = pairedObjects.first;
        auto effector = pairedObjects.second;
        obj->position = effector->position + effector->quat * effectorToPairedRelation.quat * effectorToPairedRelation.position;
        obj->quat = effector->quat * effectorToPairedRelation.quat;
        if(obj->rgBody){
			btTransform tr;
			tr.setRotation(btQuaternion(obj->quat.x, obj->quat.y, obj->quat.z, obj->quat.w));
			tr.setOrigin(btVector3(obj->position.x, obj->position.y, obj->position.z));

			obj->rgBody->setWorldTransform(tr);
        }


    }

}


NAM_END
