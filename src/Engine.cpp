#define GLEW_STATIC
#include <GL/glew.c>

#include "Engine.h"
#include "glUtils.h"
#include "Graph.h"
#include "Helper.h"

#define _DebugLine_  std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";
#define NAM_END }
#define NAM_START {

extern std::unordered_map<std::string, uint32_t>shaders;
extern Resources *globalResources;
extern Camera camera;
extern Graph mousePositionGraphX;
extern Graph mousePositionGraphY;
extern glm::vec2 screenSize;

HexColor colorTransparent	= 0xff0000ff;
HexColor colorWhite				= 0xf0f0f0ff;
HexColor colorWhite80 		= 0xf0f0f080;
HexColor colorBlack				= 0x000000ff;
HexColor colorWhite_4			= 0x404040ff;
HexColor colorGreen				= 0xff0000ff;
HexColor colorRed					= 0xED1C24ff;
HexColor colorYellow			= 0xFFF200ff;
HexColor colorBlue				= 0x0066B3ff;
HexColor colorOrange			= 0xFAA61Aff;
HexColor colorRedDarker		= 0xBA131Aff;
HexColor GolorUIcolor			= 0x21409Aff;
HexColor GolorUIcolor2		= 0x21409Aff;
HexColor GUIbcgColor			= 0xff0000ff;
HexColor colorClearColor	= 0xff0000ff;
HexColor GUIcolor					= 0xff0000ff;
HexColor GUIcolor2				= 0xff0000ff;
HexColor clearColor				= 0xff0000ff;
HexColor GUIColors[10]		= {0xff0000ff};
HexColor cRed							= 0xED1C24ff;
HexColor cGreen						= 0xff0000ff;
HexColor cBlue						= 0x0066B3ff;

HexColor LightText				= 0x0066B3ff;
HexColor DarkText					= 0x0066B3ff;
HexColor GUIbackground[4]	= {0x0066B3ff};
HexColor GUIeditBox[4]		= {0x0066B3ff};
HexColor GUIhover[4]			= {0x0066B3ff};
HexColor GUIfontColor[4]	= {0x0066B3ff};
HexColor GUIspecColor[4]	= {0x0066B3ff};


PositionSaver g_mousePositions(1);
PositionSaver g_robotPositions(100);
PositionSaver g_robotDebugPositions(100);
LineBuffer		g_lines(100);


namespace Engine NAM_START

GLuint b_position;
GLuint b_uv;
GLuint b_size;
GLuint b_color;
GLuint b_grid = 0;
GLuint b_guiRects;
GLuint b_robotPath;
GLuint b_tmp;

XORBuffer b_universalVec4;
XORBuffer b_universalLong;
UniversalCyclicBuffer <glm::vec2>b_vec2_16k;
UniversalCyclicBuffer <glm::vec4>b_vec4_1024;
UniversalCyclicBuffer <float>b_float_1024;
UniversalCyclicBuffer <HexColor>b_color_1024;

GLuint point_VBO;
GLuint quadCentered;
GLuint quadCorner;
GLuint quadUV_VBO;
GLuint screenQuad;
GLuint XLongQuad;
GLuint XLongQuad2;
GLuint tankUBO;
int g_pointsSize;

GLuint fullFBO;
GLuint halfFBO;
#include "TextureObject.h"

/// buffers
Texture objectIDTex_R16;
Texture full_R8;
Texture full_R8_2;
Texture half_R8;
Texture half_R8_2;
Texture full_RGBA8;
Texture full_RGBA8_2;
Texture half_RGBA8;
Texture half_RGBA8_2;
/// main targets
Texture full_RGBA16F;
Texture normalBuffer;
Texture colorBuffer;
Texture depthBuffer;
Texture depthBuffer2;
Texture shadowMapBuffer;

GLuint shadowMapFbo;
GLuint PlotFBO;
GLuint plotColorBuffer;

GLenum DrawBuffers[5];

const u32 shadowMapSize = 1024;

vector <menuParticleInfo> surfaceAlignedParticles;
vector<shapeInfo> point2D;
vector<shapeInfo> shape2D;
vector<shapeInfo> mask2D;
vector<shapeInfo> filledShape2D;
vector<shapeInfo> text2D;
vector<shapeInfo> pattern2D;
vector<lineInfo> 	lines2D;
vector<std::pair<GLuint, glm::vec4>> texturedBoxes;

const u32 g_lightsToForward = 2;


void genVao(vector<float>vertices, vector<float>uvs, vector<float>normals, vector<int32_t>indices, shared_ptr<Resources> &res){

	glGenVertexArrays(1, &(res->VAO));
	glBindVertexArray(res->VAO);
	GLuint *modelVBO = res->VBO;
	glGenBuffers(4, modelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), uvs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);
	glGenBuffers(1, &(res->IBO));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, res->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(),indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}
void drawPattern2D(glm::vec2 position, glm::vec2 info, string name, HexColor color){
	shapeInfo sinfo;
	auto image = globalResources->images[name];

	sinfo.info4 = glm::vec4(position.x, position.y, image.w, image.h);
	sinfo.info2 = info;
	sinfo.color = color;
	sinfo.tex = image.ID;
	pattern2D.push_back(sinfo);
}
void drawShape2D(glm::vec4 info, string shape, HexColor color){
	shapeInfo sinfo;
	sinfo.info4 = info;
	sinfo.shader = shaders[shape];
	sinfo.color = color;
	shape2D.push_back(sinfo);


}
void drawMask2D(glm::vec4 info, string tex, HexColor color){
	shapeInfo sinfo;
	sinfo.info4 = info;
	sinfo.tex = globalResources->textures[tex];
	sinfo.color = color;
	mask2D.push_back(sinfo);


}
void drawPoint2D(glm::vec4 info, string shape, HexColor color){
	shapeInfo sinfo;
	sinfo.info4 = info;
	sinfo.shader = shaders[shape];
	sinfo.color = color;
	shape2D.push_back(sinfo);
}
void drawFilledShape2D(glm::vec4 info, string shape, HexColor color){
	shapeInfo sinfo;
	sinfo.info4 = info;
	sinfo.shader = shaders[shape];
	sinfo.color = color;
	filledShape2D.push_back(sinfo);
}
void drawText2D(glm::vec2 info, string text, string font, HexColor color){
	UI::fonts[font].push(text, info, color);
}
void drawText2D(glm::vec2 info, string text, UI::FontRenderer &fontManager, HexColor color){}

void drawStrings(vector <UI::TextInfo> m_strings){}
// rysuje strza³kê, wykrywa jej z³apanie i przesuniêcie i przekazuje do lambdy; w stylu IMGUI
void drawArrow(glm::vec4 vec, HexColor color, std::function<void(float)> callback){

	callback(0.f);
}
void drawRotArrow(glm::vec4 vec, HexColor color, std::function<void(float)> callback){

	callback(0.f);
}
void drawJoint(){}
void drawBBox(){}
glm::vec4 perpendicular(glm::vec4 v){
	if(v.y == 0)
		return glm::vec4(v.z,0,-v.x, 0);
	if(v.z == 0)
		return glm::vec4(v.y,-v.x, 0, 0);
	return glm::vec4(0,v.z,-v.y, 0);
}
glm::mat4 perpendicularTransform(glm::vec4 v, glm::vec4 p){
	glm::vec4 V = glm::normalize(perpendicular(v));
	return glm::mat4(V, cross(V, v), v, p);
}

void resize(){
{ /// textures
	/// buffers
	objectIDTex_R16 = Texture {GL_R16, window_width, window_height, GL_RED, GL_UNSIGNED_SHORT, GL_NEAREST, 0};
	full_R8 = Texture {GL_R8, window_width, window_height, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	full_R8_2 = Texture {GL_R8, window_width, window_height, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	half_R8 = Texture {GL_R8, window_width/2, window_height/2, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	half_R8_2 = Texture {GL_R8, window_width/2, window_height/2, GL_RED, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	full_RGBA8 = Texture {GL_RGBA8, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	full_RGBA8_2 = Texture {GL_RGBA8, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	half_RGBA8 = Texture {GL_RGBA8, window_width/2, window_height/2, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	half_RGBA8_2 = Texture {GL_RGBA8, window_width/2, window_height/2, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	/// main targets
	full_RGBA16F = Texture {GL_RGBA16F, window_width, window_height, GL_RGBA, GL_HALF_FLOAT, GL_LINEAR, 0};
	normalBuffer = Texture {GL_RGBA16F, window_width, window_height, GL_RGBA, GL_HALF_FLOAT, GL_LINEAR, 0};
	colorBuffer = Texture {GL_RGBA8, window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, 0};
	depthBuffer = Texture {GL_DEPTH24_STENCIL8, window_width, window_height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, GL_LINEAR, 0};
	depthBuffer2 = Texture {GL_R32F, window_width, window_height, GL_RED, GL_FLOAT, GL_LINEAR, 0};
	shadowMapBuffer = Texture {GL_DEPTH_COMPONENT32, shadowMapSize, shadowMapSize, GL_DEPTH_COMPONENT, GL_FLOAT, GL_LINEAR, 0};

	full_R8.generate();
	full_R8_2.generate();
	half_R8.generate();
	half_R8_2.generate();
	full_RGBA8.generate();
	full_RGBA8_2.generate();
	half_RGBA8.generate();
	half_RGBA8_2.generate();

	full_RGBA16F.generate();
	normalBuffer.generate();
	colorBuffer.generate();
	depthBuffer2.generate();
	glGenTextures(1, &depthBuffer.ID);
	glGenTextures(1, &shadowMapBuffer.ID);

	glGenTextures(1, &depthBuffer.ID);
	glBindTexture(GL_TEXTURE_2D, depthBuffer.ID);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, window_width, window_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glGenTextures(1, &shadowMapBuffer.ID);
	glBindTexture(GL_TEXTURE_2D, shadowMapBuffer.ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri( GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}
{ /// FBOs

	//----------------
	auto FBstatus = [](){
		GLenum framebufferStatus = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			switch (framebufferStatus) {
					case GL_FRAMEBUFFER_COMPLETE_EXT:
							std::cout<<"FRAMEBUFFER Object  OK"<<endl; break;
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
							std::cout<<"FRAMEBUFFER Object %d Error: Attachment Point Unconnected"<<endl;
							break;
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
							std::cout<<"FRAMEBUFFER Object %d Error: Missing Attachment"<<endl;
							break;
					case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
							std::cout<<"FRAMEBUFFER Object  Error: Dimensions do not match"<<endl;
							break;
					case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
							std::cout<<"FRAMEBUFFER Object  Error: Formats"<<endl;
							break;
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
							std::cout<<"FRAMEBUFFER Object  Error: Draw Buffer"<<endl;
							break;
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
							std::cout<<"FRAMEBUFFER Object  Error: Read Buffer"<<endl;
							break;
					case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
							std::cout<<"FRAMEBUFFER Object  Error: Unsupported FRAMEBUFFER Configuration"<<endl;
							break;
					default:
							std::cout<<"framebuffer Object  Error: Unkown framebuffer Object Failure"<<endl;
							break;
			}
		};

	DrawBuffers[0] = GL_COLOR_ATTACHMENT0;
	DrawBuffers[1] = GL_COLOR_ATTACHMENT1;
	DrawBuffers[2] = GL_COLOR_ATTACHMENT2;
	glDrawBuffers(3, DrawBuffers);

	glGenFramebuffers(1, &fullFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, fullFBO);
		glViewport(0, 0, screenSize.x, screenSize.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer.ID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer.ID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.ID, 0);
		glDrawBuffers(2, DrawBuffers);
		FBstatus();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// glGenFramebuffers(1, &shadowMapFbo);
		// glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFbo);
		// glViewport(0, 0, shadowMapSize, shadowMapSize);
		// glDrawBuffers(0, DrawBuffers);
		// glEnable(GL_DEPTH_TEST);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, shadowMapBuffer.ID, 0);
		// FBstatus();
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &halfFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, halfFBO);
		glDrawBuffers(1, DrawBuffers);
		glViewport(0, 0, screenSize.x/2.f, screenSize.y/2.f);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, half_RGBA8.ID, 0);
		FBstatus();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


	initPlotList();
	// glBindRenderbuffer(GL_RENDERBUFFER, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	int error = glGetError();
	if(error != GL_NO_ERROR)
		std::cout<<__LINE__<<""<<error<<endl;
}
}
void init(CFG::Node &cfg){
{
		// genVoidBuffer(linesVBO, 3*1024);
		genVoidBuffer(b_position, 2*4096);
		genVoidBuffer(b_size, 2*4096);
		genVoidBuffer(b_color, 4*4096);
		genVoidBuffer(b_uv, 4*4096);
		genVoidBuffer(b_guiRects, 512*4);
		genVoidBuffer(b_robotPath, 512*4);
		genVoidBuffer(b_tmp, 512*4);
		b_universalVec4.init(16384*4);
		b_universalLong.init(32768*4);
		b_vec2_16k.init(0xFFFF*2);
	}
{ /// VBOs
		float point[] = {
					0.0f, 0.0f, 0.5f, 1.f,
					};
		float quad_centered[] = {
					-0.5f, -0.5f, 0.0f, 1.f,
					-0.5f, 0.5f, 0.0f, 1.f,
					0.5f, -0.5f, 0.0f, 1.f,
					0.5f, 0.5f, 0.0f, 1.f,
			};
		float quad_corner[] = {
					0.f, 0.f, 0.5f, 1.f,
					0.f, 1.f, 0.5f, 1.f,
					1.f, 0.f, 0.5f, 1.f,
					1.f, 1.f, 0.5f, 1.f,
			};
		float fullScreenQuad[] = {
				-1.f, -1.f,0.f, 0.f,
				-1.f, 1.f, 0.f, 1.f,
				1.f, -1.f, 1.f, 0.f,
				1.f, 1.f,  1.f, 1.f,
			};
		float quadUV[] = {
				0.f, 0.f,
				0.f, 1.f,
				1.f, 0.f,
				1.f, 1.f,
			};
		float quad_v1[] = {
					0.0f, 0.25f, 0.0f, 1.f,
					0.0f, -0.25f, 0.0f, 1.f,
					0.5f, 0.25f, 0.0f, 1.f,
					0.5f, -0.25f, 0.0f, 1.f,
			};
		float quad_x_long[] = {
					0.0f, 0.5f, 0.0f, 1.f,
					0.0f, -0.5f, 0.0f, 1.f,
					1.0f, 0.5f, 0.0f, 1.f,
					1.0f, -0.5f, 0.0f, 1.f,
			};
		glGenBuffers(1, &point_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, point_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*4, point, GL_STATIC_DRAW);

		glGenBuffers(1, &quadCentered);
			glBindBuffer(GL_ARRAY_BUFFER, quadCentered);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, quad_centered, GL_STATIC_DRAW);

		glGenBuffers(1, &quadCorner);
			glBindBuffer(GL_ARRAY_BUFFER, quadCorner);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, quad_corner, GL_STATIC_DRAW);

		glGenBuffers(1, &screenQuad);
			glBindBuffer(GL_ARRAY_BUFFER, screenQuad);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, fullScreenQuad, GL_STATIC_DRAW);

		glGenBuffers(1, &XLongQuad);
			glBindBuffer(GL_ARRAY_BUFFER, XLongQuad);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, quad_v1, GL_STATIC_DRAW);
		glGenBuffers(1, &XLongQuad2);
			glBindBuffer(GL_ARRAY_BUFFER, XLongQuad2);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*16, quad_x_long, GL_STATIC_DRAW);

		glGenBuffers(1, &quadUV_VBO);
			glBindBuffer(GL_ARRAY_BUFFER, quadUV_VBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*8, quadUV, GL_STATIC_DRAW);
}
    resize();
}
void clear(){
	glDeleteBuffers(1, &b_position);
	glDeleteBuffers(1, &b_size);
	glDeleteBuffers(1, &b_color);
	glDeleteBuffers(1, &b_uv);
	glDeleteBuffers(1, &b_guiRects);
	glDeleteBuffers(1, &point_VBO);
	glDeleteBuffers(1, &quadCentered);
	glDeleteBuffers(1, &quadCorner);
	glDeleteBuffers(1, &screenQuad);
	glDeleteBuffers(1, &XLongQuad);
	glDeleteBuffers(1, &XLongQuad2);
	glDeleteBuffers(1, &quadUV_VBO);
	glDeleteFramebuffers(1, &fullFBO);
	glDeleteFramebuffers(1, &halfFBO);
	glDeleteTextures(1, &full_RGBA8.ID);
	glDeleteTextures(1, &colorBuffer.ID);
	glDeleteTextures(1, &normalBuffer.ID);
	glDeleteTextures(1, &depthBuffer.ID);

	for(auto &it : shaders)
		glDeleteShader(it.second);
}
void clearScreenBuffers(){
	glDeleteFramebuffers(1, &fullFBO);
	glDeleteFramebuffers(1, &halfFBO);
	glDeleteTextures(1, &full_R8.ID);
	glDeleteTextures(1, &full_R8_2.ID);
	glDeleteTextures(1, &half_R8.ID);
	glDeleteTextures(1, &full_RGBA8.ID);
	glDeleteTextures(1, &half_R8_2.ID);
	glDeleteTextures(1, &full_RGBA8_2.ID);
	glDeleteTextures(1, &half_RGBA8.ID);
	glDeleteTextures(1, &half_RGBA8_2.ID);

	glDeleteTextures(1, &full_RGBA16F.ID);
	glDeleteTextures(1, &normalBuffer.ID);
	glDeleteTextures(1, &colorBuffer.ID);
	glDeleteTextures(1, &depthBuffer.ID);
	glDeleteTextures(1, &depthBuffer2.ID);

}

void plotGraphs(){
	drawPlotList();
}

#include "Engine-Ilumination.h"

glm::mat4 shadowProjection;
void generateShadowMap(Scene &scene){

	{
		int error = glGetError();
		if(error != GL_NO_ERROR)
			std::cout<<__LINE__<<""<<error<<endl;
	}


	// glUniform(shader, scene.pointLamps[0].position, "uLightPos");
	// glUniform(shader, scene.pointLamps[0].color, "uColor");
	// glUniform(shader, scene.pointLamps[0].falloffDistance, "uSize");
	// glUniform(shader, scene.pointLamps[0].energy, "uEnergy");

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapBuffer.ID, 0);
	glViewport(0, 0, shadowMapSize, shadowMapSize);

	glStencilFunc(GL_ALWAYS,1,0xFF);
	glClearDepth(1);
	glClearStencil(0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);
	glDisable(GL_BLEND);

	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
	// glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	auto &lamp = scene.pointLamps[0];
	glm::vec4 lightVector = glm::vec4(0,0,0,1) - lamp.position; /// w stronę środka
	lightVector.w = 0;

	glm::vec4 lv = glm::normalize(lightVector);
	glm::vec4 lvHorizon = glm::normalize(cross(glm::vec4(0,0,1,0), lv));
	glm::vec4 lvVert = glm::normalize(cross(lv, lvHorizon));


	glm::mat4 projection = glm::perspective(1.5f, 1.f, 1.f, 50.f);
	glm::mat4 view = identity;
	view = glm::lookAt(lamp.position.xyz(), lv.xyz(), glm::vec3(0,0,1));

	shadowProjection = projection*view;

	auto shader = shaders["ProjectShadows"];
	glUseProgram(shader);
	static auto u_modelPosition = glGetUniformLocation(shader,"uModel");
	static auto u_shadowProjection = glGetUniformLocation(shader,"uPV");
	glUniform(shader, shadowProjection, u_shadowProjection);
	glBindVertexArray(scene.resources->VAO);
	// glUniform(shader, camera.ProjectionMatrix*camera.ViewMatrix, "u_PV");

	for(auto &entity : scene.units){
		auto &mesh = *(entity.second->mesh);

		glm::mat4 transform = glm::translate(entity.second->position.xyz()) * glm::mat4_cast(entity.second->quat);

		glUniform(shader, transform, u_modelPosition);
		glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, mesh.offset);
	}

	{
		int error = glGetError();
		if(error != GL_NO_ERROR)
			std::cout<<__LINE__<<""<<error<<endl;
	}
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// glCullFace(GL_BACK);
}
void setup(Scene &scene){
	auto ctmp = colorHex(clearColor);
	if(true){ /// default
		glViewport(0, 0, window_width, window_height);
		// glClearColor(0.09f, 0.09f, 0.2f, 1.f);
		glClearColor(ctmp.x, ctmp.y, ctmp.z, 1.f);
		glClearDepth(1);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisableVertexAttribArray(0);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glDisable(GL_BLEND);

		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);
	}

	if(true){ /// FBO
		glBindFramebuffer(GL_FRAMEBUFFER, fullFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer.ID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalBuffer.ID, 0);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.ID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.ID, 0);
			glDrawBuffers(2,&DrawBuffers[0]);
		glClearColor(ctmp.x, ctmp.y, ctmp.z, 0.f);
		glClearDepth(1);
		glClearStencil(0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glViewport(0, 0, window_width, window_height);
		glDisableVertexAttribArray(0);
		glDepthFunc(GL_LEQUAL);
		glDepthRange(0.0f, 1.0f);
		glDisable(GL_BLEND);

		glDepthMask(GL_TRUE);
		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glFrontFace(GL_CCW);
	}
}
void renderScene(Scene &scene){
	glStencilFunc(GL_ALWAYS,1,0xFF);
	auto shader = shaders["SceneElement"];
	glUseProgram(shader);

	static auto uPV = glGetUniformLocation(shader,"uPV");
	static auto uView = glGetUniformLocation(shader,"uView");
	static auto u_metalTex = glGetUniformLocation(shader,"u_metalTex");

	glUniform(shader, camera.ProjectionMatrix, uPV);
	glUniform(shader, camera.ViewMatrix, uView);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(u_metalTex,0);
	glBindTexture(GL_TEXTURE_2D, globalResources->textures["metal"]);
    shadowMapBuffer.bind(1);

	glBindVertexArray(scene.resources->VAO);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	auto u_colorPosition = glGetUniformLocation(shader, "uColor");
	auto u_modelPosition = glGetUniformLocation(shader, "uModel");
	auto u_NMPosition = glGetUniformLocation(shader, "uNM");
	auto uID = glGetUniformLocation(shader, "uID");
	for(auto &entity : scene.units){
		auto &mesh = *(entity.second->mesh);
		glm::mat4 transform;

// #ifdef USE_BULLET
		auto rgBody = entity.second->rgBody;
		if(rgBody && !rgBody->isStaticOrKinematicObject()){
			auto btPos = rgBody->getCenterOfMassTransform().getOrigin();
			entity.second->position = glm::vec4(btPos[0], btPos[1], btPos[2], 1);
			auto btQuat = rgBody->getOrientation();
			entity.second->quat = glm::quat(btQuat.getW(), btQuat.getX(), btQuat.getY(), btQuat.getZ());
			transform = to_mat4(rgBody->getCenterOfMassTransform());
		}
		else
// #endif
			transform = glm::translate(entity.second->position.xyz()) * glm::mat4_cast(entity.second->quat);

		glUniform(shader, entity.second->material.color, u_colorPosition);
		// glUniform(shader, float(entity.second->ID)/65535.f, uID);
		glUniform(shader, float(entity.second->ID), uID);
		glUniform(shader, transform, u_modelPosition);
		if(globalSettings & MSAA)
			glUniform(shader, glm::transpose(glm::inverse(transform)), u_NMPosition);
			// glUniform(shader, glm::transpose(glm::inverse(camera.ViewMatrix*transform)), u_NMPosition);
		else
			// glUniform(shader, camera.ViewMatrix*transform, u_NMPosition);
			glUniform(shader, transform, u_NMPosition);
		glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, mesh.offset);
	}

	// glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, 0, 0);
	glBindVertexArray(0);
	copyDepth(scene);
}
void drawOutline(Scene &scene){

	auto shader = shaders["SceneOutline"];
	glUseProgram(shader);
	static auto uModel = glGetUniformLocation(shader,"uModel");
	static auto uColor = glGetUniformLocation(shader,"uColor");

	auto &objects = Helper::getSelection();

	{ // fill stencil
		// glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.ID, 0);
		glDrawBuffers(0,&DrawBuffers[0]);
		glClear(GL_STENCIL_BUFFER_BIT);
		glClearStencil(0);

		glDrawBuffers(0,&DrawBuffers[0]);
		glClear(GL_STENCIL_BUFFER_BIT);
		glDepthFunc(GL_LEQUAL);
		glDisable(GL_BLEND);
		// glDepthMask(GL_TRUE);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
		// glEnable(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);
		glStencilMask(0x1);
		glStencilFunc(GL_ALWAYS,0x1,0xFF);

		glBindVertexArray(scene.resources->VAO);
		for(auto &obj : objects){
			auto &mesh = *(obj->mesh);

			glm::mat4 transform = camera.ProjectionMatrix*camera.ViewMatrix*glm::translate(obj->position.xyz()) * glm::mat4_cast(obj->quat);

			glUniform(shader, glm::vec4(1,1,0.4  ,1), uColor);
			glUniform(shader, transform, uModel);
			glDrawRangeElements(GL_TRIANGLES, mesh.begin, mesh.end, mesh.count, GL_UNSIGNED_INT, mesh.offset);
		}

		if(Helper::getMarkedModule()>=0){
			glStencilMask(0x2);
			glStencilFunc(GL_ALWAYS,0x2,0x2);
			auto &mesh = *(scene.robot->chain[Helper::getMarkedModule()]->entity->mesh);

			glm::mat4 transform = camera.ProjectionMatrix*camera.ViewMatrix*glm::translate(scene.robot->chain[Helper::getMarkedModule()]->entity->position.xyz()) * glm::mat4_cast(scene.robot->chain[Helper::getMarkedModule()]->entity->quat);

			glUniform(shader, glm::vec4(1,1,0.4  ,1), uColor);
			glUniform(shader, transform, uModel);
			glDrawRangeElements(GL_TRIANGLES, mesh.begin, mesh.end, mesh.count, GL_UNSIGNED_INT, mesh.offset);
		}
	}
	{ // draw outline
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer.ID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.ID, 0);
		glDrawBuffers(1,&DrawBuffers[0]);

		// glDisable(GL_DEPTH_TEST);
		glEnable(GL_DEPTH_TEST);
		glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
		glStencilMask(0x00);
		glStencilFunc(GL_NOTEQUAL,0x1,0x1);
		// glStencilFunc(GL_ALWAYS,0x1,0xFF);
		// glStencilFunc(GL_EQUAL,0x1,0xFF);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glLineWidth(2);
		glDisable(GL_LINE_SMOOTH);
		for(auto &obj : objects){
			auto &mesh = *(obj->mesh);

			glm::mat4 transform = camera.ProjectionMatrix*camera.ViewMatrix*glm::translate(obj->position.xyz()) * glm::mat4_cast(obj->quat);

			glUniform(shader, colorHex(0x9CFF4CFF), uColor);
			glUniform(shader, transform, uModel);
			glDrawRangeElements(GL_TRIANGLES, mesh.begin, mesh.end, mesh.count, GL_UNSIGNED_INT, mesh.offset);
		}
		if(Helper::getMarkedModule()>=0){
			glStencilFunc(GL_NOTEQUAL,0x2,0x2);
			auto &mesh = *(scene.robot->chain[Helper::getMarkedModule()]->entity->mesh);

			glm::mat4 transform = camera.ProjectionMatrix*camera.ViewMatrix*glm::translate(scene.robot->chain[Helper::getMarkedModule()]->entity->position.xyz()) * glm::mat4_cast(scene.robot->chain[Helper::getMarkedModule()]->entity->quat);

			glUniform(shader, colorHex(0xFFDE4CFF), uColor);
			glUniform(shader, transform, uModel);
			glDrawRangeElements(GL_TRIANGLES, mesh.begin, mesh.end, mesh.count, GL_UNSIGNED_INT, mesh.offset);
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	}
	glStencilMask(0xff);
	glDisable(GL_STENCIL_TEST);
	glBindVertexArray(0);
}

void copyDepth(Scene &scene){

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, depthBuffer2.ID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
	glDrawBuffers(1,&DrawBuffers[0]);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	setupBuffer(screenQuad);

	auto shader = shaders["CopyDepth"];
	glUseProgram(shader);
	setupBuffer(screenQuad);
	depthBuffer.bind(0);
	// glBindTexture(GL_TEXTURE_2D, shadowMapBuffer);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// ui.rect(3,350,150,20).text("error: "+std::to_string(glGetError()))();
}
void HDR(Scene &scene){
	HDR(full_RGBA16F.ID, colorBuffer.ID, normalBuffer.ID, depthBuffer2.ID, scene);
}

void renderLights(Scene &scene){
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, full_RGBA16F.ID, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.ID, 0);

	glDrawBuffers(0,&DrawBuffers[0]);
	glClear(GL_STENCIL_BUFFER_BIT);
	{ // first pass

		glFrontFace(GL_CW);
		glDepthFunc(GL_GEQUAL);

		glDepthMask(GL_FALSE);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP,GL_KEEP,GL_REPLACE);
		auto shader = shaders["LightPointPrePass"];
		glUseProgram(shader);

		static auto uPV = glGetUniformLocation(shader,"uPV");
		static auto uView = glGetUniformLocation(shader,"uView");
		static auto uLightPos = glGetUniformLocation(shader,"uLightPos");
		static auto uSize = glGetUniformLocation(shader,"uSize");

		glUniform(shader, camera.ProjectionMatrix, uPV);
		glUniform(shader, camera.ViewMatrix, uView);

		glBindVertexArray(globalResources->VAO);
		Mesh &mesh = globalResources->meshes["sphere"];
		int id = 0x1;
		for(u32 i=g_lightsToForward; i<scene.pointLamps.size(); i++){
			auto &lamp = scene.pointLamps[i];
			glStencilMask(id); // wszystko co zapisywane do stencila, jest ANDowane przez maskê
			glStencilFunc(GL_ALWAYS,id,id); // punkt jest zawsze rysowany
			glUniform(shader, lamp.position, uLightPos);
			glUniform(shader, lamp.falloffDistance, uSize);
			glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, mesh.offset);
			id = id<<1;
		}
	}
	{ // point lights
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, full_RGBA16F.ID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthBuffer.ID, 0);
		glDrawBuffers(1,&DrawBuffers[0]);
		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);

		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_ONE, GL_ONE);
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);
		glFrontFace(GL_CCW);
		glStencilMask(0xFF);
		glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);

		auto shader = shaders["LightPoint"];
		glUseProgram(shader);

		static auto uPV = glGetUniformLocation(shader,"uPV");
		static auto uView = glGetUniformLocation(shader,"uView");
		static auto uInvPV = glGetUniformLocation(shader,"uInvPV");
		static auto uLightPos = glGetUniformLocation(shader,"uLightPos");
		static auto uSize = glGetUniformLocation(shader,"uSize");
		static auto uEyePos = glGetUniformLocation(shader,"uEyePos");
		static auto uColor = glGetUniformLocation(shader,"uColor");
		static auto uEnergy = glGetUniformLocation(shader,"uEnergy");
		static auto uNormalBuffer = glGetUniformLocation(shader,"uNormalBuffer");
		static auto uDepthBuffer = glGetUniformLocation(shader,"uDepthBuffer");

		normalBuffer.bind(0);
		depthBuffer2.bind(1);


		glUniform(shader, glm::inverse(camera.ProjectionMatrix*camera.ViewMatrix), uInvPV);
		glUniform(shader, camera.ProjectionMatrix, uPV);
		glUniform(shader, camera.ViewMatrix, uView);

		glBindVertexArray(globalResources->VAO);
		Mesh &mesh = globalResources->meshes["sphere"];
		int id = 0x1;
		for(u32 i=g_lightsToForward; i<scene.pointLamps.size(); i++){
			auto &lamp = scene.pointLamps[i];

			glStencilFunc(GL_EQUAL, id, id);
			id = id<<1;

			if(glm::distance(camera.eyePosition, lamp.position) < lamp.falloffDistance){
				glDisable(GL_DEPTH_TEST);
				glFrontFace(GL_CW);
			}

			glUniform(shader, lamp.position, uLightPos);
			glUniform(shader, camera.eyePosition, uEyePos);
			glUniform(shader, lamp.color, uColor);
			glUniform(shader, lamp.falloffDistance, uSize);
			glUniform(shader, lamp.energy, uEnergy);
				glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, mesh.offset);

			if(glm::distance(camera.eyePosition, lamp.position) < lamp.falloffDistance){
				glEnable(GL_DEPTH_TEST);
				glFrontFace(GL_CCW);
			}
		}
	}
	glDisable(GL_STENCIL_TEST);
	glBindVertexArray(0);
}
void applyLights(Scene &scene){

}

/// full->blur->blend->full
void blurWithBlendToColor(Texture &source, Texture &target){ /// zakładamy że full_fbo jest już ustawione
	// Texture buffer = source.ID==full_R8.ID ? full_R8_2 : full_R8;
	// Texture buffer = full_R8_2;
	Texture buffer = full_RGBA8_2;
	blurVertical(source, buffer, 1.f, 1.f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);

	blurHorizontal(buffer, target, 1.f, 1.f);
}
/// full->downsample->blur->blend->full
void blurWithDownsampleThenBlendToColor(Texture &source, Texture &target){
	source.genMipmaps();
	glBindFramebuffer(GL_FRAMEBUFFER, halfFBO);

	blurVertical(half_R8, half_R8_2, 1.f, 1.f);
	blurHorizontal(half_R8_2, half_R8, 1.f, 1.f);

	glBindFramebuffer(GL_FRAMEBUFFER, fullFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.ID, 0);

	blend(half_R8);
}
/// half->blur->blend->full
void blurDownsampledWithBlendToColor(Texture &source, Texture &target){ /// zakładamy że half_fbo jest już ustawione
	Texture buffer = half_RGBA8_2;
	blurVertical(source, buffer, 2.f, 1.f);
	blurHorizontal(buffer, source, 2.f, 1.f);

	glBindFramebuffer(GL_FRAMEBUFFER, fullFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.ID, 0);
		glViewport(0, 0, screenSize.x, screenSize.y);

	blend(target);
}

/**
    Only depthBuffer: bez blura
    With normalmap:
        (optional) render to half size
        blur
        blend to color
        drugi stage blura można by połączyć z blurowaniem do koloru
    -----
    Cztery opcje
    - ssao na samej głębokości
    - ssao do halfSize i blur
    - ssao do fullSize, downsample i blur
    - ssao do fullSize i blur
*/
void SSAO(){
	bool depthOnlySSAO = false;
	bool SSAOWithDownsample = false;
	if(depthOnlySSAO){
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer.ID, 0);
			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO, GL_SRC_COLOR);

			auto shader = shaders["SSAO_depthOnly"];
			glUseProgram(shader);

			auto uDepthBuffer = glGetUniformLocation(shader,"uDepthBuffer");
			auto u_SSAORandom = glGetUniformLocation(shader,"uSSAORandom");
			auto u_View = glGetUniformLocation(shader,"uView");
			auto uNear = glGetUniformLocation(shader,"uNear");
			auto uFar = glGetUniformLocation(shader,"uFar");

			glUniform(shader, screenSize.x, "uWidth");
			glUniform(shader, screenSize.y, "uHeight");

			depthBuffer2.bind(GL_TEXTURE0, uDepthBuffer);
			glActiveTexture(GL_TEXTURE1);
			glUniform1i(u_SSAORandom, 1);
			glBindTexture(GL_TEXTURE_2D, globalResources->textures["SSAORandom"]);

			glUniform(shader, camera.m_near, uNear);
			glUniform(shader, camera.m_far, uFar);

			setupBuffer(screenQuad);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	else if(SSAOWithDownsample){
		glBindFramebuffer(GL_FRAMEBUFFER, halfFBO);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, half_RGBA8.ID, 0);
		glViewport(0, 0, screenSize.x/2, screenSize.y/2);

		glDisable(GL_BLEND);

		auto shader = shaders["SSAO"];
		glUseProgram(shader);

		auto uNormalBuffer = glGetUniformLocation(shader,"uNormalBuffer");
		auto uDepthBuffer = glGetUniformLocation(shader,"uDepthBuffer");
		auto uInvPV = glGetUniformLocation(shader,"uInvPV");

		normalBuffer.bind(GL_TEXTURE0, uNormalBuffer);
		depthBuffer2.bind(GL_TEXTURE1, uDepthBuffer);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, globalResources->textures["SSAORandom"]);

		glUniform(shader, glm::inverse(camera.ProjectionMatrix*camera.ViewMatrix), uInvPV);

		setupBuffer(screenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		blurDownsampledWithBlendToColor(half_RGBA8, colorBuffer);
	}
	else {
		// glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, full_RGBA8.ID, 0);
		// glDisable(GL_BLEND);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer.ID, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_SRC_COLOR);

		auto shader = shaders["SSAO"];
		glUseProgram(shader);

		auto uNormalBuffer = glGetUniformLocation(shader,"uNormalBuffer");
		auto uDepthBuffer = glGetUniformLocation(shader,"uDepthBuffer");
		auto u_SSAORandom = glGetUniformLocation(shader,"uSSAORandom");
		auto uInvPV = glGetUniformLocation(shader,"uInvPV");

		normalBuffer.bind(GL_TEXTURE0, uNormalBuffer);
		depthBuffer2.bind(GL_TEXTURE1, uDepthBuffer);
		glActiveTexture(GL_TEXTURE2);
			glUniform1i(u_SSAORandom, 2);
		glBindTexture(GL_TEXTURE_2D, globalResources->textures["SSAORandom"]);

		glUniform(shader, camera.invPV, uInvPV);

		setupBuffer(screenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// blurWithBlendToColor(full_RGBA8, colorBuffer);
	}
}
void Sobel(){

	glBindFramebuffer(GL_FRAMEBUFFER, fullFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorBuffer.ID, 0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	// glDisable(GL_BLEND);

	auto shader = shaders["Sobel"];
	glUseProgram(shader);

	static auto uNormalBuffer = glGetUniformLocation(shader,"uNormalBuffer");
	static auto uDepthBuffer = glGetUniformLocation(shader,"uDepthBuffer");
	static auto uInvPV = glGetUniformLocation(shader,"uInvPV");
	static auto u_View = glGetUniformLocation(shader,"uView");
	static auto uNear = glGetUniformLocation(shader,"uNear");
	static auto uFar = glGetUniformLocation(shader,"uFar");

	// glActiveTexture(GL_TEXTURE0);
		// glUniform1i(uNormalBuffer, 0);
	// glBindTexture(GL_TEXTURE_2D, normalBuffer.ID);

	// glActiveTexture(GL_TEXTURE1);
		// glUniform1i(uDepthBuffer, 1);
	// glBindTexture(GL_TEXTURE_2D, depthBuffer2.ID);
	normalBuffer.bind(GL_TEXTURE0, uNormalBuffer);
	depthBuffer2.bind(GL_TEXTURE1, uDepthBuffer);

	glUniform(shader, glm::inverse(camera.ProjectionMatrix*camera.ViewMatrix), uInvPV);
	glUniform(shader, glm::inverse(camera.ViewMatrix), u_View);
	glUniform(shader, camera.m_near, uNear);
	glUniform(shader, camera.m_far, uFar);

	setupBuffer(screenQuad);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void finalize(Scene &scene){

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	setupBuffer(screenQuad);

	auto shader = shaders["ApplyFBO"];
	glUseProgram(shader);

	setupBuffer(screenQuad);
	if(globalSettings & DRAW_COLORS)
		colorBuffer.bind();
	if(globalSettings & DRAW_NORMALS)
		normalBuffer.bind();
	if(globalSettings & DRAW_DEPTH)
		// depthBuffer.bind();
		objectIDTex_R16.bind();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(0);
	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void postprocess(Scene &scene){
}

void initGrids(){
	int steps = 20;
	float spacing = 1.f;
	vector<glm::vec4>points;

	// auto add = [&steps, spacing, &points](glm::vec4 startA, glm::vec4 startB, glm::vec4 increment){
	auto add = [&steps, spacing, &points](glm::vec4 startA, glm::vec4 startB, glm::vec4 increment, float len){
		// float steps = len/spacing;
		for(float i=0; i<len+spacing; i+=spacing){
			points.push_back(startA + i*increment);
			points.push_back(startB + i*increment);
		}
	};

	//XY, XZ, YZ
	add({-10,-10,0,1}, {10,-10,0,1}, {0,1,0,0}, 20);
	add({-10,10,0,1}, {-10,-10,0,1}, {1,0,0,0}, 20);
	add({-10,0,-1,1}, {10,0,-1,1}, {0,0,1,0}, 11);
	add({-10,0,-1,1}, {-10,0,10,1}, {1,0,0,0}, 20);
	add({0,-10,-1,1}, {0,10,-1,1}, {0,0,1,0}, 11);
	add({0,-10,-1,1}, {0,-10,10,1}, {0,1,0,0}, 20);

	genBuffer4(b_grid, points);
	g_pointsSize = points.size();
}
void drawGrids(){
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto shader = shaders["Lines"];
	glUseProgram(shader);

	glUniform(shader, camera.ProjectionMatrix*camera.ViewMatrix, "uPV");
	// glUniform(shader, colorHex(0x73A2DE70), "color");
	glUniform(shader, colorHex(0xE6860090), "uColor");
	glLineWidth(1);
	glEnable(GL_LINE_SMOOTH);

	int XYlineCount = 84;
	int verticalLineCount = 40+26;

	setupBuffer(b_grid, 0,4,0);
	if(globalSettings & DRAW_XY_GRID)
		glDrawArrays(GL_LINES, 0, XYlineCount);
	if(globalSettings & DRAW_XZ_GRID)
		glDrawArrays(GL_LINES, XYlineCount, verticalLineCount);
	if(globalSettings & DRAW_YZ_GRID)
		glDrawArrays(GL_LINES, XYlineCount + verticalLineCount, verticalLineCount);


	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void drawASDFASDF(glm::vec4 point){
	glm::vec4 xy_point = point * glm::vec4(1,1,0,1);
	glm::vec4 xz_point = point * glm::vec4(1,0,1,1);
	glm::vec4 yz_point = point * glm::vec4(0,1,1,1);

	vector<glm::vec4> lines;

	if(globalSettings & DRAW_XY_GRID){
		lines.push_back(xy_point);
		lines.push_back(point);
	}
	if(globalSettings & DRAW_XZ_GRID){
		lines.push_back(xz_point);
		lines.push_back(point);
	}
	if(globalSettings & DRAW_YZ_GRID){
		lines.push_back(yz_point);
		lines.push_back(point);
	}
	b_universalVec4.update(lines);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto shader = shaders["Lines"];
	glUseProgram(shader);

	glUniform(shader, camera.ProjectionMatrix*camera.ViewMatrix, "uPV");
	glUniform(shader, colorHex(0xff3000a0), "uColor");
	glLineWidth(2);
	glEnable(GL_LINE_SMOOTH);

	b_universalVec4.bind();
	glDrawArrays(GL_LINES, 0,  lines.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void drawLineStrip(std::vector<glm::vec4> &points, HexColor color, int size){
	b_universalVec4.update(points);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto shader = shaders["Lines"];
	glUseProgram(shader);

	glUniform(shader, camera.ProjectionMatrix*camera.ViewMatrix, "uPV");
	glUniform(shader, colorHex(color), "uColor");
	glLineWidth(size);
	glEnable(GL_LINE_SMOOTH);

	b_universalVec4.bind();
	glDrawArrays(GL_LINE_STRIP, 0,  points.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void drawLines(std::vector<glm::vec4> &points, HexColor color, int size){
	b_universalVec4.update(points);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto shader = shaders["Lines"];
	glUseProgram(shader);

	glUniform(shader, camera.ProjectionMatrix*camera.ViewMatrix, "uPV");
	glUniform(shader, colorHex(color), "uColor");
	glLineWidth(size);
	glEnable(GL_LINE_SMOOTH);

	b_universalVec4.bind();
	glDrawArrays(GL_LINES, 0,  points.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
void drawPoints(std::vector<glm::vec4> points, HexColor color, float size){
	b_universalVec4.update(points);

	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	// glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable( GL_POINT_SPRITE );
	glEnable(GL_PROGRAM_POINT_SIZE);
	auto shader = shaders["PointList"];
	glUseProgram(shader);

	glUniform(shader, camera.ProjectionMatrix, "uProjection");
	glUniform(shader, camera.ViewMatrix, "uView");
	glUniform(shader, size, "uSize");
	glUniform(shader, colorHex(color), "uColor");

	b_universalVec4.bind();
	glDrawArrays(GL_POINTS, 0,  points.size());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_PROGRAM_POINT_SIZE);
	glDisable( GL_POINT_SPRITE );

}

void drawLineStrip(LineStrip &strip, bool clear){
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(strip.lineSize);
    glEnable(GL_LINE_SMOOTH);

    auto shader = shaders["Lines"];
    glUseProgram(shader);

    glUniform(shader, camera.ProjectionMatrix, "uPV");
    glUniform(shader, camera.ViewMatrix, "uView");
		glUniform(shader, colorHex(strip.color), "uColor");

    b_vec4_1024.update(strip.points);
		b_vec4_1024.bind(0, 4);

    glDrawArrays(GL_LINE_STRIP, 0,  strip.count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if(clear)
       strip.clear();
}
void drawLineSegments(LineSegments &segments, bool clear){
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glLineWidth(segments.lineSize);
    glEnable(GL_LINE_SMOOTH);

    auto shader = shaders["lineSegments"];
    glUseProgram(shader);

    glUniform(shader, camera.ProjectionMatrix, "projection");
    glUniform(shader, camera.ViewMatrix, "view");

    b_vec4_1024.update(segments.points);
        b_vec4_1024.bind(0, 4);
    b_color_1024.update(segments.colors);
        b_float_1024.bind(1, 4, 0, GL_UNSIGNED_BYTE, GL_TRUE);

    glDrawArrays(GL_LINES, 0,  segments.count);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if(clear)
       segments.clear();
}
void drawAABB(const glm::vec4 &center, const glm::vec4 &dimensions, HexColor color, int lineSize){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	// auto &mesh = meshes["box1x1m"];
	auto shader = shaders["simpleTransformAndColor"];
	glm::mat4 modelMatrix = identity;
	// dimensions.z = 0.1f;
	modelMatrix *= glm::translate(center.xyz());
	// modelMatrix *= glm::scale(dimensions.xyz()*0.5f*glm::vec3(1,1,0.00001));
	modelMatrix *= glm::scale(dimensions.xyz()*0.5f);

	glLineWidth(lineSize);
	glEnable(GL_LINE_SMOOTH);

	glUseProgram(shader);
	// glBindVertexArray(mesh.VAO);
	glUniform(shader, camera.ProjectionMatrix, "projection");
	glUniform(shader, camera.ViewMatrix, "view");
	glUniform(shader, modelMatrix, "model");
	glUniform(shader, colorHex(color), "color");

	// glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, mesh.offset);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void drawOABB(const btTransform &transform, const glm::vec4 &dimensions, HexColor color, int lineSize){
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4 modelMatrix = to_mat4(transform);

	glLineWidth(lineSize);
	glEnable(GL_LINE_SMOOTH);

	auto shader = shaders["simpleTransformAndColor"];
	glUseProgram(shader);
	glUniform(shader, camera.ProjectionMatrix, "projection");
	glUniform(shader, camera.ViewMatrix, "view");
	glUniform(shader, modelMatrix, "model");
	glUniform(shader, colorHex(color), "color");

	auto &mesh = globalResources->meshes["box"];
	glBindVertexArray(mesh.VAO);
	glDrawElements(GL_TRIANGLES, mesh.count, GL_UNSIGNED_INT, mesh.offset);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void drawTexturedBox(GLuint texture, const glm::vec4 &box){
	texturedBoxes.emplace_back(texture, box);
}


void drawVector(glm::vec4 position, glm::vec4 axis, float lenght){


}

void renderGUI(UI::IMGUI &gui){

	glDepthMask(0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	for(u32 layer=0; layer<3; layer++){
	{ // text
        gui.m_UIContainer->draw(gui, layer);
		GLuint shader = shaders["Text"];
		glUseProgram(shader);

		glUniformMatrix4fv(glGetUniformLocation(shader,"uPV"), 1, GL_FALSE, glm::value_ptr(orthoMatrix));

		setupBuffer(quadCorner, 0, 4, 0);
		for (auto &it : UI::fonts){
			if(it.second.generator->m_empty[layer])
				continue;

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, it.second.texID);

			auto &renderData = it.second.generator->m_renderData[layer];

			updateBuffer(b_position, renderData.positions);
			updateBuffer(b_uv, renderData.uvs);
			updateBuffer(b_size, renderData.sizes);
			updateBuffer(b_color, renderData.colors);
			glBindBuffer(GL_ARRAY_BUFFER, b_universalLong.current);

			setupBuffer(b_position, 1, 2, 1);
			setupBuffer(b_uv, 2, 4, 1);
			setupBuffer(b_size, 3, 2, 1);
			setupBuffer(b_color, 4, 4, 1, GL_UNSIGNED_BYTE, GL_TRUE);

			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, renderData.m_size);
			it.second.generator->clear(layer);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glDisableVertexAttribArray(4);

		}


	}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	glDepthMask(1);


}

void renderShapes(){
	// if(GUI)
	// drawPattern2D(glm::vec2(mouse_x, mouse_y), glm::vec2(0,0), "kursor", colorWhite);

	glDepthMask(0);
	glDisable(GL_CULL_FACE);
	{ // p[oints
		glEnable( GL_POINT_SPRITE );
		glEnable(GL_PROGRAM_POINT_SIZE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, point_VBO);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
		for(auto &it : point2D){
			auto shader = it.shader;
			glUseProgram(shader);

			glUniform(shader, orthoMatrix, "projection");
			glUniform(shader, it.info4, "info");
			glUniform(shader, it.color, "color");

			glDrawArrays(GL_POINTS, 0, 1);
		}
		point2D.clear();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDisable(GL_BLEND);
		glDisable(GL_PROGRAM_POINT_SIZE);
		glDisable( GL_POINT_SPRITE );
	}
	glEnable(GL_ALPHA_TEST);
	setupBuffer(quadCentered, 0,4,0);
	for(auto &it : shape2D){
		auto shader = it.shader;
		glUseProgram(it.shader);


		glUniform(shader, orthoMatrix, "projection");
		glUniform(shader, it.info4, "info");
		glUniform(shader, colorHex(it.color), "color");

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	}
	shape2D.clear();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisable(GL_ALPHA_TEST);

	glEnable(GL_BLEND);
	setupBuffer(quadCentered, 0,4,0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for(auto &it : filledShape2D){
		auto shader = it.shader;

		glUseProgram(it.shader);

		glUniform(shader, orthoMatrix, "projection");
		glUniform(shader, it.info4, "info");
		glUniform(shader, colorHex(it.color), "color");
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		}
	filledShape2D.clear();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*if(false){ // text
		GLuint shader = shaders["text"];
		glUseProgram(shader);

		glUniformMatrix4fv(glGetUniformLocation(shader,"projection"), 1, GL_FALSE, glm::value_ptr(orthoMatrix));
		// glUniform3fv(glGetUniformLocation(shader,"color"), 1, glm::value_ptr(GUIcolor));

		setupBuffer(quadCorner, 0, 4, 0);
		// setupBuffer(quadCentered, 0, 4, 0);
		for(auto &it : fonts){
			if(it.second.generator->m_empty)
				continue;


			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, it.second.texID);

			// it.second.generator->generateText();
			auto &renderData = it.second.generator->m_renderData;

			updateBuffer2(b_position, renderData.positions);
			updateBuffer4(b_uv, renderData.uvs);
			updateBuffer2(b_size, renderData.sizes);
			updateBuffer(b_color, renderData.colors);
			// b_universalLong.update(textInfo.total);
			glBindBuffer(GL_ARRAY_BUFFER, b_universalLong.current);

			// zrobic z tego VAO
			// glEnableVertexAttribArray(1);
			// glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
			// glVertexAttribDivisor(1, 1);

			// glEnableVertexAttribArray(2);
			// glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*2));
			// glVertexAttribDivisor(2, 1);

			// glEnableVertexAttribArray(3);
			// glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*6));
			// glVertexAttribDivisor(3, 1);

			// glEnableVertexAttribArray(4);
			// glVertexAttribPointer(4, 4, GL_UNSIGNED_BYTE , GL_FALSE, 0, (void*)(sizeof(float)*8));
			// glVertexAttribDivisor(4, 1);

			setupBuffer(b_position, 1, 2, 1);
			setupBuffer(b_uv, 2, 4, 1);
			setupBuffer(b_size, 3, 2, 1);
			setupBuffer(b_color, 4, 4, 1, GL_UNSIGNED_BYTE);

			glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, renderData.m_size);
			it.second.generator->clear();
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
			glDisableVertexAttribArray(3);
			glDisableVertexAttribArray(4);

		}



	}*/
	if(true){ /// patterns
		GLuint shader = shaders["AlphaPattern2D"];
		glUseProgram(shader);

		glUniform(shader, orthoMatrix, "uPV");

		setupBuffer(quadCentered, 0, 4, 0);
		for(auto &it : pattern2D){
			glUniform(shader, colorHex(it.color), "uColor");
			glUniform(shader, it.info4, "uPattern");
			glUniform(shader, it.info2, "uInfo");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, it.tex);
			glUniform1i(glGetUniformLocation(shader,"uTexture"),0);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		pattern2D.clear();
	}
	glDepthMask(0);
	if(true){ /// graphs
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		auto shader = shaders["Image2D"];
		glUseProgram(shader);
		glUniform(shader, orthoMatrix, "uPV");
		setupBuffer(quadCorner, 0, 4, 0);

		for(auto &it : texturedBoxes){
			glUniform(shader, it.second, "uBox");
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, it.first);

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		}
		texturedBoxes.clear();
	}

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	glDepthMask(1);
}

bool initGlew(){
	if(glewInit() != GLEW_OK)
		return true;

	// std::cout << "Status: Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	// std::cout << "OpenGL version " << glGetString(GL_VERSION) << " supported" << endl;
	return false;
}

NAM_END

#include "Engine-Externals.h"
