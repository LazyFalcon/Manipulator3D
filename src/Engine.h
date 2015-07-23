#pragma once
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/Camera.h>
#include <Utils/MeshInfo.h>
#include <Utils/Camera.h>
#include <Utils/IMGUI_V4.h>
#include <Utils/glUtils.h>
#include "Common.h"
#include "Robot.h"
#include "CFGParser.h"
#include "ResourceLoader.h"
#include "Engine-DrawUtils.h"

#define NAM_START {
#define NAM_END }

extern HexColor LightText;
extern HexColor DarkText;
extern HexColor GUIbackground[4];
extern HexColor GUIeditBox[4];
extern HexColor GUIhover[4];
extern HexColor GUIfontColor[4];
extern HexColor GUIspecColor[4];




enum {
	LIGHTS =            0b1,
	SHADOWS =           0b10,
	WIREFRAME =         0b100,
	GOD_RAYS =          0b1000,
	MSAA =              0b10000,
	HDR =               0b100000,
	DOF =               0b1000000,
	SSAO =              0b10000000,
	SOBEL =             0b100000000,
	PREVENT_COLLISIONS =0b1000000000,
	DRAW_COLORS =       0b10000000000,
	DRAW_NORMALS =      0b100000000000,
	DRAW_DEPTH =        0b1000000000000,
	DRAW_LIGHTS =       0b10000000000000,
	DRAW_XY_GRID =      0b100000000000000,
	DRAW_XZ_GRID =      0b1000000000000000,
	DRAW_YZ_GRID =      0b10000000000000000,
};


namespace Engine NAM_START
extern GLuint b_position;
extern GLuint b_uv;
extern GLuint b_size;
extern GLuint b_color;
extern GLuint b_grid;
extern GLuint b_guiRects;
extern GLuint b_robotPath;
extern GLuint b_tmp;

struct XORBuffer {
	GLuint a,b;
	uint32_t current;
	void init(int count){
		genVoidBuffer(a, count);
		genVoidBuffer(b, count);
		current = a;
	}
	void update(vector<glm::vec4> &data){

		std::swap(a,b);
		current = a;
		updateBuffer(current, data);
	}
	void update(vector<float> &data){

		std::swap(a,b);
		current = a;
		updateBuffer(current, data);
	}
	void bind( int numBuffer=0, int dataSize=4, int divisor=0, GLenum type = GL_FLOAT){
		setupBuffer(current, numBuffer, dataSize, divisor, type);
	}

};
extern XORBuffer b_universalVec4;
extern XORBuffer b_universalLong;

template<typename DataType>
struct UniversalCyclicBuffer {
    GLuint glBuffer[3];
    int currentID = 0;
    GLuint current;
    void init(int maxDataNumber){
        genVoidBuffer(glBuffer[0], maxDataNumber);
        genVoidBuffer(glBuffer[1], maxDataNumber);
        genVoidBuffer(glBuffer[2], maxDataNumber);
        current = glBuffer[0];
    }
    void update(vector<DataType> &data){
				currentID = (currentID+1)%3;
        current = glBuffer[currentID];
        updateBuffer(current, data);
    }
    void update(vector<DataType> &data, uint32_t from, uint32_t to){
				currentID = (currentID+1)%3;
        current = glBuffer[currentID];
        updateBuffer(current, data, from, to);
    }
    void bind(int numBuffer=0, int dataSize=4, int divisor=0, GLenum type = GL_FLOAT, bool normalize = false){
        setupBuffer(current, numBuffer, dataSize, divisor, type, normalize);
    }
};

extern UniversalCyclicBuffer <glm::vec4>b_vec4_1024;
extern UniversalCyclicBuffer <float>b_float_1024;
extern UniversalCyclicBuffer <HexColor>b_color_1024;

extern GLuint point_VBO;
extern GLuint quadCentered;
extern GLuint quadCorner;
extern GLuint quadUV_VBO;
extern GLuint screenQuad;
extern GLuint XLongQuad;
extern GLuint XLongQuad2;
extern GLuint tankUBO;
extern int g_pointsSize;

extern GLuint FBO[3];
extern GLuint frameBuffer[2];
extern GLuint colorBuffer;
extern GLuint normalBuffer;
extern GLuint depthBuffer;
extern GLuint depthBuffer2;
extern GLuint stencilBuffer;
extern GLuint lightBuffer;
extern GLenum DrawBuffers[2];


struct shapeInfo {
	glm::vec4 info4;
	glm::vec2 info2;
	GLuint shader;
	GLuint tex;
	HexColor color;
	string text;
};
struct lineInfo {
	glm::vec2 start;
	glm::vec2 end;
	int size;
	HexColor color;
};
struct menuParticleInfo {
	glm::vec4 normal;
	glm::vec4 position;
	glm::vec4 size;
	HexColor color;
	GLuint tex;
};
extern std::vector <menuParticleInfo> surfaceAlignedParticles;
extern std::vector<shapeInfo> point2D;
extern std::vector<shapeInfo> shape2D;
extern std::vector<shapeInfo> mask2D;
extern std::vector<shapeInfo> filledShape2D;
extern std::vector<shapeInfo> text2D;
extern std::vector<shapeInfo> pattern2D;
extern std::vector<lineInfo> 	lines2D;

void genVao(vector<float>vertices, vector<float>uvs, vector<float>normals, vector<int32_t>indices, unique_ptr<Resources> &res);

/// Debug drawing, push to namespace
void drawArrow(glm::vec4 vec, HexColor color, std::function<void(float)> callback);
void drawRotArrow(glm::vec4 vec, HexColor color, std::function<void(float)> callback);
void drawJoint();
void drawBBox();

/// utils
glm::vec4 perpendicular(glm::vec4 v);
glm::mat4 perpendicularTransform(glm::vec4 v, glm::vec4 p);
void init(CFG::Node &cfg);
void clear();

/// Drawing things
void plotGraphs();
void generateShadowMap(Scene &scene);
void setup(Scene &scene);
void renderScene(Scene &scene);
void drawOutline(Scene &scene);

void copyDepth(Scene &scene);
void renderLights(Scene &scene);
void applyLights(Scene &scene);
void SSAO();
void Sobel();
void postprocess(Scene &scene);
void finalize(Scene &scene);

void initGrids();
void drawGrids();
void renderGUI(UI::IMGUI &gui);
void renderShapes();




bool initGlew();

NAM_END

