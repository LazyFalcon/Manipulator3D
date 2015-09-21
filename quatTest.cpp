
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
#include <iostream>
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
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2])+" "+string_formatted(acos(v[3])*2.f);
}
std::string to_string(const glm::vec3 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2]);
}


int main(){
	glm::vec4 z(0,0,1,0);
	glm::vec4 x(1,0,0,0);
	glm::quat b(0.5,0,0,1);
	glm::quat c = glm::angleAxis(1.f, glm::vec3(0,0,1));

	cout<<to_string(b*z)<<endl;
	cout<<to_string(b*x)<<endl;
	cout<<to_string(c*z)<<endl;

	return 0;
}