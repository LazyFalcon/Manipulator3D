#include "Includes.h"
#include "Utils.h"

static const float pi = 3.141592f;
static const float hpi = 0.5f * 3.141592f;
static const float pi2 = 2.f * 3.141592f;
static const double dpi = 3.141592653589793;
static const double hdpi = 0.5 * 3.141592653589793;
static const double sqdpi = 3.141592653589793 * 3.141592653589793;
static const double dpi2 = 2.0 * 3.141592653589793;

glm::vec4 colorHex(uint32_t hexVal){
	return glm::vec4(
		 ((hexVal >> 24) & 0xFF) / 255.f,
		 ((hexVal >> 16) & 0xFF) / 255.f,
		 ((hexVal >> 8) & 0xFF) / 255.f,
		 (hexVal & 0xFF) / 255.f
	);

}

HexColor gradientCalc(HexColor left, HexColor right, u8 position){
	u8 *arrLeft = reinterpret_cast<u8*>(&left);
	u8 *arrRight = reinterpret_cast<u8*>(&right);
	arrLeft[0] = arrLeft[0] + ((arrRight[0]-arrLeft[0])*position)/0xFF;
	arrLeft[1] = arrLeft[1] + ((arrRight[1]-arrLeft[1])*position)/0xFF;
	arrLeft[2] = arrLeft[2] + ((arrRight[2]-arrLeft[2])*position)/0xFF;
	arrLeft[3] = arrLeft[3] + ((arrRight[3]-arrLeft[3])*position)/0xFF;
	return left;
}
int clamp(int val, int a, int b){
	return std::max( std::min( val, b ), a );
}
std::default_random_engine randEngine;
std::normal_distribution<float> normalDistr;
float RNG(){
	return glm::clamp(normalDistr(randEngine),0.5f, 1.5f);
}
float RNG(float mod){
	return glm::clamp(normalDistr(randEngine),1.f-mod, 1.f+mod);
}
float sgnRNG(){
	return (1.f-RNG())>=0 ? 1.f : -1.f;
}
int iRNG(int val){
	return (int)(val*RNG());
}

void setColumn(glm::mat4 &mat, int num, glm::vec4 vec){
	mat[num][0] = vec[0];
	mat[num][1] = vec[1];
	mat[num][2] = vec[2];
	mat[num][3] = vec[3];
}

btVector3 to_btVector3(const glm::vec4& v){
	return btVector3(v[0], v[1], v[2]);
}
btVector3 to_btVector3(const glm::vec3& v){
	return btVector3(v[0], v[1], v[2]);
}
btVector3 to_btVector3(const glm::vec2& v){
	return btVector3(v[0], v[1], 0);
}
btTransform to_btTransform(const glm::mat4& mat){
	const float* data = glm::value_ptr(mat);
	btTransform bulletTransform;
	bulletTransform.setFromOpenGLMatrix(data);
	return bulletTransform;
}

glm::vec3 to_vec3(const btVector3& v){
	return glm::vec3(v[0], v[1], v[2]);
}
glm::vec4 to_vec4(const btVector3& v){
	return glm::vec4(v[0], v[1], v[2], 1);
}
glm::vec4 to_vec41(const btVector3& v){
	return glm::vec4(v[0], v[1], v[2], 1);
}
glm::vec4 to_vec40(const btVector3& v){
	return glm::vec4(v[0], v[1], v[2], 0);
}
glm::mat4 to_mat4(const btTransform& trans){
	static float tmp[16] = {1};

	trans.getOpenGLMatrix(tmp);

	glm::mat4 modelMatrix = glm::make_mat4x4(tmp);
	return modelMatrix;
}

glm::vec4 toGlm(const btVector3 &v){
	return glm::vec4(v[0], v[1], v[2], 1);
}
glm::vec3 toGlm3(const btVector3 &v){
	return glm::vec3(v[0], v[1], v[2]);
}
btVector3 tobt3(const glm::vec4 &v){
	return btVector3(v[0], v[1], v[2]);
}
btVector3 tobt3(const glm::vec3 &v){
	return btVector3(v[0], v[1], v[2]);
}
glm::mat4 toGlm(const btTransform &trans){
	static float tmp[16]={1};

	trans.getOpenGLMatrix(tmp);

	glm::mat4 modelMatrix=glm::make_mat4x4(tmp);
	return modelMatrix;
}

glm::vec4 cross(glm::vec4 const & x, glm::vec4 const & y){
	return glm::vec4(glm::cross(glm::vec3(x.xyz()), glm::vec3(y.xyz())),0);
}
glm::mat4 getOrientation(btRigidBody* _body){
	static float tmp[16]={1};
	btTransform blah = _body->getCenterOfMassTransform();
	// _body->getMotionState()->getWorldTransform(blah);
	blah.getOpenGLMatrix(tmp);

	glm::mat4 modelMatrix=glm::make_mat4x4(tmp);
	return modelMatrix;
}
glm::mat4 getOrientation(const btTransform &blah){
	static float tmp[16];
	blah.getOpenGLMatrix(tmp);
	return glm::make_mat4(tmp);
}
glm::mat4 GLMfromBtT(const btTransform &transform){
	static float tmp[16];
	transform.getOpenGLMatrix(tmp);
	return glm::make_mat4(tmp);
}
btTransform btTFromGLM(glm::mat4 &transform){
	const float* data = glm::value_ptr(transform);
	btTransform bulletTransform;
	bulletTransform.setFromOpenGLMatrix(data);
	return bulletTransform;
}
std::string string_formatted(double f){
	// std::ostringstream out;
	// out.precision(2);
	// out << std::setprecision(1) << f;
	// out<<f;
	char buffer [10];
	sprintf(buffer, "%.2f", f);

	return std::string(buffer);
}
std::string string_formatted(float f){
	// std::ostringstream out;
	// out.precision(2);
	// out << std::setprecision(1) << f;
	// out<<f;
	char buffer [10];
	sprintf(buffer, "%.2f", f);

	return std::string(buffer);
}
std::string to_string(const glm::vec4 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2])+" "+string_formatted(v[3]);
}
std::string to_string(const glm::quat &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2])+" "+string_formatted(acos(v[3])*2.f);
}
std::string to_string(const glm::vec3 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2]);
}
std::string to_string(const btVector3 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2]);
}
std::string to_string(const btTransform &t){

	// static float v[16]={1};

	// t.getOpenGLMatrix(v);
	return glm::to_string( getOrientation(t) );
}

float sign(float x){
	if(x < 0.f)
		return -1.f;
	else if(x > 0.f)
		return 1.f;
	return 0.f;
}
float period(float val, float a, float b){
	if(val >= b)
		val -= b-a;
	else if(val < a)
		val += b-a;
	return val;
}
double period(double val){
	if(val >= dpi)
		return val - dpi2;
	else if(val < -dpi)
		return val + dpi2;
	return val;
}

double sign(double x){
	if(x < 0.0)
		return -1.0;
	else if(x > 0.0)
		return 1.0;
	return 0.0;
}
double period(double val, double a, double b){
	if(val >= b)
		val -= b-a;
	else if(val < a)
		val += b-a;
	return val;
}

float circleDistance(float target, float value){
	float dist = target - value;
	if(dist >pi)
		return pi2 - dist;
	if(dist < -pi)
		return dist - pi2;
	return dist;
}
double circleDistance(double target, double value){
	double dist = target - value;
	if(dist > dpi)
		return dist - dpi2;
	if(dist < -dpi)
		return dpi2 - dist;
	return dist;
}

double clamp(double x, double minVal, double maxVal){
	return std::min(std::max(x, minVal), maxVal);
}

/* template <typename T>
bool saveCache(std::string fileName, std::vector<T> &data){
	ofstream file;
	file.open("cache/"+fileName+".cache", ios::binary | ios::out | ios::trunc);
		int size = data.size();
		file.write((char*)data.data(), data.size()*sizeof(T));
	file.close();
	return true;
}
template <typename T>
int loadCache(std::string fileName, std::vector<T> &data){
	ifstream file;
	int size=0;
	// try {
		file.open("../bin/cache/"+fileName+".cache", ios::binary | ios::in);
		if(file.is_open()){
			file.seekg(0, std::ios::end);
			size = file.tellg()/sizeof(T);
			file.seekg(0, std::ios::beg);

			// cerr<<"rozmiar danych: "<<size<<endl;
			data.resize(size);
			file.read((char*)data.data(), size*sizeof(T));
		}
		else
			cerr<<"\n\n\nfail..\n\n"<<endl;
		file.close();
	// }
	// catch (ifstream::failure e) {
    // cout << "Exception opening "+fileName+": "<<e.what()<<endl;;
  // }
	// cerr<<"loaded: "<<size<<endl;
	return size;
} */

/* // void listFiles(std::set<std::string>&files, std::string pathName, std::string fileType){
	// files.clear();
	// dirent *file;
	// DIR *path;
	// path = opendir(pathName.c_str());
	// if(path){
		// while(( file = readdir(path) ) ){
			// string name(file->d_name);
			// if(name.substr(name.find('.')) == fileType || fileType=="*"){
				// files.insert(name.substr(0,name.find('.')));
			// }
		// }

		// closedir(path);
	// }
	// else
			 // cout<<"dirent.h fucked up for: "+pathName<<endl;
// } */


double PDreg::operator () (double toReach, double current, double dt){
	speed = (prev - current)/dt;
	prev = current;

	return (toReach - current)*(dt*kr) - speed*(td*dt);
}


std::list <Statement> statements;
void statement(std::string text, float lifeTime){
	statements.emplace_back(Statement{text, lifeTime});
}

glm::quat glm_fromAxis(glm::vec3 v){
	return glm::rotation(glm::vec3(0,0,1), glm::normalize(v));
}
glm::quat glm_fromAxes(glm::vec3 parallel, glm::vec3 perpendicular){
    auto z = glm::normalize(parallel);
    auto x = glm::normalize(perpendicular);

    return glm::quat_cast(glm::mat3(x, glm::cross(z, x), z));
}
glm::quat glm_fromAxes4(glm::vec4 parallel, glm::vec4 perpendicular){
    return glm_fromAxes(parallel.xyz(), perpendicular.xyz());
}

