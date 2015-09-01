#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/Camera.h>
#include <Utils/IMGUI_V4.h>
#include "common.h"
#include "Robot.h"
#include "JacobianTransposed.h"
#include "Timer.h"
#include "JacobianMatrix.h"

#include "Graph.h"

extern UI::IMGUI ui;
extern Camera camera;
extern float	 mouse_x, mouse_y;

extern int robotPositionsCounter;
extern int robotPositionsMax;
extern vector<glm::vec4> robotPositions;
extern PositionSaver g_mousePositions;
extern PositionSaver g_robotPositions;
extern PositionSaver g_robotDebugPositions;
extern std::list<Plot*> plotList;

// static const float pi = 3.141592f;
static const float hpi = 0.5f * 3.141592f;
static const float pi2 = 2.f * 3.141592f;
// static const double dpi = 3.141592653589793;
static const double hdpi = 0.5 * 3.141592653589793;
static const double sqdpi = 3.141592653589793 * 3.141592653589793;
static const double dpi2 = 2.0 * 3.141592653589793;

u32 lastIterationCount;

glm::vec4 g_targetPosition;

Graph jacobianIterations("jacobianIterations", Graph::LastX, 0xFFFF00FF, 250);
Graph jacobianPrecision("jacobianPrecision", Graph::LastX, 0xFF4000FF, 250);

void cutHigherThanPi(vector <double> &vec){
	for(auto &v : vec){
		if(v > dpi2){
			v -= dpi2 * floor(v/dpi2);
		}
		else if(v < -dpi2){
			v -= dpi2 * floor(v/dpi2);
		}

	}
}

bool JT0::solve(Point target, Robot &robot){
	float maxDistanceInSingleStep = 0.1f; /// m
	auto subTarget = robot.endEffector.position;
	auto totalDistance = glm::distance(target.position, robot.endEffector.position);
	auto distance = totalDistance;
	auto vecToTarget = glm::normalize(target.position - robot.endEffector.position);

	float minR = std::min(glm::distance(target.position, glm::vec4(0,0,0,1)), glm::distance(robot.endEffector.position, glm::vec4(0,0,0,1)));

	auto currentPosition = robot.endEffector.position;
	auto tmpJoints = robot.getVariables();
	u32 iterations = 0;
	while(distance >= maxDistanceInSingleStep){
		iterations++;
		distance -= maxDistanceInSingleStep;
		auto param = distance/totalDistance;
		subTarget += vecToTarget * maxDistanceInSingleStep;

		auto subTargetOnSphere = glm::normalize(subTarget - glm::vec4(0,0,0,1)) * std::max(glm::length(subTarget), minR);
		subTargetOnSphere.w = 1;

		performIK({currentPosition, robot.endEffector.quat} , {subTargetOnSphere, target.quat}, robot);
		currentPosition = subTargetOnSphere;
		robot.insertVariables(result);
	}

	performIK({currentPosition, robot.endEffector.quat}, target, robot, 0.0001);

	robot.insertVariables(tmpJoints);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT0::performIK(Point start, Point target, Robot &robot, double precision){
	int iterationLimit = 2000;
	float minError = precision;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	// double i = 0.2;
	double i = 1.5;
	// for(auto &it : variables.getVector()){
		// it = 0.0;
	// }
	for(auto &it : enhancement.getVector()){
		it = i;
		// it = 1;
		i -= 0.2;
		// i += 0.9;
	}
	// enhancement(3) *= 15;
	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);
	float quatError = 1;
	auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
	auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6
	u32 iterations = 0;
	for(; (positionError > minError || quatError > minError*10) && iterations<iterationLimit; iterations++){
		auto positionDelta = (target.position - endEffector.position);
		auto axisDelta = -glm::cross(glm::axis(target.quat), glm::axis(endEffector.quat))*0.f;

		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, axisDelta.x, axisDelta.y, axisDelta.z});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		// robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
		quatError = glm::length(axisDelta);
	}
	// cout<<iterations<<" << "<<quatError<<endl;
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < minError;
	lastIterationCount = iterations;
	return succes;
}

bool JT1::solve(Point target, Robot &robot){
	float maxDistanceInSingleStep = 0.1f; /// m
	auto subTarget = robot.endEffector.position;
	auto totalDistance = glm::distance(target.position, robot.endEffector.position);
	auto distance = totalDistance;
	auto vecToTarget = glm::normalize(target.position - robot.endEffector.position);

	float minR = std::min(glm::distance(target.position, glm::vec4(0,0,0,1)), glm::distance(robot.endEffector.position, glm::vec4(0,0,0,1)));

	auto currentPosition = robot.endEffector.position;
	auto tmpJoints = robot.getVariables();
	u32 iterations = 0;
	while(distance >= maxDistanceInSingleStep){
		iterations++;
		distance -= maxDistanceInSingleStep;
		auto param = distance/totalDistance;
		subTarget += vecToTarget * maxDistanceInSingleStep;

		auto subTargetOnSphere = glm::normalize(subTarget - glm::vec4(0,0,0,1)) * std::max(glm::length(subTarget), minR);
		subTargetOnSphere.w = 1;

		performIK({currentPosition, robot.endEffector.quat} , {subTargetOnSphere, target.quat}, robot);
		currentPosition = subTargetOnSphere;
		robot.insertVariables(result);
	}
	performIK({currentPosition, robot.endEffector.quat}, target, robot);

	robot.insertVariables(tmpJoints);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT1::performIK(Point start, Point target, Robot &robot, double precision){
	int iterationLimit = 2000;
	float minError = precision;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	// double i = 0.2;
	double i = 1.5;
	// for(auto &it : variables.getVector()){
		// it = 0.0;
	// }
	for(auto &it : enhancement.getVector()){
		it = i;
		// it = 1;
		i -= 0.2;
		// i += 0.9;
	}
	// enhancement(3) *= 15;
	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);
	float quatError = 1;
	auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
	auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6
	u32 iterations = 0;
	for(; (positionError > minError || quatError > minError*10) && iterations<iterationLimit; iterations++){
		auto positionDelta = (target.position - endEffector.position);
		auto axisDelta = -glm::cross(glm::axis(target.quat), glm::axis(endEffector.quat));

		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, axisDelta.x, axisDelta.y, axisDelta.z});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		// robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
		quatError = glm::length(axisDelta);
	}
	// cout<<iterations<<" << "<<quatError<<endl;
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < minError;
	lastIterationCount = iterations;
	return succes;
}

bool JT2::solve(Point target, Robot &robot){
	float maxDistanceInSingleStep = 0.1f; /// m
	auto subTarget = robot.endEffector.position;
	auto totalDistance = glm::distance(target.position, robot.endEffector.position);
	auto distance = totalDistance;
	auto vecToTarget = glm::normalize(target.position - robot.endEffector.position);

	float minR = std::min(glm::distance(target.position, glm::vec4(0,0,0,1)), glm::distance(robot.endEffector.position, glm::vec4(0,0,0,1)));

	auto currentPosition = robot.endEffector.position;
	auto tmpJoints = robot.getVariables();
	u32 iterations = 0;
	// while(distance >= maxDistanceInSingleStep){
		// iterations++;
		// distance -= maxDistanceInSingleStep;
		// auto param = distance/totalDistance;
		// subTarget += vecToTarget * maxDistanceInSingleStep;

		// auto subTargetOnSphere = glm::normalize(subTarget - glm::vec4(0,0,0,1)) * std::max(glm::length(subTarget), minR);
		// subTargetOnSphere.w = 1;

		// performIK({currentPosition, robot.endEffector.quat} , {subTargetOnSphere, target.quat}, robot);
		// currentPosition = subTargetOnSphere;
		// robot.insertVariables(result);
	// }
	performIK({currentPosition, robot.endEffector.quat}, target, robot);

	robot.insertVariables(tmpJoints);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT2::performIK(Point start, Point target, Robot &robot, double precision){
	int iterationLimit = 2000;
	float minError = precision;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	double i = 1.5;
	for(auto &it : enhancement.getVector()){
		it = i;
		i -= 0.2;
	}
	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);
	float quatError = 1;
	auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
	auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6
	u32 iterations = 0;
	for(; (positionError > minError || quatError > minError*10) && iterations<iterationLimit; iterations++){
		auto positionDelta = (target.position - endEffector.position);
		glm::vec3 t = glm::normalize((glm::vec3(target.quat.x, target.quat.y, target.quat.z)));
		glm::vec3 e = glm::normalize((glm::vec3(endEffector.quat.x, endEffector.quat.y, endEffector.quat.z)));
		// auto axisDelta = -glm::cross(glm::axis(target.quat), glm::axis(endEffector.quat));
		auto axisDelta = -glm::cross(t, e);

		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, axisDelta.x, axisDelta.y, axisDelta.z});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		// robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
		quatError = glm::length(axisDelta);
	}
	// cout<<iterations<<" << "<<quatError<<endl;
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < minError;
	lastIterationCount = iterations;
	return succes;
}

extern glm::vec4 robotTarget;
extern glm::vec4 endPos;
extern glm::vec4 normal;
extern glm::vec4 surfaceOfMotion;
extern glm::quat gripperQuat;

void robotFollowTheMouseTest(Robot &robot){
	if(ui.rClick()){
		endPos = robot.endEffector.position;
		gripperQuat = robot.endEffector.quat;
		normal = glm::normalize(camera.Normal);
		// endPos = camera.eyePosition + normal*2.f;
		surfaceOfMotion = glm::vec4(glm::vec3(normal.xyz()), -glm::dot(normal, endPos));
	}

	// glm::vec4 newPoint = camera.eyePlaneIntersection(surfaceOfMotion);

	JT1 ik;
	// auto res = ik.performIK({ newPoint, gripperQuat }, robot);
	// auto res = ik.performIK({ robotTarget, gripperQuat }, robot);

	if(ui.rRepeat() && ik.succes){
		robotTarget = camera.eyePlaneIntersection(surfaceOfMotion);
		g_mousePositions(robotTarget);
	}
	robot.insertVariables(ik.result);

}

void jacobianTransposeInit(){
	jacobianIterations.setBouds({0,250,0,500});
	jacobianPrecision.setBouds({0,250,0,1});
}
void jacobianTransposeUpdate(){


}
void test(Robot &robot){
}

void jacobianTransposeInitialCall(Robot &robot){
	// robot.update(0.1);
	// auto solver = make_unique<JT1>();

	// Point target = {glm::vec4(4, 1, 4, 1), glm::angleAxis(1.f, glm::vec3(1,0,-1))};
	// Point target = {robot.endEffector.position, glm::angleAxis(1.f, glm::vec3(0,1,0))};

	// solver->solve(target, robot);
	// robot.insertVariables(solver->result);
}

