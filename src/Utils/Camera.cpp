#include "Utils.h"
#include "Includes.h"
#include "Camera.h"

extern glm::mat4 identity;
extern glm::vec3 		Z;
extern glm::vec3 		X;
extern glm::vec3 		Y;
extern bool 				GUI;
extern float 				window_width, window_height;
extern glm::vec2    mousePosition;
extern const float 	PI;
extern const float 	pi;
extern glm::vec4 		CameraUp;
extern glm::vec4 		CameraRight;
extern glm::vec4 		CameraNormal;
extern GLFWwindow 	*window;

void Camera::init(){
	cameraType = CAMERA_FOLLOW;
	cameraProjection = PERSPECTIVE_PROJECTION;
	std::vector<float> points {0.f, 0.7f, 1.f};
	std::vector<float> weights {0.3f,1,1};
	m_rotBezier.edit(points, weights);
	m_rotBezier.reg("camera");
	rotMatrix = identity;
	ViewMatrix = identity;
	camOffset = glm::vec3(0,0,-7);
	mainQuat = glm::angleAxis(0.f,X);
	setProjection();
}

// void Camera::zoomAngle(int x){
	// zoom_vel=18.f*x;
		// m_angle += zoom_vel*m_angle/45;
	// setProjection();
// }
void Camera::zoom(int x){
	zoom_vel=18.f*x;
}
void Camera::zoomStep(float x){
	if(cameraType == CAMERA_SNIPER)
		m_angle += 10*x*m_angle/45;
	else if(cameraType == CAMERA_FREE)
		Position += Normal*(5.f*x);
	else if(cameraType == CAMERA_FOLLOW || CAMERA_QUATERNION_BASED)
		distance += 5*x;
	setProjection();
}
void Camera::changeView(){
	if(cameraType == CAMERA_SNIPER){
		cameraType = CAMERA_FREE;
		m_angle = 90.f;
		m_near = 0.01f;
		setProjection();
	}
	else if(cameraType == CAMERA_FOLLOW){
		cameraType = CAMERA_SNIPER;
		m_angle = 50.f;
		m_near = 0.01f;
		setProjection();
	}
	else if(cameraType == CAMERA_FREE){
		cameraType = CAMERA_FOLLOW;
		m_angle = freeCamAngle;
		m_near = 0.01f;
		setProjection();
	}
}
void Camera::moveCamera(float moveFront, float moveUP, float rotateZ, float rotateY, float roll){
	Velocity = 500.f*glm::vec4(-moveFront, rotateZ/4, moveUP/4, 0);
	Omega=-glm::vec4(Z,0)*rotateZ+glm::vec4(Y,0)*rotateY+glm::vec4(X,0)*roll;
}
void Camera::setMouseMov(float _dx, float _dy){
	dx = _dx;
	dy = _dy;
}
void Camera::setCamPosition(glm::mat4 a){
	cam_position = (a*P0).xyz();
	Transformation = a;
}
void Camera::setCamPosition(glm::vec3 a){
	cam_position = a;
}
void Camera::updateCamera(float step){
	double x=0, y=0;

	if(!GUI){
		glfwGetCursorPos(window, &x,&y);
		x = glm::clamp(float(x), 0.f, window_width);
		y = glm::clamp(float(y), 0.f, window_height);
		dx = (x-window_width/2.f)/window_width/2.f;
		dy = (y-window_height/2.f)/window_height/2.f;
	}
	if(cameraType == CAMERA_QUATERNION_BASED){
		distance += zoom_vel*step;

		// good for free movement, no gimbal lock
		// mainQuat *= glm::angleAxis(dx, Up.xyz()) * glm::angleAxis(dy, Right.xyz());

		// mainQuat =  mainQuat * glm::angleAxis(dy*2.5f, Right.xyz()) * glm::angleAxis(dx*2.5f, Z);
		mainQuat =  glm::rotate(mainQuat, dy*2.5f, Right.xyz());
		mainQuat =  glm::rotate(mainQuat, dx*2.5f, Z);
		rot_x = glm::pitch(mainQuat);
		rot_z = glm::roll(mainQuat);
		rotMatrix = glm::mat4_cast(mainQuat);
	}
	else if(cameraType == CAMERA_FREE){
		Position += (-Velocity[0]*Normal + Velocity[1]*Right - Velocity[2]*Up)*step*300.f;
		rot_z += m_rotBezier.eval(dx)*2;
		rot_x -= m_rotBezier.eval(dy)*2;

		rot_z = period(rot_z, 0,2*PI);
		rot_x = glm::clamp(rot_x, 0.f, PI);
		rotMatrix = glm::rotate(identity, -rot_x, glm::vec3(1,0,0))*glm::rotate(identity, rot_z, glm::vec3(0,0,1));
	}
	else if(cameraType == CAMERA_SNIPER){
		m_angle += zoom_vel*step;
		m_angle = glm::clamp(m_angle, 0.1f, 160.f);
		setProjection();
		rot_z += m_rotBezier.eval(dx)*0.9f*m_angle/35.f;
		rot_x += m_rotBezier.eval(dy)*0.4f*m_angle/35.f;
		// rot_z = glm::clamp(rot_z, -0.2f, 0.2f);
		rot_x = glm::clamp(rot_x, -0.5f, 0.5f);
		rot_z = period(rot_z, 0,2*PI);

		rotMatrix = glm::rotate(identity, rot_x, glm::vec3(1,0,0))*glm::rotate(identity, rot_z, glm::vec3(0,0,1));
	}
	else if(CAMERA_FOLLOW){
		distance += zoom_vel*step;
		rot_z += m_rotBezier.eval(dx)*2;
		rot_x -= m_rotBezier.eval(dy)*1.2;

		rot_z = period(rot_z, 0,2*PI);
		rot_x = glm::clamp(rot_x, 0.f, PI*0.65f);

		rotMatrix = glm::rotate(identity, -rot_x, X3)*glm::rotate(identity, rot_z, Z3);
	}
	else if(EYE){
		distance += zoom_vel*step;
		rot_z += m_rotBezier.eval(dx)*2;
		rot_x += m_rotBezier.eval(dy)*2;
		// rot_z -= dx*0.01;
		// rot_x += dy*0.01;

		rot_z = period(rot_z, 0,2*PI);
		rot_x = glm::clamp(rot_x, 0.f, PI*0.65f);

		rotMatrix = glm::rotate(identity, -rot_x, X3)*glm::rotate(identity, rot_z, Z3);
	}

	// eyePosition = -ViewMatrix*P0;
	dx = 0;
	dy = 0;
}
void Camera::setMatrices(){
	if(cameraType == CAMERA_FOLLOW || cameraType == EYE || CAMERA_QUATERNION_BASED){
		ViewMatrix = glm::translate(identity, glm::vec3(0.f, 0.f, -distance));
		ViewMatrix *= rotMatrix*glm::translate(glm::mat4(1.0f), camOffset-cam_position);
		eyePosition = glm::affineInverse(ViewMatrix)*P0;
	}
	if(cameraType == CAMERA_SNIPER){

		ViewMatrix = identity;
		ViewMatrix *= glm::rotate(identity, PI/2, glm::vec3(1,0,0))*glm::rotate(identity, PI, glm::vec3(0,1,0))*glm::rotate(identity, PI, glm::vec3(0,0,1));
		ViewMatrix *= rotMatrix;
		// ViewMatrix *= glm::translate(identity, -cam_position);
		// ViewMatrix *= glm::translate(identity, (glm::affineInverse(Transformation)*cam_position).xyz());
		ViewMatrix *= glm::affineInverse(Transformation);
		eyePosition = Transformation*P0;
	}
	if(cameraType == CAMERA_FREE){
		ViewMatrix = identity;
		ViewMatrix *= rotMatrix*glm::translate(identity, -glm::vec3(Position.xyz()));
		eyePosition = glm::affineInverse(ViewMatrix)*P0;
	}
	CameraUp = Up = (Y4*ViewMatrix);
	CameraRight = Right = (X4*ViewMatrix);
	CameraNormal = Normal = (Z4*ViewMatrix);
	Up.w = 0;
	Right.w = 0;
	Normal.w = 0;
	CameraUp.w = 0;
	CameraRight.w = 0;
	CameraNormal.w = 0;
	VP = ProjectionMatrix*ViewMatrix;
    invPV = glm::inverse(ProjectionMatrix * ViewMatrix);
	// ViewMatrix = ViewMatrix*glm::scale(identity, glm::vec3(0.3, 0.3, 0.3));
	// NormalMatrix = glm::transpose(glm::inverse(ViewMatrix));
}
void Camera::setProjection(){
	if(cameraProjection == PERSPECTIVE_PROJECTION){
		if(m_angle < 60){
			ProjectionMatrix = glm::perspective(m_angle/m_aspect*PI/180, m_aspect, 1.f, m_far);
		}
		else if(m_angle < 30){

			ProjectionMatrix = glm::perspective(m_angle/m_aspect*PI/180, m_aspect, 10.f, m_far);
		}
		else if(m_angle < 5){

			ProjectionMatrix = glm::perspective(m_angle/m_aspect*PI/180, m_aspect, 100.f, m_far);
		}
		else
			ProjectionMatrix = glm::perspective(m_angle/m_aspect*PI/180, m_aspect, m_near, m_far);
	}
	else
		// ProjectionMatrix = glm::ortho(-5.f*m_aspect, 5.f*m_aspect, -5.f, 5.f, 1.f, 50.f);
		ProjectionMatrix = glm::ortho(-distance*m_aspect, distance*m_aspect, -distance, distance, 1.f, 50.f);

}
/*
ftl        ftr
     fc
fbl       fbr
0         	1

3			2
*/

glm::vec4 plane(glm::vec4 normal, glm::vec4 point){
	return glm::vec4 (glm::normalize(glm::vec3(normal.xyz())), -glm::dot(glm::normalize(normal), point));
}
glm::vec4 plane(glm::vec3 v1,glm::vec3 v2,glm::vec3 v3){
	glm::vec3 normal = glm::normalize(glm::triangleNormal(v1,v2,v3));
	return glm::vec4 (normal, -glm::dot(normal, v3));
}

void Camera::getFrustum(std::vector <glm::vec4> &tab){
	tab.clear();
	glm::vec4 a_near = plane(-Normal, eyePosition-Normal * m_near);
	glm::vec4 a_far = plane(Normal, eyePosition-Normal * m_far);

	float w_far = tan(m_angle*PI/180*0.5f)*m_far;
	float h_far = w_far/m_aspect;
	glm::vec4 fc = -Normal*m_far+eyePosition;

	glm::vec3 eye = eyePosition.xyz();
	glm::vec3 v0 = (fc-	Up*h_far+	Right*w_far).xyz();
	glm::vec3 v1 = (fc-	Up*h_far-	Right*w_far).xyz();
	glm::vec3 v2 = (fc+	Up*h_far-	Right*w_far).xyz();
	glm::vec3 v3 = (fc+	Up*h_far+	Right*w_far).xyz();

	glm::vec4 left = plane(v1, v2, eye);
	glm::vec4 right = plane(v3, v0, eye);
	glm::vec4 down = plane(v0, v1, eye);
	glm::vec4 up = plane(v2, v3, eye);

	tab.push_back(a_near);
	tab.push_back(a_far);
	tab.push_back(left);
	tab.push_back(right);
	tab.push_back(down);
	tab.push_back(up);
	tab.push_back(eyePosition);
	// glm::vec4 middle = (fc + glm::vec4(eye,0.f))/2.f;
	tab.push_back(-Normal*m_far*0.5f+eyePosition);
}
void Camera::getFrustum(std::vector <glm::vec4> &tab, float p_near, float p_far){
	tab.clear();
	glm::vec4 a_near = plane(-Normal, eyePosition-Normal * p_near);
	glm::vec4 a_far = plane(Normal, eyePosition-Normal * p_far);

	float w_far = tan(m_angle*PI/180*0.5f)*p_far;
	float h_far = w_far/m_aspect;
	glm::vec4 fc = -Normal*p_far+eyePosition;

	glm::vec3 eye = eyePosition.xyz();
	glm::vec3 v0 = (fc-	Up*h_far+	Right*w_far).xyz();
	glm::vec3 v1 = (fc-	Up*h_far-	Right*w_far).xyz();
	glm::vec3 v2 = (fc+	Up*h_far-	Right*w_far).xyz();
	glm::vec3 v3 = (fc+	Up*h_far+	Right*w_far).xyz();

	glm::vec4 left = plane(v1, v2, eye);
	glm::vec4 right = plane(v3, v0, eye);
	glm::vec4 down = plane(v0, v1, eye);
	glm::vec4 up = plane(v2, v3, eye);

	tab.push_back(a_near);
	tab.push_back(a_far);
	tab.push_back(left);
	tab.push_back(right);
	tab.push_back(down);
	tab.push_back(up);
	tab.push_back(eyePosition);
	// glm::vec4 middle = (fc + glm::vec4(eye,0.f))/2.f;
	tab.push_back(-Normal*p_far*0.5f+eyePosition);
}
void Camera::getFrustumZeroPosition(std::vector <glm::vec4> &tab){
	tab.clear();
	glm::vec4 fakePosition(0,0,eyePosition.z,1);

	glm::vec4 a_near = plane(-Normal, fakePosition-Normal * m_near);
	glm::vec4 a_far = plane(Normal, fakePosition-Normal * m_far);

	float w_far = tan(m_angle*PI/180*0.5f)*m_far;
	float h_far = w_far/m_aspect;
	glm::vec4 fc = -Normal*m_far+fakePosition;

	glm::vec3 eye = fakePosition.xyz();
	glm::vec3 v0 = (fc-	Up*h_far+	Right*w_far).xyz();
	glm::vec3 v1 = (fc-	Up*h_far-	Right*w_far).xyz();
	glm::vec3 v2 = (fc+	Up*h_far-	Right*w_far).xyz();
	glm::vec3 v3 = (fc+	Up*h_far+	Right*w_far).xyz();

	glm::vec4 left = plane(v1, v2, eye);
	glm::vec4 right = plane(v3, v0, eye);
	glm::vec4 down = plane(v0, v1, eye);
	glm::vec4 up = plane(v2, v3, eye);

	tab.push_back(a_near);
	tab.push_back(a_far);
	tab.push_back(left);
	tab.push_back(right);
	tab.push_back(down);
	tab.push_back(up);
	tab.push_back(fakePosition);
	// glm::vec4 middle = (fc + glm::vec4(eye,0.f))/2.f;
	tab.push_back(-Normal*m_far*0.5f+fakePosition);//14
}
glm::vec4 Camera::getPosition(){
	return glm::inverse(ViewMatrix)*glm::vec4(0,0,0,1);

}
glm::vec4 Camera::getMouseLookAt(){
	return Normal;
}

glm::vec4 Camera::getMouseRay(glm::vec2 positionDelta){
	if(cameraProjection == PERSPECTIVE_PROJECTION){
	glm::vec3 screen((mousePosition.x + positionDelta.x), (mousePosition.y + positionDelta.y), m_near);
	glm::vec4 viewport(0,0,window_width, window_height);

	return -glm::normalize(eyePosition - glm::vec4(glm::unProject(screen, ViewMatrix, ProjectionMatrix, viewport), 1));
	}
	else {
		return Normal;
	}
}
glm::vec4 Camera::getMouseRay(){
	if(cameraProjection == PERSPECTIVE_PROJECTION){
	// MouseRay = glm::normalize(Right*(float)(mousePosition.x/window_width*2.f) + Up*(float)(mousePosition.y/window_height*2.f) -Normal);

	glm::vec3 screen(mousePosition.x, mousePosition.y, m_near);
	// glm::vec3 screen(mousePosition.x, mousePosition.y, 1.0);
	glm::vec4 viewport(0,0,window_width, window_height);

	return -glm::normalize(eyePosition - glm::vec4(glm::unProject(screen, ViewMatrix, ProjectionMatrix, viewport), 1));
	}
	else {
		return Normal;
	}
}
glm::vec4 Camera::getScreenPointRay(float x, float y){
		glm::vec3 screen(x, y, m_near);
	// glm::vec3 screen(mousePosition.x, mousePosition.y, 1.0);
	glm::vec4 viewport(0,0,window_width, window_height);

	return -glm::normalize(eyePosition - glm::vec4(glm::unProject(screen, ViewMatrix, ProjectionMatrix, viewport), 1));
}

glm::vec4 Camera::eyePlaneIntersection(const glm::vec4 &plane, glm::vec2 positionDelta){
	glm::vec4 startPosition;
	if(cameraProjection == PERSPECTIVE_PROJECTION){
		startPosition = eyePosition;
	}
	else {
		float x = distance*m_aspect * mousePosition.x/window_width - distance*m_aspect*0.5f;
		float y = distance*mousePosition.y/window_height - distance*0.5f;
		startPosition = eyePosition+Right*x + Up*y;
	}

	float distance = glm::dot(startPosition, plane);
	auto ray = getMouseRay(positionDelta);
	return startPosition + ray*distance/glm::dot(ray.xyz(), -plane.xyz());
}
/**
 *  Przeciêcie wektora oko-mysz z p³aszczyzn¹
 */
glm::vec4 Camera::eyePlaneIntersection(const glm::vec4 &plane){
	glm::vec4 startPosition;
	if(cameraProjection == PERSPECTIVE_PROJECTION){
		startPosition = eyePosition;
	}
	else {
		float x = 5.f*m_aspect * mousePosition.x/window_width/2;
		float y = 5.f * mousePosition.y/window_height/2;
		startPosition = eyePosition+Right*x + Up*y;
	}

	float distance = glm::dot(startPosition, plane);
	auto ray = getMouseRay();
	return startPosition + ray*distance/glm::dot(ray.xyz(), -plane.xyz());

}
/**
 *  P³aszczyzna przechodzaca przez dany punkt, równoleg³a do kamery
 */
glm::vec4 Camera::surfaceNormalToCamera(const glm::vec4 &point){
	return glm::vec4(glm::vec3(Normal.xyz()), -glm::dot(Normal, point));
}
// wyciagn¹c rot_x, i _z z kwarternionu
void Camera::setOrientation(float new_x, float new_z){
	rot_x = new_x;
	rot_z = new_z;
}
void Camera::setNormal(glm::quat newQuat){
	mainQuat = glm::normalize(newQuat);
	// rot_x = glm::yaw(newQuat);
	// rot_z = glm::pitch(newQuat);
	// rotMatrix = glm::mat4_cast(newQuat);
}

glm::mat4 Camera::getFarProjection(float p_near, float p_far){
	return glm::perspective(m_angle/m_aspect*PI/180, m_aspect, p_near, p_far);
}