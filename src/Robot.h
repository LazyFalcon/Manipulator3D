#pragma once
class Robot;
class Entity;
enum  JointType{
	HINGE = 1,
	PRISMATIC = 2,
};

struct Point {
	glm::vec4 position;
	glm::quat quat;
};

class Module {
public:
	int type;
	glm::vec4 axis;
	glm::vec4 vecToA;
	glm::vec4 vecToB;
	float min, max;
	std::string name;
	Entity *entity;
	float value;
	
	glm::vec4 getMainAxis(){
		return glm::normalize(vecToA + vecToB);
	}
	glm::vec3 getMainAxis3(){
		return glm::normalize(vecToA.xyz() + vecToB.xyz());
	}
};
class Gripper : public Module{};

class IIK {
public:	
	std::vector<double> result;
	glm::vec4 endPosition;
	bool succes = false;
	
	virtual bool solve(Point aim, Robot &robot) = 0;
	virtual bool performIK(Point aim, Robot &robot)=0;
};


class Robot {
public:
	std::vector<Module*> chain;
	Point endEffector;
	
	std::vector<glm::vec4> forward(); // punkt ze wspó³¿êdnych
	void inverse(); // wspó³¿êdne z punktu
	void update(float dt);
	std::vector<float> getVariables();
	Point simulate(std::vector<double> &vec);
	std::vector<Point> simulateFullData(std::vector<double> &variables);
	glm::vec4 clamp(std::vector<double> &vec);
	glm::vec4 insertVariables(std::vector<double> &vec);
	int getSize(){return chain.size();}
};

class JT0 : public IIK {
public:

	bool solve(Point aim, Robot &robot);
	bool performIK(Point aim, Robot &robot);
};
class JT1 : public IIK {
public:

	bool solve(Point aim, Robot &robot);
	bool performIK(Point aim, Robot &robot);
};
class JTReversed : public IIK {
public:

	bool solve(Point aim, Robot &robot);
	bool performIK(Point aim, Robot &robot);
};
class CCD : public IIK {
public:

	bool solve(Point aim, Robot &robot);
	bool performIK(Point aim, Robot &robot);
};




