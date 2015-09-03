#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <random>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <chrono>
#include <string>
#include <unordered_map>
#include <iomanip>
#include <type_traits>
#include <memory>
#include <dirent.h>

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <fstream>
#include <ctime>
#include <cmath>

#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SSE2
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/projection.hpp>

#include <glm/gtx/simd_mat4.hpp>
#include <glm/gtx/simd_vec4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtx/matrix_major_storage.hpp>
#include <glm/gtx/perpendicular.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/optimum_pow.hpp>
using namespace std;

std::string string_formatted(double f){
	// std::ostringstream out;
	// out.precision(2);
	// out << std::setprecision(1) << f;
	// out<<f;
	char buffer [10];
	sprintf(buffer, "%.2f", f);

	return string(buffer);
}
std::string string_formatted(float f){
	// std::ostringstream out;
	// out.precision(2);
	// out << std::setprecision(1) << f;
	// out<<f;
	char buffer [10];
	sprintf(buffer, "%.2f", f);

	return string(buffer);
}

std::string to_string(const glm::vec4 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2])+" "+string_formatted(v[3]);
}
std::string to_string(const glm::quat &v){
	return string_formatted(v.x)+" "+string_formatted(v.y)+" "+string_formatted(v.z)+" "+string_formatted(acos(v.w)*2.f);
}
std::string to_string(const glm::vec3 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2]);
}

string prettyTo_string(const glm::mat4 &m){
    return to_string(glm::row(m, 0)) + "\n" + to_string(glm::row(m, 1)) + "\n" + to_string(glm::row(m, 2)) + "\n" + to_string(glm::row(m, 3));
}

const float pi = 3.141592f;
static const float hpi = 0.5f * 3.141592f;

void drawQaut(const glm::quat &q){
        cout<<glm::angle(q)<<endl;
        cout<<"  q: "+to_string(q)<<endl;
        cout<<"  a: "+to_string(glm::normalize(glm::axis(q)))<<endl;
        cout<<"  e: "+to_string(glm::eulerAngles(q))<<endl;
}

void quatTest_1(){
    glm::quat q1 = glm::angleAxis(0.f, glm::vec3(0,0,1));
    glm::quat q2 = glm::angleAxis(0.5f, glm::vec3(0,0,1));
    glm::quat q3 = glm::angleAxis(1.0f, glm::vec3(0,0,1));
    cout<<prettyTo_string(glm::mat4_cast(q1))<<endl;
    cout<<"------------------"<<endl;
    cout<<prettyTo_string(glm::mat4_cast(q2))<<endl;
    cout<<"------------------"<<endl;
    cout<<prettyTo_string(glm::mat4_cast(q3))<<endl;
    cout<<"------------------"<<endl;
}
void quatTest_2(){
    glm::quat qs[] = {
                    // glm::quat(0,0,0,1),
                    // glm::quat(1,0,0,1),
                    // glm::quat(2,0,0,1),
                    // glm::quat(0.5,0,0,1),
                    // glm::quat(0.1,0,0,1),
                    // glm::angleAxis(pi, glm::vec3(0,0,1)), // rotation from angle and axis
                    // glm::quat(cos(pi*0.5),0,0,1), // orientation from angle and axis
                    // glm::quat(cos(1.256*0.5),0,0,1), // orientation from angle and axis
                    // glm::quat(cos(0.1*0.5),0,0,1), // orientation from angle and axis
                    glm::angleAxis(pi, glm::normalize(glm::vec3(1,1,1))), // rotation from angle and axis
                    glm::angleAxis(0.f, glm::normalize(glm::vec3(1,1,1))), // rotation from angle and axis
                    glm::quat(cos(pi*0.5),glm::normalize(glm::vec3(1,1,1))), // orientation from
                    glm::quat(cos(0.5*0.5),glm::normalize(glm::vec3(1,1,1))), // orientation from
                    glm::quat(cos(0.0*0.5),glm::normalize(glm::vec3(1,1,1))), // orientation from
                    // glm::quat(glm::vec3(0,0,pi)),
                    // glm::quat(glm::vec3(0,0,pi*2)),
                    // glm::quat(glm::vec3(0,0,pi*0.5)),
        };
    for(auto &q : qs){
        cout<<glm::angle(q)<<endl;
        cout<<"\t"+to_string(q)<<endl;
        cout<<"\t"+to_string(glm::normalize(glm::axis(q)))<<endl;
        cout<<"\t"+to_string(glm::eulerAngles(q))<<endl;
    }
}
/**
 *  Podsumowując:
 *      kwaterniony można łatwo zamieniać pomiędzy katami eulera
 *      z angleAxis(kąt, wektor) można zdefiniować rotację, sam kwaternion bezpośrednio tworzy się poprzez(cos(kąt/2), oś)
 *      osie trzeba znormalizować
 *      kąt zero tworzy nam przekształcenie identycznościowe :(
 *      za to acos(quat.w)*2 zawiera kąt który nas interesuje(nawet zera), angle axis z zera tworzy identycznościowe
 *
 *  http://stackoverflow.com/questions/26871550/how-to-rotate-3d-camera-with-glm
 *
 */

void interpolationTest(){

    glm::quat q1(cos(hpi*0.5), 0, 0, -1);
    glm::quat q2(cos(hpi*1.5), 0, 0, -1);





    cout<<"  q1: "+to_string(glm::cross(glm::normalize(glm::axis(q1)), glm::normalize(glm::axis(q2))))<<endl;
    cout<<"  q1: "+to_string(glm::cross(glm::normalize(glm::vec3(q1.x, q1.y, q1.z)), glm::normalize(glm::vec3(q2.x, q2.y, q2.z))))<<endl;
    // cout<<"  q1: "+to_string(glm::normalize(glm::vec3(q1.x, q1.y, q1.z)))<<endl;
    // cout<<"  q2: "+to_string(glm::normalize(glm::axis(q2)))<<endl;
    // cout<<"  q2: "+to_string(glm::normalize(glm::vec3(q2.x, q2.y, q2.z)))<<endl;


    // for(auto i = 0.f; i<=1.f; i+=0.2){
        // auto q = glm::slerp(q1, q2, i);
        // drawQaut(q);
    // }
}

int main(){
    // quatTest_2();
    interpolationTest();
    return 0;
}
