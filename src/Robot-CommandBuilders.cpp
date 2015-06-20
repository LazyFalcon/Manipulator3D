#include <regex>
#include <Utils/IMGUI_V4.h>
#include "Robot-CommandBuilders.h"
#include "Widgets.h"
extern UI::IMGUI ui;

void incr(double &value){value += 0.01;}
void decr(double &value){value -= 0.01;}
void incr(float &value){value += 0.01f;}
void decr(float &value){value -= 0.01f;}

wxg::DropdownWithCallback <std::string>velocityDropdowns(UI::AlignTop, 100, {"0.25m/s", "0.5m/s", "1m/s", "100mm/s"});
wxg::DropdownWithCallback <std::string>accelerationDropdowns(UI::AlignTop, 100, {"0.25m/s", "0.5m/s", "1m/s", "100mm/s"});
wxg::DropdownWithCallback <std::string>timeDropdowns(UI::AlignTop, 100, {"1s", "2s", "3s", "4s", "5s", "6s"});

double parseVelocity(const std::string &word){
	std::smatch match;
	std::regex regex(R"(([\-]?[0-9]+[\.]?[0-9]*))");
	std::string units;
	double value;
	if(std::regex_search(word, match, regex)){
		value = std::stod(match[1]);
		units = match[2];
	}
	if(std::regex_search(units, match, std::regex(R"((.*)/(.*))"))){
		std::string num = match[1];
		std::string denum = match[2];

		double mod = 1.0;
		if(num == "m") mod = 1.0;
		else if(num == "cm") mod /= 100.0;
		else if(num == "mm") mod /= 1000.0;
		std::cout<<num<<std::endl;
		value *= mod;
	}
	return value;
}
double parseAcceleration(const std::string &word){
	std::smatch match;
	std::regex regex(R"(([\-]?[0-9]+[\.]?[0-9]*))");
	std::string units;
	double value;
	if(std::regex_search(word, match, regex)){
		value = std::stod(match[1]);
		units = match[2];
	}
	if(std::regex_search(units, match, std::regex(R"((.*)/(.*))"))){
		std::string num = match[1];
		std::string denum = match[2];

		double mod = 1.0;
		if(num == "m") mod = 1.0;
		else if(num == "cm") mod /= 100.0;
		else if(num == "mm") mod /= 1000.0;
		std::cout<<num<<std::endl;
		value *= mod;
	}
	return value;
}
float parseTime(const std::string &word){
	std::smatch match;
	std::regex regex(R"(([\-]?[0-9]+[\.]?[0-9]*))");
	std::string units;
	float value;
	if(std::regex_search(word, match, regex)){
		value = std::stod(match[1]);
		units = match[2];
	}

	double mod = 1.0;
	if(units == "s") mod = 1.0;
	else if(units == "ms") mod /= 100.0;
	else if(units == "min") mod /= 1000.0;
	value *= mod;

	return value;
}

void MoveBuilder::init(){
	velocityDropdowns.callback = [this](std::string val){velocity(val);};
	accelerationDropdowns.callback = [this](std::string val){acceleration(val);};
	timeDropdowns.callback = [this](std::string val){time(val);};
}
void MoveBuilder::widget(){
	vertical(
		ui.rect(120, 20).text("velocity")();
		horizontal(
			ui.rect(15, 22).text("-", UI::CenterText)
				.onRepeat([this]{decr(moveCommand->velocity);}, 25u)
				.onlClick([this]{decr(moveCommand->velocity);})
				(UI::Button);
			velocityDropdowns.run();
			ui.rect(120,22).edit(moveCommand->velocity, UI::TextToRight)();
			ui.rect(15, 22).text("+", UI::CenterText)
				.onRepeat([this]{incr(moveCommand->velocity);}, 25u)
				.onlClick([this]{incr(moveCommand->velocity);})
				(UI::Button);
		)
	)
	vertical(
		ui.rect(120, 20).text("acceleration")();
		horizontal(
			ui.rect(15, 22).text("-", UI::CenterText)
				.onRepeat([this]{decr(moveCommand->acceleration);}, 25u)
				.onlClick([this]{decr(moveCommand->acceleration);})
				(UI::Button);
			accelerationDropdowns.run();
			ui.rect(120,22).edit(moveCommand->acceleration, UI::TextToRight)();
			ui.rect(15, 22).text("+", UI::CenterText)
				.onRepeat([this]{incr(moveCommand->acceleration);}, 25u)
				.onlClick([this]{incr(moveCommand->acceleration);})
				(UI::Button);
		)
	)
	vertical(
		ui.rect(120, 20).text("time")();
		horizontal(
			ui.rect(15, 22).text("-", UI::CenterText)
				.onRepeat([this]{decr(moveCommand->time);}, 25u)
				.onlClick([this]{decr(moveCommand->time);})
				(UI::Button);
			timeDropdowns.run();
			ui.rect(120,22).edit(moveCommand->time, UI::TextToRight)();
			ui.rect(15, 22).text("+", UI::CenterText)
				.onRepeat([this]{incr(moveCommand->time);}, 25u)
				.onlClick([this]{incr(moveCommand->time);})
				(UI::Button);
		)
	)
}

CommandType CommandTypeArray[] = {Empty, Move, Wait, Conditional, ConditionalCall,};
std::string CommandTypeName[] = {"---", "Move", "Wait", "Conditional", "ConditionalCall",};
unique_ptr<ICommandBuilder> CommandBuilders[1];
