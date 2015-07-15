#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "MoveCommandBuilder.h"
#include "../Robot-Commands.h"
#include "../Widgets.h"

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

