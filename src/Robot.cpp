#include "Utils/Includes.h"
#include "Utils/Utils.h"
#include "Utils/BaseStructs.h"
#include "common.h"
#include "Robot.h"
#include "Helper.h"

extern const float pi;
static const float hpi = 0.5f * 3.141592f;
extern const double dpi;
extern PositionSaver g_robotPositions;
const double jointEpsilon = 1.0*dpi/60.0/180.0; /// one minute in radians

void Robot::update(float dt){

	glm::vec4 position = basePosition;
	glm::vec3 axis = chain[0]->axis.xyz();
	// glm::quat transform(1,0,0,0);
	glm::quat transform{0,0,0,1};
	for(auto &module : chain){
		if(module->type == REVOLUTE_JOINT){
			// module->value = period(module->value);
			// module->value = glm::clamp(module->value, module->min, module->max);
			transform = transform*glm::angleAxis(float(module->value), glm::normalize(module->axis.xyz()));
			position += transform*module->vecToA;
		}
		else if(module->type == PRISMATIC_JOINT){
			position += transform*(module->vecToA + module->axis*float(module->value));
		}
		module->entity->quat = transform;
		module->entity->position = position;

		if(module->entity->rgBody && config.usePhysics){
			btTransform tr;
			tr.setRotation(btQuaternion(transform.x, transform.y, transform.z, transform.w));
			tr.setOrigin(btVector3(position.x, position.y, position.z));

			// module->entity->rgBody->setWorldTransform(tr);
			module->entity->rgBody->getMotionState()->setWorldTransform(tr);
			module->entity->rgBody->setCenterOfMassTransform(tr);
		}

		axis = transform*module->axis.xyz();
		position += transform*module->vecToB;
	}

	auto positionShift = glm::distance(position, endEffector.position);
	endEffector = Point {position, glm::angleAxis(1.f,glm::normalize(axis.xyz()))};

	Helper::record().EffectorPosition = position;
	Helper::record().EffectorOrientation = glm::angleAxis(1.f,glm::normalize(axis.xyz()));
	Helper::record().EffectorDelta = positionShift;
	Helper::record().EffectorAcceleration = (positionShift/dt - Helper::record().EffectorVelocity)/dt;
	Helper::record().EffectorVelocity = positionShift/dt;
}

Point Robot::simulate(std::vector<double> &variables){
	glm::vec4 position = basePosition;
	glm::vec3 axis = chain[0]->axis.xyz();
	// glm::quat transform(1,0,0,0);
	glm::quat transform{0,0,0,1};
	for(int i=0; i<getSize(); i++){
		auto &module = chain[i];
		if(module->type == REVOLUTE_JOINT){
			// variables[i] = period(variables[i]);
			transform = transform*glm::angleAxis((float)variables[i], glm::normalize(module->axis.xyz()));
			position += transform*module->vecToA;
		}
		else if(module->type == PRISMATIC_JOINT){
			position += transform*(module->vecToA + module->axis*(float)variables[i]);
		}
		axis = transform*module->axis.xyz();
		position += transform*module->vecToB;

	}
	return {position, glm::angleAxis(1.f,glm::normalize(axis.xyz()))};
}

std::vector<double> Robot::getVariables(){
	std::vector<double> out(getSize());
	for(int i=0; i<getSize(); i++)
		out[i] = chain[i]->value;
	return out;
}

std::vector<Point> Robot::simulateFullData(std::vector<double> &variables){
	glm::vec4 position = basePosition;
	glm::vec3 axis = chain[0]->axis.xyz();
	// glm::quat transform(1,0,0,0);
	std::vector<Point> out(variables.size());
	glm::quat transform{0,0,0,1};
	for(int i=0; i<getSize(); i++){
		auto &module = chain[i];
		if(module->type == REVOLUTE_JOINT){
			// module->value = period(module->value);
			transform = transform*glm::angleAxis((float)variables[i], glm::normalize(module->axis.xyz()));
			position += transform*module->vecToA;
		}
		else if(module->type == PRISMATIC_JOINT){
			position += transform*(module->vecToA + module->axis*(float)variables[i]);
		}
		axis = transform*module->axis.xyz();
		position += transform*module->vecToB;
		out[i] = Point{};

	}
	return out;
}
void Robot::clamp(std::vector<double> &vec){
	if(not config.useRobotConstraints) return;
	for(int i=0; i<getSize() && i<vec.size(); i++){
		auto &module = chain[i];
		vec[i] = glm::clamp(vec[i], module->min, module->max);
	}
}
glm::vec4 Robot::insertVariables(std::vector<double> &vec){
	for(int i=0; i<getSize() && i<vec.size(); i++){
		auto &module = chain[i];
		module->value = vec[i];
	}
}

bool Robot::goTo(const std::vector<double> &jointPositions){
	u32 loopSize = std::min(chain.size(), jointPositions.size());
	isReady = false;
	for(u32 i=0; i<loopSize; i++){
		auto delta = circleDistance(jointPositions[i], chain[i]->value);
		chain[i]->targetValue = delta;
	}
}

bool Robot::goTo(float dt, double jVelocityModifier){
	isReady = true;
	for(auto &module : chain){
		isReady &= module->goTo(dt, jVelocityModifier);
	}
}

double Module::computeMaxStep(float dt, double jVelocityModifier){
	double step = dt * maxVelocty * jVelocityModifier;
	return step;
}
bool Module::goTo(float dt, double jVelocityModifier){
	if(glm::epsilonEqual(targetValue, 0.0, jointEpsilon)){
		return true;
	}
	auto maxStep = computeMaxStep(dt, jVelocityModifier);

	auto step = glm::clamp(targetValue, -maxStep, maxStep);
	value += step;
	targetValue -= step;

	auto lVel = lastVelocity;
	lastVelocity = step/dt;
	lastAcceleration = (lastVelocity - lVel)/dt;

	if(glm::epsilonEqual(targetValue, 0.0, jointEpsilon)){
		return true;
	}

	return false;
}

std::vector<double> targetVariables;
void robotTestInit(Robot &robot){
	targetVariables = robot.getVariables();
	for(auto &it : targetVariables)
		it += 0.5;
	// robot.isReady = false;
}
void robotTest(float dt, Robot &robot){
	if(robot.isReady) return;

	// robot.goTo(targetVariables, dt/1000);


}

void Robot::reset(){
	for(auto &module : chain)
		module->reset();
}