#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/IMGUI_V4.h>
#include "MoveCommandBuilder.h"
#include "Editor.h"
#include "../Robot-Commands.h"
#include "../Widgets.h"
#include "../IInterpolator.h"
#include "../RobotController.h"

void incr(double &value){value += 0.01;}
void decr(double &value){value -= 0.01;}
void incr(float &value){value += 0.01;}
void decr(float &value){value -= 0.01;}

#define FIELDWITHNAME(name, widget) vertical(ui.rect(120,17).text(name, "ui_10"s)();\
	horizontal(\
	ui.border(2);\
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
			ui.rect(120,22).edit(value)(UI::EditBox);


extern unique_ptr<RobotController> RC;

MoveCommandBuilder& MoveCommandBuilder::finish(){
	moveCommand->solver = make_unique<JT1>();
	RC->commands.push_back(moveCommand);
	init();
	return *this;
}

extern UI::IMGUI ui;
namespace Editor NAM_START

extern PolylineEditor polylineEditor;

wxg::DropdownPairWithCallback<double> velocities (UI::AlignTop, 100, std::vector <pair<string, double>>{
	{"0.1 m/s", 0.1},
	{"0.15 m/s", 0.15},
	{"0.25 m/s", 0.25},
	{"0.5 m/s", 0.5},
	{"1.0 m/s", 1.0},
	{"1.5 m/s", 1.5},
	{"2.0 m/s", 2.0},
	{"3.0 m/s", 3.0},
	{"4.0 m/s", 4.0},
});
wxg::DropdownPairWithCallback<double> jointVelocities (UI::AlignTop, 100, std::vector <pair<string, double>>{
	{"0.1 rad/s", 0.1},
	{"0.15 rad/s", 0.15},
	{"0.25 rad/s", 0.25},
	{"0.5 rad/s", 0.5},
	{"1.0 rad/s", 1.0},
	{"1.5 rad/s", 1.5},
	{"2.0 rad/s", 2.0},
	{"3.0 rad/s", 3.0},
	{"4.0 rad/s", 4.0},
});
wxg::DropdownPairWithCallback<double> accelerations (UI::AlignTop, 100, std::vector <pair<string, double>>{
	{"0.1 m/s^2", 0.1},
	{"0.15 m/s^2", 0.15},
	{"0.25 m/s^2", 0.25},
	{"0.5 m/s^2", 0.5},
	{"1.0 m/s^2", 1.0},
});
wxg::DropdownPairWithCallback<double> times (UI::AlignTop, 100, std::vector <pair<string, double>>{
	{"1s", 1000},
	{"2s", 2000},
	{"3s", 3000},
	{"5s", 5000},
	{"10s", 10000},
});
wxg::DropdownPairWithCallback<Interpolator> interpolatorTypes (UI::AlignTop, 100);
wxg::DropdownListWithCallback<shared_ptr<IInterpolator>> interpolatorFromList (UI::AlignTop, 100);

void MoveCommandBuilderWidget_inits(){
	interpolatorTypes.options = interpolatorEnumWithName;
	interpolatorTypes.value = interpolatorEnumWithName[0].second;
	interpolatorFromList.options = &getInterpolators();
	interpolatorFromList.value = *(getInterpolators().begin());
}
void MoveCommandBuilderWidget::enter(){
	Editor::polylineEditor.set(moveCommandBuilder->moveCommand->interpolator);
}
void MoveCommandBuilderWidget::run(){
	ui.rect(120, 20).text("Move command editor")();
	editName();
	editVelocity();
	editAcceleration();
	editTime();
	editInterpolator();
	editSolver();
	finalize();
}
void MoveCommandBuilderWidget::editName(){
	FIELDWITHNAME("Name", EDIT(moveCommandBuilder->moveCommand->name););
}
void MoveCommandBuilderWidget::editVelocity(){
	FIELDWITHNAME("Velocity",
		DECR(moveCommandBuilder->moveCommand->velocity);
		velocities.run([this](double val){moveCommandBuilder->velocity(val);});
		EDIT(moveCommandBuilder->moveCommand->velocity);
		INCR(moveCommandBuilder->moveCommand->velocity);
		);
}
void MoveCommandBuilderWidget::editJointVelocity(){
	FIELDWITHNAME("JointVelocity",
		DECR(moveCommandBuilder->moveCommand->jointVelocity);
		jointVelocities.run([this](double val){moveCommandBuilder->jointVelocity(val);});
		EDIT(moveCommandBuilder->moveCommand->jointVelocity);
		INCR(moveCommandBuilder->moveCommand->jointVelocity);
		);
}
void MoveCommandBuilderWidget::editAcceleration(){
	FIELDWITHNAME("Acceleration",
		DECR(moveCommandBuilder->moveCommand->acceleration);
		accelerations.run([this](double val){moveCommandBuilder->acceleration(val);});
		EDIT(moveCommandBuilder->moveCommand->acceleration);
		INCR(moveCommandBuilder->moveCommand->acceleration);
		);
}
void MoveCommandBuilderWidget::editTime(){
	FIELDWITHNAME("Time",
		DECR(moveCommandBuilder->moveCommand->time);
		times.run([this](double val){moveCommandBuilder->time(val);});
		EDIT(moveCommandBuilder->moveCommand->time);
		INCR(moveCommandBuilder->moveCommand->time);
		);
}
void MoveCommandBuilderWidget::editInterpolator(){
	FIELDWITHNAME("Interpolator",
		// interpolatorTypes.run([this](Interpolator val){moveCommandBuilder->interpolator(val);});
		interpolatorFromList.run([this](shared_ptr<IInterpolator> val){moveCommandBuilder->interpolator(val);});
		ui.rect(110,22).text(interpolatorFromList.getSelectedName())(UI::EditBox);
		ui.rect(10,22).text("+", UI::CenterText)(UI::Button);
	);
}
void MoveCommandBuilderWidget::editSolver(){
	FIELDWITHNAME("Solver",
		ui.rect(130,22).text("Transposed Jacobian", UI::CenterText)();
	);
}
void MoveCommandBuilderWidget::finalize(){
	// ui.rect(130, 30).text("Add new command", UI::CenterText)(UI::Button)
		// .onlClick([this]{
			// moveCommandBuilder->finish();
		// });
}


NAM_END
