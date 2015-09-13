#pragma once
//jointed a sniper maj¹ odwrócone xz
enum CameraType {CAMERA_FREE, CAMERA_FOLLOW, CAMERA_SNIPER, EYE, CAMERA_QUATERNION_BASED, PERSPECTIVE_PROJECTION, ORTHO_PROJECTION};
class Camera {
public:
	glm::vec3 Z3{0,0,1};
	glm::vec3 Y3{0,1,0};
	glm::vec3 X3{1,0,0};
	glm::vec4 Z4{0,0,1,0};
	glm::vec4 Y4{0,1,0,0};
	glm::vec4 X4{1,0,0,0};
	glm::vec4 P0{0,0,0,1};

	CameraType cameraType;
	CameraType cameraProjection;

	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	glm::mat4 NormalMatrix;
	glm::mat4 invPV;

	glm::vec3 cam_position;
	glm::vec3 camOffset;
	glm::vec4 Position;
	glm::mat4 VP;
	glm::mat4 rotMatrix;
	glm::mat4 Transformation;
	glm::vec4 Up;
	glm::vec4 Normal;
	glm::vec4 Right;
	glm::vec4 eyePosition;
	glm::vec4 Velocity;
	glm::vec4 Omega;
	glm::vec4 MouseRay;
	float dx {0};
	float dy {0};
	float sensivity;
	float distance {10.f};
	float rot_z {0.2f};
	// float rot_x {0.1f};
	float rot_x {1.4f};
	float zoom_vel {0.f};
	float m_near {0.01f};
	float m_far {2000.f};
	float m_aspect;
	float m_angle {90.f};
	float m_width;
	float m_height;
	void getFrustum(glm::vec4 *tab, bool reset){}
	void getFrustum(std::vector <glm::vec4> &tab);
	void getFrustum(std::vector <glm::vec4> &tab, float near, float far);
	void getFrustumZeroPosition(std::vector <glm::vec4> &tab);
	glm::mat4 getFarProjection(float p_near, float p_far);
	void changeView();
	void updateCamera(float step);
	void moveCamera(float moveFront, float moveUP, float rotateZ, float rotateY, float roll);
	void setMatrices();
	void setProjection();
	void zoom(int x);//w przód-, w ty³+
	void zoomStep(float x);//w przód-, w ty³+
	glm::vec4 getMouseLookAt();
	glm::vec4 getPosition();
	void setCamPosition(glm::mat4 a);
	void setCamPosition(glm::vec3 a);
	void recalculateProjection();
	void setMouseMov(float _dx, float _dy);
	void setNormal(glm::quat newQaut);
	void setOrientation(float new_x, float new_z);
	glm::vec4 getMouseRay(glm::vec2 positionDelta);
	glm::vec4 getMouseRay();
	glm::vec4 getScreenPointRay(float x, float y);
	glm::vec4 eyePlaneIntersection(const glm::vec4 &plane, glm::vec2 positionDelta);
	glm::vec4 eyePlaneIntersection(const glm::vec4 &plane);
	glm::vec4 surfaceNormalToCamera(const glm::vec4 &point);
	Bezier <float> m_rotBezier;
	void init();
	float height = 2;

	float freeCamAngle = 90;
	float followCamAngle = 90;
	float sniperCamAngle = 90;

	glm::quat mainQuat;
};
