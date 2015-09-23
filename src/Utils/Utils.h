#pragma once
#include "Includes.h"

glm::vec4 colorHex(uint32_t hexVal);
HexColor gradientCalc(HexColor left, HexColor right, u8 position);
// glm::vec4 colorHex(uint64_t hexVal);
int clamp(int val, int a, int b);
float RNG();
float RNG(float mod);
float sgnRNG();
int iRNG(int val);

void setColumn(glm::mat4 &mat, int num, glm::vec4 vec);

btVector3 to_btVector3(const glm::vec4&);
btVector3 to_btVector3(const glm::vec3&);
btVector3 to_btVector3(const glm::vec2&);
btTransform to_btTransform(const glm::mat4&);

glm::vec3 to_vec3(const btVector3&);
glm::vec4 to_vec4(const btVector3&);
glm::vec4 to_vec41(const btVector3&);
glm::vec4 to_vec40(const btVector3&);
glm::mat4 to_mat4(const btTransform&);

glm::vec4 toGlm(const btVector3 &v);
glm::vec3 toGlm3(const btVector3 &v);
btVector3 tobt3(const glm::vec4 &v);
btVector3 tobt3(const glm::vec3 &v);
glm::mat4 toGlm(const btTransform &trans);

glm::vec4 cross(glm::vec4 const & x, glm::vec4 const & y);
glm::mat4 getOrientation(btRigidBody* _body);
glm::mat4 getOrientation(const btTransform &blah);
glm::mat4 GLMfromBtT(const btTransform &transform);
btTransform btTFromGLM(glm::mat4 &transform);
std::string string_formatted(double f);
std::string string_formatted(float f);
std::string to_string(const glm::vec4 &v);
std::string to_string(const glm::quat &v);
std::string to_string(const glm::vec3 &v);
std::string to_string(const btVector3 &v);
std::string to_string(const btTransform &t);

float period(float val, float a, float b);
double period(double val, double a, double b);
double period(double val);
float sign(float x);
double sign(double x);
float circleDistance(float target, float value);
double circleDistance(double target, double value);

double clamp(double x, double minVal, double maxVal);

template <typename T>
bool saveCache(std::string fileName, std::vector<T> &data, std::string ex = "cache"){
	std::ofstream file;
	file.open("cache/"+fileName+"."+ex, std::ios::binary | std::ios::out | std::ios::trunc);
		int size = data.size();
		file.write((char*)data.data(), data.size()*sizeof(T));
	file.close();
	return true;
}
template <typename T>
int loadCache(std::string fileName, std::vector<T> &data, std::string ex = "cache"){
	std::ifstream file;
	int size=0;
	// try {
		file.open("../bin/cache/"+fileName+"."+ex, std::ios::binary | std::ios::in);
		if(file.is_open()){
			file.seekg(0, std::ios::end);
			size = file.tellg()/sizeof(T);
			file.seekg(0, std::ios::beg);

			// cerr<<"rozmiar danych: "<<size<<endl;
			data.resize(size);
			file.read((char*)data.data(), size*sizeof(T));
		}
		else
			std::cerr<<"\n\n\nfail..\n\n"<<std::endl;
		file.close();
	// }
	// catch (ifstream::failure e) {
    // cout << "Exception opening "+fileName+": "<<e.what()<<endl;;
  // }
	// cerr<<"loaded: "<<size<<endl;
	return size;
}
// void listFiles(std::set<std::string>&files, std::string pathName, std::string fileType);

class PDreg {
public:
	double kr;
	double td;
	double ti;

	double speed;
	double prev;
	double integral;

	double operator () (double toReach, double current, double dt);
};

struct Statement {
	std::string statement;
	float lifeTime;
};

void statement(std::string text, float lifeTime = 5.f);

glm::quat glm_fromAxis(glm::vec3 v);
glm::quat glm_fromAxes(glm::vec3 parallel, glm::vec3 perpendicular);
glm::quat glm_fromAxes4(glm::vec4 parallel, glm::vec4 perpendicular);



#include "Bezier.h"