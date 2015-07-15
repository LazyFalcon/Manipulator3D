#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "MoveCommandBuilder.h"
#include "../Robot-Commands.h"
#include "../Widgets.h"
#include "../IInterpolator.h"

extern UI::IMGUI ui;

void incr(double &value){value += 0.01;}
void decr(double &value){value -= 0.01;}

#define FIELDWITHNAME(name, widget) vertical(ui.rect(120,20).text(name, "ui_10"s)();\
	horizontal(\
		widget\
	)\
   )

#define INCR(value) \
			ui.rect(15, 22).text("-", UI::CenterText)\
				.onRepeat([this]{incr(value);}, 25u)\
				.onlClick([this]{incr(value);})\
				(UI::Button);

#define DECR(value) \
			ui.rect(15, 22).text("-", UI::CenterText)\
				.onRepeat([this]{decr(value);}, 25u)\
				.onlClick([this]{decr(value);})\
				(UI::Button);

#define EDIT(value) \
			ui.rect(120,22).edit(value)();

DropdownPairWithCallback<double>(UI::AlignTop, 100, std::vector <pair<string, double>> velocities {
	{"0.1m/s", 0.1},
	{"0.15m/s", 0.15},
	{"0.25m/s", 0.25},
	{"0.5m/s", 0.5},
	{"1.0m/s", 1.0},
});
DropdownPairWithCallback<double>(UI::AlignTop, 100, std::vector <pair<string, double>> accelerations {
	{"0.1m/s^2", 0.1},
	{"0.15m/s^2", 0.15},
	{"0.25m/s^2", 0.25},
	{"0.5m/s^2", 0.5},
	{"1.0m/s^2", 1.0},
});
DropdownPairWithCallback<double>(UI::AlignTop, 100, std::vector <pair<string, double>> times {
	{"1s", 1000},
	{"2s", 2000},
	{"3s", 3000},
	{"5s", 5000},
	{"10s", 10000},
});
// DropdownPairWithCallback(UI::AlignTop, 100,std::vector <pair<string, shared_ptr<IInterpolator>>> times {});

void MoveCommandBuilderWidget::run(){
	ui.rect(120, 20).text("Move command editor")();
	editName();
	editVelocity();
	editAcceleration();
	editTime();
	editInterpolator();
	editSolver();
}

void MoveCommandBuilderWidget::editName(){
	FIELDWITHNAME("Name", ui.rect(120, 20).edit(moveCommandBuilder->moveCommand->name)(););
}

void MoveCommandBuilderWidget::editVelocity(){
	FIELDWITHNAME("Velocity",
		DECR(moveCommandBuilder->moveCommand->velocity);
		velocities.run([this](double val){moveCommandBuilder->velocity(val);});
		EDIT(moveCommandBuilder->moveCommand->velocity);
		INCR(moveCommandBuilder->moveCommand->velocity);
		);
}

void MoveCommandBuilderWidget::editAcceleration(){
	FIELDWITHNAME("Velocity",
		DECR(moveCommandBuilder->moveCommand->velocity);
		EDIT(moveCommandBuilder->moveCommand->velocity);
		INCR(moveCommandBuilder->moveCommand->velocity);
		);
}

void MoveCommandBuilderWidget::editTime(){
	FIELDWITHNAME("Acceleration",
		DECR(moveCommandBuilder->moveCommand->acceleration);
		EDIT(moveCommandBuilder->moveCommand->acceleration);
		INCR(moveCommandBuilder->moveCommand->acceleration);
		);
}

void MoveCommandBuilderWidget::editInterpolator(){
}

void MoveCommandBuilderWidget::editSolver(){
}

