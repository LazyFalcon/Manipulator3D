#pragma once
// tu powinny trafic wszystkie globale dzielone pommiędzy wszystkimi gównami

extern glm::vec4 		viewCenter;
extern glm::vec3 		Z;
extern glm::vec3 		X;
extern glm::vec3 		Y;
extern glm::vec4 		Zz;
extern glm::vec4 		Xx;
extern glm::vec4 		Yy;
extern glm::vec4 		CameraUp;
extern glm::vec4 		CameraRight;
extern glm::vec4 		CameraNormal;
extern const float 	PI;
extern const float 	pi;
extern const float 	toRad;
extern bool 				GUI;
extern glm::mat4 		identity;
extern bool 				rClick;
extern float				scrollPos;
extern float				scrollDel;
extern float				scrollMov;
extern float				mouse_x, mouse_y;
extern glm::vec2 		mousePosition;
extern bool					LeftMousePressed;
extern bool					RightMousePressed;
extern bool					signal10ms;
extern glm::mat4 		orthoMatrix;
extern GLFWwindow 	*window;
extern float 					window_width, window_height;
extern bool 				quit;
extern int64_t 			globalSettings;


class PositionSaver {
public:
	std::vector<glm::vec4> positions;
	int size;
	int counter;
	PositionSaver(int x): positions(x), size(x), counter(0){};
	void operator()(const glm::vec4 &vec){
		positions[counter++] = vec;
		counter = counter%size;
	}

};

class LineBuffer {
public:
	std::vector<glm::vec4> points;
	int size;
	int counter;
	LineBuffer(int x): points(x), size(x), counter(0){};
	void operator()(const glm::vec4 &p1, const glm::vec4 &p2){
		points[counter++] = p1;
		points[counter++] = p2;
		counter = counter%size;
	}
};