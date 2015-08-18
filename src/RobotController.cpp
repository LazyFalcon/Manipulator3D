﻿#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include <Utils/IMGUI_V4.h>
#include "Robot.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
#include "Graph.h"
#include "Editor/Editor.h"

extern UI::IMGUI ui;
/*
	Robot ma w IK wbudowane sledzenie punktu z okresloną predkością, nie udaje się do zadanego punktu od razu. wiec jak interpolator wypluje kolejny punkt robot dojedzie do niego
	- ze "stałą" prędkością(po prostej)
	- wyznaczy ik tego punktu i poleci interpolacją złączową( choć bedzie mu to trudno zrobić(musiałby co klatkę liczyć ik tego punktu, w sumie dałoby to lepsze przybliżenie tego punktu) )


*/
extern float g_timeStep;

// static const float pi = 3.141592f;
static const float hpi = 0.5f * 3.141592f;
static const float pi2 = 2.f * 3.141592f;
// static const double dpi = 3.141592653589793;
static const double hdpi = 0.5 * 3.141592653589793;
static const double sqdpi = 3.141592653589793 * 3.141592653589793;
static const double dpi2 = 2.0 * 3.141592653589793;

Graph var0("var1", Graph::LastX, 0xFF0000FF, 250);
Graph var1("var1", Graph::LastX, 0x00FF00FF, 250);
Graph var2("var1", Graph::LastX, 0x0000FFFF, 250);
Graph var3("var1", Graph::LastX, 0xFFFF00FF, 250);
Graph var4("var1", Graph::LastX, 0xFF00FFFF, 250);
Graph var5("var1", Graph::LastX, 0xFFFFFFFF, 250);
Graph boundUp("var1", Graph::LastX, 0x202020FF, 250);
Graph boundDown("var1", Graph::LastX, 0x202020FF, 250);
Graph zero("var1", Graph::LastX, 0x202020FF, 250);
extern Plot mainPlot;

void RCTest(RobotController &rc){
	glm::vec4 p0(2, 5, 4, 1);
	glm::vec4 p1(4, 0, 5, 1);
	glm::vec4 p2(1, -5, 2, 1);
	glm::vec4 p3(0, -5, 3, 1);
	glm::vec4 p4(-1, -5, 2, 1);
	glm::vec4 p5(-1, -5.3, 1.9, 1);
	glm::vec4 p6(-4, -4, 1.9, 1);

	std::cout<<"Start test"<<std::endl;
	// rc.move(new HermiteFiniteDifference({p0, p1, p2, p3, p4, p5, p6}), "move 4");
	// rc.move(addInterpolator(Interpolator::HermiteFiniteDifferenceClosed, {p0, p1, p2, p3, p4, p5, p6}), "First move");
	rc.move(addInterpolator(Interpolator::HermiteFiniteDifference, {p0, p1, p2, p3, p4, p5, p6}), "First move");
	rc.move(addInterpolator(Interpolator::HermiteFiniteDifferenceClosed, {p0, p3, p6, p1, p4, p5, p2}), "Second move");
	// rc.wait(20);
	addInterpolator(Interpolator::HermiteFiniteDifferenceClosed, {p0, p3, p6})->name = "Hermite Closed";
	addInterpolator(Interpolator::HermiteCardinal, {p0, p3, p6})->name = "Hermite Cardinal";

	var0.setBouds({0, 250, -pi2, pi2});
	var1.setBouds({0, 250, -pi2, pi2});
	var2.setBouds({0, 250, -pi2, pi2});
	var3.setBouds({0, 250, -pi2, pi2});
	var4.setBouds({0, 250, -pi2, pi2});
	var5.setBouds({0, 250, -pi2, pi2});
	boundUp.setBouds({0, 250, -pi2, pi2});
	boundDown.setBouds({0, 250, -pi2, pi2});
	zero.setBouds({0, 250, -pi2, pi2});
	mainPlot.push(&var0);
	mainPlot.push(&var1);
	mainPlot.push(&var2);
	mainPlot.push(&var3);
	mainPlot.push(&var4);
	mainPlot.push(&var5);
	mainPlot.push(&boundUp);
	mainPlot.push(&boundDown);
	mainPlot.push(&zero);
	// mainPlot.push(&jacobianPrecision);
	plotList.push_front(&mainPlot);

}


void RobotController::run(){
	if (!commands.empty()){
		state = RCStates::Run;
		//commandIter = commands.begin();
		if(!(*commandIter)->isRuning){
			(*commandIter)->init(*this);
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
	if(commandIter == commands.end()){
		stop();
	}
	else {
		Editor::set(*commandIter);
	}
}
void RobotController::prev(){
	if(commandIter != commands.begin()){
		commandIter--;

	}
}
MoveCommand& RobotController::move(shared_ptr<IInterpolator> interpolator, const std::string &name){
	MoveCommand *newCommand = new MoveCommand(interpolator);
	interpolator->name = name;
	newCommand->name = name;
	newCommand->isRuning = false;
	commands.emplace_back(newCommand);

	if (commandIter == commands.end())
		commandIter = commands.begin();

	newCommand->velocity = 1.8;
	newCommand->acceleration = 1.8;
	newCommand->solver = make_unique<JT0>();

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





