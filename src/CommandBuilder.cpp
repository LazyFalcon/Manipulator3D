#include <Utils/IMGUI_V4.h>
#include "CommandBuilder.h"
extern UI::IMGUI ui;

void incr(double &value){value += 0.01;}
void decr(double &value){value -= 0.01;}
void incr(float &value){value += 0.01f;}
void decr(float &value){value -= 0.01f;}

void MoveBuilder::widget(){
	vertical(
	horizontal(
		ui.rect(5, 22).text(u"-").onlClick([this]{decr(moveCommand->velocity);})();
		ui.rect()();
		ui.rect(5, 22).text(u"+").onlClick([this]{incr(moveCommand->velocity);})();
	)
	// wgx::editbox("velocity: ", [this](double value){velocity(value);});
	)
}

CommandType CommandTypeArray[] = {Empty, Move, Wait, Conditional, ConditionalCall,};
std::string CommandTypeName[] = {"---", "Move", "Wait", "Conditional", "ConditionalCall",};
unique_ptr<ICommandBuilder> CommandBuilders[1];
