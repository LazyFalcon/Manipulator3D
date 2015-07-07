#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <Utils/IMGUI_V4.h>
#include "Robot.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
extern UI::IMGUI ui;
/*
	Robot ma w IK wbudowane sledzenie punktu z okresloną predkością, nie udaje się do zadanego punktu od razu. wiec jak interpolator wypluje kolejny punkt robot dojedzie do niego
	- ze "stałą" prędkością(po prostej)
	- wyznaczy ik tego punktu i poleci interpolacją złączową( choć bedzie mu to trudno zrobić(musiałby co klatkę liczyć ik tego punktu, w sumie dałoby to lepsze przybliżenie tego punktu) )


*/
extern float g_timeStep;

void RCTest(RobotController &rc){
	glm::vec4 p0(1, 6, 3, 1);
	glm::vec4 p1(4, 0, 5, 1);
	glm::vec4 p2(1, -6, 2, 1);
	glm::vec4 p3(0, -6, 3, 1);
	glm::vec4 p4(-1, -6, 2, 1);
	glm::vec4 p5(-1, -6, 1.9, 1);

	std::cout<<"Start test"<<std::endl;
	rc.move(new HermiteFiniteDifference({p0, p1, p2, p3, p4, p5}), "move 4");

}

//TODO: Całą odpowiedzialność za wykonanie akcji i tworzenie przerzucić do commandsów, robotControler niech zajmuje się tylko przechowywaniem commandsów, i interfejsowaniem pomiedzy command a userem, i chuj

// załóżmy iż obsługujemy tylko tkomendę move, pomijamy tu pierdoły typu przyspieszenia, prędkości itp. (dojdą później)
void RobotController::run(){
	if (!commands.empty()){
		state = RCStates::Run;
		//commandIter = commands.begin();
		if(!(*commandIter)->isRuning){
			(*commandIter)->init(*this);
			logger::log<<"Wielka czarna dupa!"<<std::endl;

		}
	}
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
		if (commandIter == commands.end())
			stop();
}
void RobotController::prev(){
	if (commandIter != commands.begin())
		commandIter--;
}
MoveCommand& RobotController::move(IInterpolator *interpolator, const std::string &name){
	MoveCommand *newCommand = new MoveCommand(interpolator);
	newCommand->name = name;
	newCommand->isRuning = false;
	commands.emplace_back(newCommand);

	if (commandIter == commands.end())
		commandIter = commands.begin();

	newCommand->velocity = 135.8;
	newCommand->solver = new  JT1();

	return *newCommand;
}
WaitCommand& RobotController::wait(float time){
	WaitCommand *newCommand = new WaitCommand(time);
	newCommand->name = "wait: " + std::to_string((int)time/1000)+"s";
	newCommand->isRuning = false;
	commands.push_back(std::unique_ptr<ICommand>(newCommand));

	if (commandIter == commands.end())
		commandIter = commands.begin();

	return *newCommand;
}

bool WaitCommand::update(RobotController &rc, float dt){
	timeLeft -= dt;
	return timeLeft < 0.f;
}

bool RobotController::update(float dt){

	if(commandIter == commands.end()){
		stop();
	}

	if(state == RCStates::Pause)
		return false;

	if((*commandIter)->update(*this, dt)){
		++commandIter;
		(*commandIter)->init(*this);
		if (commandIter == commands.end()){
			stop();
		}
		return true;
	}
	return false;
}


void MoveCommand::init(RobotController &rc){
	isRuning = true;
	solver->solve(Point{ interpolator->firstPoint(), glm::quat(0, 0, 0, 1) }, *rc.robot);
	targetJointPosition = solver->result;
	rc.robot->isReady = false;
	std::cout<<"Init command"<<std::endl;


	previousPoint = rc.robot->endEffector.position;
	rc.robot->insertVariables(targetJointPosition);
}
double MoveCommand::calculateRequiredDistance(float dt){
	return dt*velocity;
}
glm::vec4 MoveCommand::calculateNextPoint(float dt){
	// requiredDistance += calculateRequiredDistance(dt);
	requiredDistance = calculateRequiredDistance(dt);
	glm::vec4 newTarget;

	while(requiredDistance > 0.0 && (not interpolator->finished)){
		newTarget = interpolator->nextPoint();
		requiredDistance -= glm::distance(previousPoint, newTarget);
	}

	return newTarget;
}
bool MoveCommand::update(RobotController &rc, float dt){

	// previousPoint = rc.robot->endEffector.position;

	if(not rc.robot->isReady){
		rc.robot->goTo(dt);
		previousPoint = rc.robot->endEffector.position;
		// rc.robot->insertVariables(targetJointPosition);
		return false;
	}
	// if(rc.robot->isReady && interpolator->finished){
		// interpolator->reset();
		// logger::log<<"job done"<<endl;
		// return true;
	// }
	glm::vec4 newTarget = calculateNextPoint(dt);

	// std::cout<<glm::to_string(newTarget)<<std::endl;

	solver->solve(Point{ newTarget, glm::quat(0, 0, 0, 1) }, *rc.robot);
	targetJointPosition = solver->result;
	rc.robot->goTo(targetJointPosition);

	return false;
}



