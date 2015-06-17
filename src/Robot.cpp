#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include "common.h"
#include "Robot.h"

extern const float pi;
extern PositionSaver g_robotPositions;

std::vector<glm::vec4> Robot::forward(){
	for(auto &module : chain){
		
		
	}
	
}


// axis(quat): oś
// angle(quat): kąt
// angleAxis(angle, vec3 axis): quat
// rotate(quat, angle, axis)
void Robot::update(float dt){
	
	glm::vec4 position = glm::vec4(0,0,0,1);
	glm::vec3 axis = chain[0]->axis.xyz();
	// glm::quat transform(1,0,0,0);
	glm::quat transform{0,0,0,1};
	for(auto &module : chain){
		if(module->type == HINGE){
			module->value = period(module->value, -2*pi, 2*pi);
			// module->value = glm::clamp(module->value, module->min, module->max);
			transform = transform*glm::angleAxis(module->value, glm::normalize(module->axis.xyz()));
			position += transform*module->vecToA;
		}
		else if(module->type == PRISMATIC){
			// position += transform*(module->vecToA + module->axis*module->value);
		}
		module->entity->quat = transform;
		module->entity->position = position;
		axis = transform*module->axis.xyz();
		position += transform*module->vecToB;
		
	}
	endEffector = Point {position, glm::angleAxis(0.f,glm::normalize(axis.xyz()))};
	
	g_robotPositions(position);
}

std::vector<float> Robot::getVariables(){
	std::vector<float> out(getSize());
	for(int i=0; i<getSize(); i++)
		out[i] = chain[i]->value;
	return out;
}
Point Robot::simulate(std::vector<double> &variables){
	glm::vec4 position = glm::vec4(0,0,0,1);
	glm::vec3 axis = chain[0]->axis.xyz();
	// glm::quat transform(1,0,0,0);
	glm::quat transform{0,0,0,1};
	for(int i=0; i<getSize(); i++){
		auto module = chain[i];
		if(module->type == HINGE){
			variables[i] = period(variables[i], -2*pi, 2*pi);
			transform = transform*glm::angleAxis((float)variables[i], glm::normalize(module->axis.xyz()));
			position += transform*module->vecToA;
		}
		else if(module->type == PRISMATIC){
			position += transform*(module->vecToA + module->axis*(float)variables[i]);
		}
		axis = transform*module->axis.xyz();
		position += transform*module->vecToB;
		
	}
	return {position, glm::angleAxis(0.f,glm::normalize(axis.xyz()))};
}
std::vector<Point> Robot::simulateFullData(std::vector<double> &variables){
	glm::vec4 position = glm::vec4(0,0,0,1);
	glm::vec3 axis = chain[0]->axis.xyz();
	// glm::quat transform(1,0,0,0);
	std::vector<Point> out(variables.size());
	glm::quat transform{0,0,0,1};
	for(int i=0; i<getSize(); i++){
		auto module = chain[i];
		if(module->type == HINGE){
			module->value = period(module->value, 0.f, 2*pi);
			transform = transform*glm::angleAxis((float)variables[i], glm::normalize(module->axis.xyz()));
			position += transform*module->vecToA;
		}
		else if(module->type == PRISMATIC){
			position += transform*(module->vecToA + module->axis*(float)variables[i]);
		}
		axis = transform*module->axis.xyz();
		position += transform*module->vecToB;
		out[i] = Point{};
		
	}
	return out;
}
glm::vec4 Robot::clamp(std::vector<double> &vec){
	
	for(int i=0; i<getSize() && i<vec.size(); i++){
		auto module = chain[i];
		vec[i] = glm::clamp((float)vec[i], module->min, module->max);
			// vec[i] = glm::clamp((float)vec[i], -pi, pi);
	}
}
glm::vec4 Robot::insertVariables(std::vector<double> &vec){
	for(int i=0; i<getSize() && i<vec.size(); i++){
		auto module = chain[i];
		module->value = vec[i];
	}
}



