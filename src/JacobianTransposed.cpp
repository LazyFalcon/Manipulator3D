
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

glm::vec4 g_targetPosition;

Graph jacobianIterations("jacobianIterations", Graph::LastX, 0xFFFF00FF, 250);
Graph jacobianPrecision("jacobianPrecision", Graph::LastX, 0xFF4000FF, 250);
extern Plot mainPlot;

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
	performIK({}, target, robot);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT0::performIK(Point start, Point target, Robot &robot){
	int iterationLimit = 2000;
	float minError = 0.001f;

	auto endEffector = robot.endEffector;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	double i = 0.2;
	// double i = 1.5;
	// for(auto &it : variables.getVector()){
		// it = 0.0;
	// }
	for(auto &it : enhancement.getVector()){
		// it = i;
		it = 1;
		// i -= 0.2;
		i += 0.9;
	}
	// enhancement(3) *= 15;

	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, robot.endEffector.position);

	g_mousePositions(target.position);
	// if(positionError > minError*5.f)
		// target.position = robot.endEffector.position + (target.position - robot.endEffector.position)*0.1f;

	auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
	auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6
	u32 iteration = 0;
	for(; positionError > minError && iteration<iterationLimit; iteration++){


		auto positionDelta  = (target.position - endEffector.position);
		// auto axisDelta  = glm::cross(glm::axis(endEffector.quat), glm::axis(target.quat));

		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z,0, 0, 0});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		// robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
	}
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < minError;

	return succes;
}

bool JT1::solve(Point target, Robot &robot){
	float maxDistanceInSingleStep = 0.5f; /// m
	auto subTarget = target.position;
	auto totalDistance = glm::distance(target.position, robot.endEffector.position);
	auto distance = totalDistance;
	auto vecToTarget = glm::normalize(target.position - robot.endEffector.position);

	auto currentPosition = robot.endEffector.position;
	while(distance >= maxDistanceInSingleStep){
		distance -= maxDistanceInSingleStep;
		auto param = distance/totalDistance;
		// subTarget = glm::mix(robot.endEffector.position, target.position, param);
		subTarget += vecToTarget * maxDistanceInSingleStep;
		performIK({currentPosition, robot.endEffector.quat} , {subTarget, target.quat}, robot);
		currentPosition = subTarget;
	}

	performIK({currentPosition, robot.endEffector.quat}, target, robot);

	// cutHigherThanPi(result);

		// robot.insertVariables(result);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT1::performIK(Point start, Point target, Robot &robot){
	/* int iterationLimit = 2000;
	float minError = 0.001f;
	// double minError = 0.01f;

	auto endEffector = robot.endEffector;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	double i = 0.2;
	for(auto &it : enhancement.getVector()){
		it = i;
		// it = 1;
		// i -= 0.2;
		i += 0.9;
	}

	variables.insertColumn(0, robot.getVariables());

	double positionError = glm::distance(target.position, robot.endEffector.position);

	u32 iteration = 0;

	glm::vec4 vecToTarget = glm::normalize(target.position - robot.endEffector.position);
	glm::vec4 semiTarget = robot.endEffector.position;
	while(positionError > minError){
		double semiPositionError = glm::clamp(positionError, 0.0, 0.10);
		semiTarget += vecToTarget*glm::clamp(float(semiPositionError), 0.f, 0.1f);
		iteration = 0;

		cutHigherThanPi(variables.getVector());

		auto jacobian = buildJacobian(robot, variables.getVector(), endEffector);
		auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6

		for(; semiPositionError > minError && iteration<iterationLimit; iteration++){
			auto positionDelta  = (semiTarget - endEffector.position);
			auto axisDelta  = glm::cross(glm::axis(endEffector.quat), glm::axis(target.quat));

			// force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, axisDelta.x, axisDelta.y, axisDelta.z});
			force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z,0,0,0});

			auto a = dot(jjp*force, force);
			a = a/dot(jjp*force, jjp*force);
			auto gradient = jacobian*force*a;
			gradient = mul(gradient, enhancement);

			variables = gradient + variables;
			endEffector = robot.simulate(variables.getVector());
			g_targetPosition = endEffector.position;
			semiPositionError = glm::distance(endEffector.position, semiTarget);
		}

		positionError = glm::distance(endEffector.position, target.position);
	}
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < minError;

	return succes; */
	int iterationLimit = 2000;
	float minError = 0.001f;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	double i = 0.2;
	// double i = 1.5;
	// for(auto &it : variables.getVector()){
		// it = 0.0;
	// }
	for(auto &it : enhancement.getVector()){
		// it = i;
		it = 1;
		// i -= 0.2;
		i += 0.9;
	}
	// enhancement(3) *= 15;

	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);

	g_mousePositions(target.position);
	// if(positionError > minError*5.f)
		// target.position = robot.endEffector.position + (target.position - robot.endEffector.position)*0.1f;

	auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
	auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6
	u32 iteration = 0;
	for(; positionError > minError && iteration<iterationLimit; iteration++){


		auto positionDelta  = (target.position - endEffector.position);
		// auto axisDelta  = glm::cross(glm::axis(endEffector.quat), glm::axis(target.quat));

		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z,0, 0, 0});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		// robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
	}
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < minError;

	return succes;
}

bool JTReversed::performIK(Point start, Point target, Robot &robot){

	return true;
}

extern glm::vec4 robotTarget;
extern glm::vec4 endPos;
extern glm::vec4 normal;
extern glm::vec4 surfaceOfMotion;
extern glm::quat gripperQuat;
// TODO: poprawić newPoint, ma on bć punktm przecięcia ray z płaszczyzną, obecnie jest po chuju
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
	auto solver = make_unique<JT1>();

	Point target = {glm::vec4(2, 5, 4, 1), glm::quat(0.1, 0,0,-1)};

	// solver->solve(target, robot);
	// robot.insertVariables(solver->result);
}

