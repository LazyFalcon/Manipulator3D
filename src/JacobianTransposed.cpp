#include "Utils/Includes.h"
#include "Utils/Utils.h"
#include "Utils/Camera.h"
#include "Utils/IMGUI_V4.h"
#include "common.h"
#include "Robot.h"
#include "JacobianTransposed.h"
#include "Timer.h"
#include "JacobianMatrix.h"
#include "Helper.h"

#include "Graph.h"

extern UI::IMGUI ui;
extern Camera camera;
extern float mouse_x, mouse_y;

extern int robotPositionsCounter;
extern int robotPositionsMax;
extern vector<glm::vec4> robotPositions;
extern PositionSaver g_mousePositions;
extern PositionSaver g_robotPositions;
extern PositionSaver g_robotDebugPositions;
extern std::list<Plot*> plotList;

// static const float pi = 3.141592f;
// static const float hpi = 0.5f * 3.141592f;
static const float pi2 = 2.f * 3.141592f;
// static const double dpi = 3.141592653589793;
static const double hdpi = 0.5 * 3.141592653589793;
static const double sqdpi = 3.141592653589793 * 3.141592653589793;
static const double dpi2 = 2.0 * 3.141592653589793;

u32 lastIterationCount;
double lastSolverError;
glm::vec4 g_targetPosition;

glm::vec3 DEBUG_VEC3_1;
glm::vec3 DEBUG_VEC3_2;
glm::vec3 DEBUG_VEC3_3;
glm::vec4 DEBUG_VEC4_1;
glm::vec4 DEBUG_VEC4_2;
glm::quat DEBUG_QUAT;

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

	performIK({currentPosition, robot.endEffector.quat}, target, robot);

	robot.insertVariables(tmpJoints);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT0::performIK(Point start, Point target, Robot &robot, double precision){
	Timer<double, 1000,1> precisetimer;
	precisetimer.start();

	u32 iterationLimit = robot.config.solverIterationLimit;
	float positionPrecision = precision>0 ? precision : robot.config.positionPrecision;
	float orientationPrecision = precision>0 ? precision*10 : robot.config.orientationPrecision;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);

	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);
	u32 iterations = 0;
	for(; positionError > positionPrecision && iterations<iterationLimit; iterations++){
		auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
		auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6,

		auto positionDelta = (target.position - endEffector.position);
		auto axisDelta = -glm::cross(glm::axis(target.quat), glm::axis(endEffector.quat))*0.f;

		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, axisDelta.x, axisDelta.y, axisDelta.z});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;

		variables = gradient + variables;
		robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
	}
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < positionPrecision;
	lastIterationCount = iterations;

	precisetimer.end();

	Helper::record().IKIterationTime = precisetimer.get();
	Helper::record().IKIterarationCount = iterations;
	Helper::record().IKPositionError = positionError;

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
Timer<double, 1000,1> precisetimer;
	precisetimer.start();

	u32 iterationLimit = robot.config.solverIterationLimit;
	float positionPrecision = precision>0 ? precision : robot.config.positionPrecision;
	float orientationPrecision = precision>0 ? precision*10 : robot.config.orientationPrecision;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	double i = 1.0;
	u32 count = 0;
	bool oneChance = true;
	for(auto &it : enhancement.getVector()){
		it = i;
		// i += 0.2;
		if((count++)%2) i = 1.1;
		else i = 0.9;
	}
	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);
	u32 iterations = 0;
	for(; positionError > positionPrecision && iterations<iterationLimit; iterations++){

		auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
		auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6

		auto positionDelta = (target.position - endEffector.position);

		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, 0,0,0});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);

		if(iterations > 100 && oneChance){
			iterations = 0;
			oneChance = false;
			// variables.insertColumn(0, robot.getVariables());
			variables = mul(variables, enhancement);
		}
	}

	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < positionPrecision;
	lastIterationCount = iterations;

	precisetimer.end();

	Helper::record().IKIterationTime = precisetimer.get();
	Helper::record().IKIterarationCount = iterations;
	Helper::record().IKPositionError = positionError;

	return succes;
}
/**
 *  Local rotation of last joint
 */
bool JT2::solve(Point target, Robot &robot){
	float maxDistanceInSingleStep = 0.1f; /// m
	auto subTarget = robot.endEffector.position;
	auto totalDistance = glm::distance(target.position, robot.endEffector.position);
	auto distance = totalDistance;
	auto vecToTarget = glm::normalize(target.position - robot.endEffector.position);

	float minR = std::min(glm::distance(target.position, glm::vec4(0,0,0,1)), glm::distance(robot.endEffector.position, glm::vec4(0,0,0,1)));
	auto currentPosition = robot.endEffector.position;
	auto tmpJoints = robot.getVariables();

	/// enable this when solver behave bad
	// glm::vec3 t = glm::normalize((glm::vec3(target.quat.x, target.quat.y, target.quat.z)));
	// glm::vec3 e = glm::normalize((glm::vec3(robot.endEffector.quat.x, robot.endEffector.quat.y, robot.endEffector.quat.z)));
	// float angle = acos(glm::dot(t,e))/pi;
	// if(angle > 0.1f){

		// auto prewQ = robot.endEffector.quat;
		// auto newQ = glm::slerp(robot.endEffector.quat, target.quat, 1 - angle);
		// while(angle > 0.05f){
			// performIK({currentPosition, prewQ}, {target.position, newQ}, robot);
			// angle -= 0.1f;
			// }
		// cout<<"next?";
		// cin.ignore();
		// prewQ = newQ;
		// robot.insertVariables(result);
	// }

	u32 iterations = 0;
	while(distance >= maxDistanceInSingleStep){
		iterations++;
		distance -= maxDistanceInSingleStep;
		auto param = distance/totalDistance;
		subTarget += vecToTarget * maxDistanceInSingleStep;

		auto subTargetOnSphere = glm::normalize(subTarget - glm::vec4(0,0,0,1)) * std::max(glm::length(subTarget), minR);
		subTargetOnSphere.w = 1;

		// performIK({currentPosition, robot.endEffector.quat} , {subTargetOnSphere, target.quat}, robot);
		performIK({currentPosition, robot.endEffector.quat} , {
					subTargetOnSphere, glm::slerp(robot.endEffector.quat, target.quat, param)
				}, robot);
		currentPosition = subTargetOnSphere;
		robot.insertVariables(result);
	}


	performIK({currentPosition, robot.endEffector.quat}, target, robot);

	robot.insertVariables(tmpJoints);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT2::performIK(Point start, Point target, Robot &robot, double precision){
	Timer<double, 1000,1> precisetimer;
	precisetimer.start();

	u32 iterationLimit = robot.config.solverIterationLimit;
	float positionPrecision = precision>0 ? precision : robot.config.positionPrecision;
	float orientationPrecision = precision>0 ? precision*10 : robot.config.orientationPrecision;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	double i = 0.5;
	for(auto &it : enhancement.getVector()){
		it = i;
		i += 0.2;
	}
	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);
	float quatError = 1;
	u32 iterations = 0;
	for(; (positionError > positionPrecision || quatError > orientationPrecision) && iterations<iterationLimit; iterations++){

		auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
		auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6

		auto positionDelta = (target.position - endEffector.position);
		glm::vec3 t = glm::normalize((glm::vec3(target.quat.x, target.quat.y, target.quat.z)));
		glm::vec3 e = glm::normalize((glm::vec3(endEffector.quat.x, endEffector.quat.y, endEffector.quat.z)));

		auto axisDelta = glm::cross(e, t);

		float axisMod = 3.1;
		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, axisDelta.x*axisMod, axisDelta.y*axisMod, axisDelta.z*axisMod});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		// variables.getVector().back() = acos(target.quat.w)*2.0;
		robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
		quatError = glm::length(axisDelta);
	}
	// cout<<iterations<<" << "<<quatError<<endl;
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < positionPrecision && quatError < positionPrecision*10;
	lastIterationCount = iterations;
	lastSolverError = quatError;

	precisetimer.end();

	Helper::record().IKIterationTime = precisetimer.get();
	Helper::record().IKIterarationCount = iterations;
	Helper::record().IKPositionError = positionError;
	Helper::record().IKOrientationError = quatError;

	return succes;
}

/**
 *  Global rotation of last joint
 */
bool JT3::solve(Point target, Robot &robot){
	float maxDistanceInSingleStep = 0.1f; /// m
	auto subTarget = robot.endEffector.position;
	auto totalDistance = glm::distance(target.position, robot.endEffector.position);
	auto distance = totalDistance;
	auto vecToTarget = glm::normalize(target.position - robot.endEffector.position);

	float minR = std::min(glm::distance(target.position, glm::vec4(0,0,0,1)), glm::distance(robot.endEffector.position, glm::vec4(0,0,0,1)));
	auto currentPosition = robot.endEffector.position;
	auto tmpJoints = robot.getVariables();

	/// enable this when solver behave bad
	// glm::vec3 t = glm::normalize((glm::vec3(target.quat.x, target.quat.y, target.quat.z)));
	// glm::vec3 e = glm::normalize((glm::vec3(robot.endEffector.quat.x, robot.endEffector.quat.y, robot.endEffector.quat.z)));
	// float angle = acos(glm::dot(t,e))/pi;
	// if(angle > 0.1f){

		// auto prewQ = robot.endEffector.quat;
		// auto newQ = glm::slerp(robot.endEffector.quat, target.quat, 1 - angle);
		// while(angle > 0.05f){
			// performIK({currentPosition, prewQ}, {target.position, newQ}, robot);
			// angle -= 0.1f;
			// }
		// cout<<"next?";
		// cin.ignore();
		// prewQ = newQ;
		// robot.insertVariables(result);
	// }

	u32 iterations = 0;
	while(distance >= maxDistanceInSingleStep){
		iterations++;
		distance -= maxDistanceInSingleStep;
		auto param = distance/totalDistance;
		subTarget += vecToTarget * maxDistanceInSingleStep;

		auto subTargetOnSphere = glm::normalize(subTarget - glm::vec4(0,0,0,1)) * std::max(glm::length(subTarget), minR);
		subTargetOnSphere.w = 1;

		// performIK({currentPosition, robot.endEffector.quat} , {subTargetOnSphere, target.quat}, robot);
		performIK({currentPosition, robot.endEffector.quat} , {
					subTargetOnSphere, glm::slerp(robot.endEffector.quat, target.quat, param)
				}, robot);
		currentPosition = subTargetOnSphere;
		robot.insertVariables(result);
	}


	performIK({currentPosition, robot.endEffector.quat}, target, robot);

	robot.insertVariables(tmpJoints);
	return glm::distance(endPosition, target.position) < 0.005;
}
bool JT3::performIK(Point start, Point target, Robot &robot, double precision){
	Timer<double, 1000,1> precisetimer;
	precisetimer.start();

	u32 iterationLimit = robot.config.solverIterationLimit;
	float positionPrecision = precision>0 ? precision : robot.config.positionPrecision;
	float orientationPrecision = precision>0 ? precision*10 : robot.config.orientationPrecision;

	auto endEffector = start;
	auto force = Matrix(6,1);
	Matrix variables(robot.getSize(), 1);
	Matrix enhancement(robot.getSize(), 1);

	double i = 0.5;
	for(auto &it : enhancement.getVector()){
		it = i;
		i += 0.2;
	}
	variables.insertColumn(0, robot.getVariables());

	float positionError = glm::distance(target.position, endEffector.position);
	float quatError = 1;
    float lastJointError = 0;
	u32 iterations = 0;

    glm::vec3 t;
    glm::vec3 e;
	for(; (positionError > positionPrecision || quatError > orientationPrecision || abs(lastJointError) > 0.001) && iterations<iterationLimit; iterations++){
	// for(; (positionError > positionPrecision || quatError > orientationPrecision || abs(lastJointError) < 0.999) && iterations<iterationLimit; iterations++){

		auto jacobian = buildJacobian(robot,variables.getVector(), endEffector);
		auto jjp = jacobian.transposed() * jacobian; // 6xn * nx6 da 6x6

		auto positionDelta = (target.position - endEffector.position);
		// glm::vec3 t = glm::normalize((glm::vec3(target.quat.x, target.quat.y, target.quat.z)));
		// glm::vec3 e = glm::normalize((glm::vec3(endEffector.quat.x, endEffector.quat.y, endEffector.quat.z)));
		t = glm::normalize(glm::vec3(target.quat * glm::vec4(0,0,1,0).xyz()));
		e = glm::normalize(glm::vec3(endEffector.quat * glm::vec4(0,0,1,0).xyz()));


		// auto axisDelta = glm::cross(e, t) - glm::cross(e_x, t_x);
		auto axisDelta = glm::cross(e, t);

		float axisMod = 3.1;
		force.insertColumn(0, {positionDelta.x, positionDelta.y, positionDelta.z, axisDelta.x*axisMod, axisDelta.y*axisMod, axisDelta.z*axisMod});

		auto a = dot(jjp*force, force);
		a = a/dot(jjp*force, jjp*force);
		auto gradient = jacobian*force*a;
		gradient = mul(gradient, enhancement);

		variables = gradient + variables;
		// lastJointError = circleDistance(acos(target.quat.w)*2.0, acos(endEffector.quat.w)*2.0);


		// lastJointError = 1.f - glm::dot(t_x, e_x);
		// lastJointError = glm::length(glm::cross(e_x, t_x)) * glm::dot(glm::normalize(glm::cross(e_x, t_x)), e);
		// lastJointError = glm::length(1.f - glm::dot(t_x, e_x)) * glm::dot(glm::normalize(glm::cross(e_x, t_x)), e);

        // if(iterations%10 == 0){
            // cout<<"------------"<<endl;
            // cout<<to_string(t_x)<<endl;
            // cout<<to_string(e_x)<<endl;
            // cout<<"( "<<lastJointError<<" )"<<endl;
        // }
        // variables.getVector().back() += lastJointError * 0.1f;
		// variables.getVector().back() = period(variables.getVector().back());
		robot.clamp(variables.getVector());
		endEffector = robot.simulate(variables.getVector());
		g_targetPosition = endEffector.position;
		positionError = glm::distance(endEffector.position, target.position);
		quatError = glm::length(axisDelta);
	}

    glm::vec3 t_x = glm::normalize((target.quat * glm::vec4(1,0,0,0)).xyz());
    glm::vec3 e_x = glm::normalize((endEffector.quat * glm::vec4(1,0,0,0)).xyz());
    cout<<"a: "<<variables.getVector().back()<<endl;
    variables.getVector().back() -= glm::orientedAngle(t_x, e_x, e);
    cout<<"a: "<<variables.getVector().back()<<endl;
    endEffector = robot.simulate(variables.getVector());

    t_x = glm::normalize((target.quat * glm::vec4(1,0,0,0)).xyz());
    auto e_x2 = glm::normalize((endEffector.quat * glm::vec4(1,0,0,0)).xyz());
    cout<<"------------"<<endl;
    cout<<"t_x: "<<to_string(t_x)<<endl;
    cout<<"e_x: "<<to_string(e_x)<<endl;
    cout<<"e_x2: "<<to_string(e_x2)<<endl;
    cout<<"q: "<<to_string(endEffector.quat)<<endl;
    cout<<"a: "<<glm::orientedAngle(t_x, e_x, e)<<endl;
    cout<<"------------"<<endl;


	// cout<<iterations<<" << "<<quatError<<endl;
	endPosition = endEffector.position;
	result = variables.getVector();
	succes = positionError < positionPrecision && quatError < positionPrecision*10;
	lastIterationCount = iterations;
	lastSolverError = quatError;

	precisetimer.end();

	Helper::record().IKIterationTime = precisetimer.get();
	Helper::record().IKIterarationCount = iterations;
	Helper::record().IKPositionError = positionError;
	Helper::record().IKOrientationError = quatError;

	return succes;
}

