#pragma once
#include <Utils/BaseStructs.h>
class Robot;
enum JointType {
	REVOLUTE_JOINT = 1,
	PRISMATIC_JOINT = 2,
};

enum class SolverType : int {
	JT0, JT1, JT2, CCD
};

struct SystemSettings
{
	double positionPrecision;
	double orientationPrecision;
	u32 solverIterationLimit;
	bool useRobotConstraints;
	bool enableCollisions;
	bool usePhysics;
};

struct Point {
	glm::vec4 position;
	glm::quat quat;
};

class Module {
public:
	virtual ~Module(){
		std::cerr<<"~Module"<<std::endl;
	}
	Module(){
		lastVelocity = 0.0;
		lastAcceleration = 0.0;
		value = 0.0;
	}
	bool goTo(float dt, double jVelocityModifier);
	double computeMaxStep(float dt, double jVelocityModifier);
	glm::vec4 getMainAxis(){
		return glm::normalize(vecToA + vecToB);
	}
	glm::vec3 getMainAxis3(){
		return glm::normalize(vecToA.xyz() + vecToB.xyz());
	}
	void reset(){
		lastVelocity = 0;
		lastAcceleration = 0;
		value = 0;
		targetValue = 0;
	}

	void incr(){value += incrValue;}
	void decr(){value -= incrValue;}

	double incrValue = 0.01;

	int type;
	glm::vec4 axis;
	glm::vec4 vecToA; /// wektor ze œrodka do zamocowania cz³onu
	glm::vec4 vecToB; /// wektor do wyjœcia z cz³onu
	std::string name;
	std::shared_ptr<Entity> entity;
	double min, max;
	double value;
	double targetValue;
	double maxVelocty;
	double maxAcceleration;
	double lastVelocity;
	double lastAcceleration;
};

class Gripper : public Module{};
class Solver {
public:
	std::vector<double> result;
	glm::vec4 endPosition;
	glm::vec4 endQuat;
	bool succes = false;

	virtual bool solve(Point target, Robot &robot) = 0;
	virtual bool performIK(Point start, Point target, Robot &robot, double precision = -1)=0;
	virtual ~Solver(){}
};

class Robot {
public:
	~Robot(){
		for(auto &it : chain)
			it.reset();
		chain.clear();
		std::cerr<<"~Robot"<<std::endl;
	}
	Robot(){
		config.positionPrecision = 0.0005;
		config.orientationPrecision = 0.01;
		config.solverIterationLimit = 2000;
		config.useRobotConstraints = false;
		config.enableCollisions = true;
		config.usePhysics = true;
	}

	void update(float dt);
	Point simulate(std::vector<double> &vec);
	std::vector<double> getVariables();
	bool goTo(float dt, double jVelocityModifier);
	bool goTo(const std::vector<double> &jointPositions);
	std::vector<Point> simulateFullData(std::vector<double> &variables);
	glm::vec4 clamp(std::vector<double> &vec);
	glm::vec4 insertVariables(std::vector<double> &vec);
	int getSize(){return chain.size();}
	void reset();
	Module& module(int i){
		return *chain[i];
	}
	int getModuleCount(){
		return chain.size();
	}

	SystemSettings config;
	SystemSettings& getConfig(){
		return config;
	}
	void getConfig(const SystemSettings &c){
		config = c;
	}
	bool isReady { true };
	Point endEffector;

	glm::vec4 basePosition;

	std::vector<std::unique_ptr<Module>> chain;
};

/// without orientation
class JT0 : public Solver {
public:
	~JT0(){
		std::cerr<<"~JT0"<<std::endl;
	}
	bool solve(Point aim, Robot &robot);
	bool performIK(Point start, Point target, Robot &robot, double precision = -1);
};
/// with orientation
class JT1 : public Solver {
public:
	~JT1(){
		std::cerr<<"~JT1"<<std::endl;
	}
	bool solve(Point aim, Robot &robot);
	bool performIK(Point start, Point target, Robot &robot, double precision = -1);
};
/// with orientation
class JT2 : public Solver {
public:
	~JT2(){
		std::cerr<<"~JT2"<<std::endl;
	}
	bool solve(Point aim, Robot &robot);
	bool performIK(Point start, Point target, Robot &robot, double precision = -1);
};

void robotTestInit(Robot &robot);
void robotTest(float dt, Robot &robot);
