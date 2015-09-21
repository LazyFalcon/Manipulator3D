#include "Utils/Includes.h"
#include "common.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include "Utils/Utils.h"
#include "Utils/glUtils.h"
#include "Utils/IMGUI_V4.h"
#include "Utils/UIContainer.cpp"
#include "Utils/Camera.h"
#include "CFGParser.h"
#include "Timer.h"
#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";

extern u32 lastIterationCount;
extern u32 lastPathIterationCount;
extern double lastSolverError;
extern float lastPathIterationdistance;
extern glm::vec3 DEBUG_VEC3_1;
extern glm::vec3 DEBUG_VEC3_2;
extern glm::vec3 DEBUG_VEC3_3;
extern glm::vec4 DEBUG_VEC4_1;
extern glm::vec4 DEBUG_VEC4_2;
extern glm::quat DEBUG_QUAT;

using namespace std::chrono;
glm::vec4      viewCenter =	glm::vec4(0.f,0.f,0.f,0.f);
glm::vec3      Z =			glm::vec3(0.f,0.f,1.f);
glm::vec3      X =			glm::vec3(1.f,0.f,0.f);
glm::vec3      Y =			glm::vec3(0.f,1.f,0.f);
glm::vec4      Zz =		glm::vec4(0.f,0.f,1.f,0.f);
glm::vec4      Xx =		glm::vec4(1.f,0.f,0.f,0.f);
glm::vec4      Yy =		glm::vec4(0.f,1.f,0.f,0.f);
glm::vec4      CameraUp;
glm::vec4      CameraRight;
glm::vec4      CameraNormal;
const float    PI = 3.141592f;
const float    pi = 3.141592f;
const double   dpi = 3.141592653589793;
const float    toRad = pi/180;
const float    toDeg = 180/pi;
bool 					 GUI = true;
glm::mat4 		 identity = glm::mat4(1);
bool           lClick = false;
bool           rClick = false;
float          g_scrollPos = 0;
float          g_scrollDel = 0;
float          scrollMov = 0;
float          mouse_x, mouse_y;
// glm::vec4 		mousePosition(0);
glm::vec2      mousePosition(700, 350);
glm::vec2      mouseTranslation(0);
glm::vec2      mouseMoveVector(0);
glm::vec2      mouselClick(0);
glm::vec2      mouserClick(0);
float          mouseMoveLen(0);
bool           LeftMousePressed = false;
bool           RightMousePressed = false;
bool           MiddleMousePressed = false;
bool           signal10ms = false;
glm::mat4      orthoMatrix;
GLFWwindow     *window;
float          window_width = 1400, window_height = 700;
glm::vec2      screenSize;
bool           quit = false;
bool           UI::GetInput = false;
int64_t        globalSettings;
u32            RedC = 0xFF000000;
u32            GreenC = 0x00FF0000;
u32            BlueC = 0x0000FF00;
u32            AlphaC = 0x000000FF;
float          defaultAngle;
std::unordered_map<string, UI::Font>	UI::fonts;
std::list <Statement> statements;
std::default_random_engine randEngine(156);
std::normal_distribution<float> normalDistr(1.f, 0.2f);
float UI::LastTextLength;
float UI::LastTextHeight;
int UI::g_UILayer = 0;

glm::vec4 robotTarget(5,5,3,1);
glm::vec4 endPos(0,0,10,1);
glm::vec4 normal(0,0,-1,0);
glm::vec4 surfaceOfMotion(0,0,-1,10);
glm::quat gripperQuat(1,0,0,0);

UI::IMGUI ui;
Camera camera;

class Entity;
#include "Robot.h"
int robotPositionsCounter = 0;
int robotPositionsMax = 100;
vector<glm::vec4> robotPositions(robotPositionsMax);
#include "BulletWorld.h"
#include "ResourceLoader.h"
#include "Widgets.h"
#include "Engine.h"
#include "Editor/Editor.h"
#include "Editor/MoveCommandBuilder.h"
namespace Editor
{
	extern unique_ptr<PolylineEditor> polylineEditor;
}

#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
BulletWorld bulletWorld;
shared_ptr<RobotController> RC;
shared_ptr<Scene> scene;
shared_ptr<Resources> globalResources;
#include "Menu.h"
CFG::Node cfg_settings;
#include "Helper.h"
#include "PythonBindings.h"

std::string shadersToReload[] = {
	"BlurVertical",
	"BlurHorizontal",
	"GlobalLightWithoutShadowMap",
	"ToneMapping",
	"SSAO",
	"SSAO_depthOnly",
	"ApplyFBO",
	"SceneElement",
};

void mainLoop();
void reloadWhatIsPossible();
void initContext(CFG::Node &cfg);

void Shut_Down(int return_code){
	glfwTerminate();
	exit(return_code);
}

glm::vec3 camPosition(0,0,3);

int main(){
	// logger::init();
	// globalSettings |= DRAW_XY_GRID;
	// globalSettings |= SSAO;
	// globalSettings |= HDR;
	globalSettings |= HIDE_GUI;
	globalSettings |= SOBEL;
	globalSettings |= DRAW_COLORS;
	cfg_settings = CFG::Load("../settings.yml");
	initContext(cfg_settings["Window"]);

	scene = make_shared<Scene>();
	RC = make_shared<RobotController>();
	globalResources = make_shared<Resources>();


	bulletWorld.init();

	{ // load res
		auto &&resources = CFG::Load("../resources.yml");
		ResourceLoader loader(globalResources);
		loader.loadResources(resources["Main"]);
		loader.loadFonts(resources["Fonts"]);
	}

	camera.init();
	camera.camOffset = glm::vec3(0,0,-0.5);
	Engine::init(cfg_settings["Graphic"]);
	Engine::initGrids();
	reloadWhatIsPossible();
	ui.m_imageSet = &(globalResources->imageSets["Menu"]);
	ui.setDefaultFont("ui_12", 12);
	PythonBindings::init(RC, scene, cfg_settings["DefaultScript"].value);

	Helper::reloadScene("../models/"+cfg_settings["DefaultScene"].value+".yml", RC, scene, bulletWorld);
	// PythonBindings::loadMainScript("BaseScript", RC, scene);
	PythonBindings::loadMainScript(cfg_settings["DefaultScript"].value, RC, scene);
	mainLoop();

	Editor::clear();
	Editor::MoveCommandBuilderWidget_terminate();
	Engine::clear();
	globalResources.reset();
	RC.reset();
	scene.reset();
	cerr<<"One says goodbye!"<<endl;
	PythonBindings::terminate();

	return 0;
}

void fastLoop(float step){
	RC->update(RC, scene, step/1000.0f);
	scene->robot->update(step/1000.0f);
	RCUtils::update();
	bulletWorld.update(step/1000.0f);
	PythonBindings::update(RC, scene);
}
void renderLoop(){
	Engine::getDataAndStartQuery("Total");
	// Engine::plotGraphs();
	// Engine::generateShadowMap(*scene);
	Engine::setup(*scene);
	Engine::renderScene(*scene);
	Engine::sampleDataUnderMouse(mousePosition);
	// if(globalSettings & LIGHTS)Engine::renderLights(*scene);
	// if(globalSettings & LIGHTS)Engine::applyLights(*scene);
	if(globalSettings & SOBEL)Engine::Sobel();
	if(globalSettings & HDR)Engine::HDR(*scene);
	if(globalSettings & SSAO)Engine::SSAO();
	Engine::drawOutline(*scene);
	Engine::postprocess(*scene);

	if(!RC->commands.empty()){
		Engine::drawLineStrip(RC->getCommand()->getPath(), 0xFFB300F0);
		Engine::drawLineStrip(RC->getCommand()->getPolyline(), 0x73FF0080,1);
	}
	if(Editor::polylineEditor->polyline){
		Engine::drawLineStrip(Editor::polylineEditor->polyline->visualisation, 0xFF6200F0);
		Engine::drawLineStrip(Editor::polylineEditor->polyline->points, 0xFF620080,1);
	}
	// Engine::drawPoints({g_targetPosition}, 0xFF2000FF, 6);
	Engine::drawGrids();
	Engine::finalize(*scene);
	if(globalSettings & HIDE_GUI)Engine::renderGUI(ui);
	if(globalSettings & HIDE_GUI)Engine::renderShapes();
	Engine::endQuery();
	glfwSwapBuffers(window);
}
void prerequisites(){
	Engine::initQueries();
	Editor::MoveCommandBuilderWidget_inits();
	Editor::init();
	glfwShowWindow(window);
}
void updates(float dt){
	Editor::update(*RC);
	// PythonBindings::update(RC, scene);
}
void mainLoop(){
	Timer<float, 1000,30> timer;
	Timer<double, 1000,30> dtimer;
	Timer<u32, 1000,1> msecTimer;
	Timer<u32, 1000,60> frameTimeCounter;
	Timer<double, 1000,1> precisetimer;
	float timeAccumulator = 0.f;
	float dt = 0.f;
	double ddt = 0.0;
	u32 msdt = 0;
	u32 accu10ms = 0;
	u32 accu50ms = 0;
	glm::vec3 tmpAxis(0);
	float tmpAngle(0);
	float tmpAngle2(0);

	prerequisites();

	std::string ikTime = "--";
	cout<<"Here we go."<<endl;
	while(!quit){
		precisetimer();
		frameTimeCounter();
		msecTimer();
		dt = timer();
		Helper::record().FrameTime = dtimer();
		timeAccumulator += dt;
		accu10ms += msecTimer.get();
		accu50ms += msecTimer.get();
		signal10ms = false;
		globalSettings &= ~GET_PERF;

		if(accu10ms > 10){
			accu10ms = 0;
			signal10ms = true;
		}
		if(accu50ms > 50){
			accu50ms = 0;
			globalSettings |= GET_PERF;
		}

		glfwPollEvents();

		ui.begin();
		if(LeftMousePressed){
			ui.setMouseLPressed();
		}
		if(RightMousePressed){
			ui.setMouseRPressed();
		}
		ui.setMouse(mouse_x, mouse_y);
		ui.updateCounter(msecTimer.get());

		double m_x, m_y;
		glfwGetCursorPos(window, &m_x, &m_y);
		mouse_x = m_x;
		mouse_y = m_y;
		mouse_y = window_height - mouse_y;
		mouseTranslation = glm::vec2(mouse_x, mouse_y) - mousePosition;
		mouseMoveVector = glm::normalize(mouseTranslation);
		mouseMoveLen = glm::length(mouseTranslation);
		mousePosition = glm::vec2(mouse_x, mouse_y);

		// precisetimer();
		while(timeAccumulator >= scene->fixedLoopStep && !quit){ // fixed step loop
			timeAccumulator -= scene->fixedLoopStep;
			fastLoop(scene->fixedLoopStep);
		}
		// precisetimer();
		// ikTime = precisetimer.getString();
		if(ui.captureMouse) _DebugLine_

		camera.setCamPosition(camPosition);
		camera.setMatrices();
		camera.updateCamera(dt);

		UI::GetInput = ui.textEditor.state();
		updates(dt);
		lClick = false;
		rClick = false;
		Helper::showPopups();
		MainMenu();
		ui.table(UI::LayoutVertical | UI::AlignLeft | UI::AlignBottom );

			auto &&vars = RC->robot->getVariables();
			for(auto it : vars){
				ui.rect().text(std::to_string(it)).font("ui_12"s)();
			}
			DEBUG_QUAT = RC->robot->endEffector.quat;
			auto DEBUG_QUAT_2 = scene->get("Target")->quat;

			glm::mat3 m = glm::toMat3(DEBUG_QUAT_2);

			DEBUG_VEC3_1 = m * glm::vec3(0,0,1);
			DEBUG_VEC3_2 = m * glm::vec3(1,0,0);

			ui.rect().color(gradientCalc(0x00FF00FF, 0xFF0000FF, u8(frameTimeCounter.get()/25.0*255.0))).text(frameTimeCounter.getString()+"ms").font("ui_12"s)();
			ui.rect().text("rot_z "+std::to_string(camera.rot_z)).font("ui_12"s)();
			ui.rect().text("rot_x "+std::to_string(camera.rot_x)).font("ui_12"s)();
			ui.rect().text("IK time: " + ikTime).font("ui_12"s)();
			ui.rect().text("Current: " + RC->getComandName()).font("ui_12"s)();
			ui.rect().text("Iterations: " + std::to_string(lastIterationCount)).font("ui_12"s)();
			ui.rect().text("pIterations: " + std::to_string(lastPathIterationCount)).font("ui_12"s)();
			ui.rect().text("dIteration: " + std::to_string(lastPathIterationdistance)).font("ui_12"s)();
			ui.rect().text("LastError: " + std::to_string(lastSolverError)).font("ui_12"s)();
			// ui.rect().text("------- ").font("ui_12"s)();
			ui.rect().text("DEBUG_VEC3_1: " + to_string(DEBUG_VEC3_1)).font("ui_12"s)();
			ui.rect().text("DEBUG_VEC3_2: " + to_string(DEBUG_VEC3_2)).font("ui_12"s)();
			// ui.rect().text("DEBUG_VEC3_3: " + to_string(DEBUG_VEC3_3)).font("ui_12"s)();
			// ui.rect().text("DEBUG_VEC4_1: " + to_string(DEBUG_VEC4_1)).font("ui_12"s)();
			ui.rect().text("DEBUG_VEC4_2: " + to_string(DEBUG_QUAT * glm::vec4(0,0,-1,0))).font("ui_12"s)();
			ui.rect().text("DEBUG_QUAT: " + to_string(DEBUG_QUAT)).font("ui_12"s)();
			ui.rect().text("DEBUG_QUAT_2: " + to_string(DEBUG_QUAT_2)).font("ui_12"s)();
		ui.endTable();
		// ui.rect(window_width*0.5-50, 2, 100, 20).text(to_string(Helper::getCursor()))();
		if(RC) Helper::directControlWidget(window_width-300, 2, mousePosition, *RC);
		// Helper::cursorVidgetHorizontal({window_width*0.5-50, 2});
		ui.image(Helper::getScreenCursor(camera), "Cursor").color(0xFFFFFFFF)();

		if(globalSettings & DRAW_PERF) Engine::drawQueries(ui);

		ui.end();
		precisetimer();
		ikTime = precisetimer.getString();
		renderLoop();

		Helper::moveCameraByMouse(camera, mousePosition, mouseTranslation, MiddleMousePressed);

		g_scrollDel = 0.0;
	}
}

void scrollCallback(GLFWwindow *window, double xOff, double yOff){
	g_scrollDel = yOff;
	g_scrollPos += yOff;
	camera.zoomStep(-yOff*0.4);
}
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods){
	if(key == GLFW_KEY_KP_ENTER)
			key = GLFW_KEY_ENTER;
	if(mods == GLFW_MOD_ALT && key == GLFW_KEY_F4)
		quit = true;

	ui.keyInput(key, action, mods);
	if(UI::GetInput) return;

	Helper::moveCameraByKeys(camera, key, action, mods);
	Helper::processKeys(key, action, mods);
	Editor::processKeys(key, action, mods, *RC);

	PythonBindings::handleInput(key, action,  mods, RC, scene);

	if(key == 'H' && action == GLFW_PRESS){
		globalSettings ^= HIDE_GUI;
	}
	if(key == 'R' && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS){
		ResourceLoader loader(scene->resources);
		for(auto &it : shadersToReload){
			loader.reloadShader(it);
		}
		reloadWhatIsPossible();
	}

	if(action == GLFW_PRESS && key == GLFW_KEY_F5){
		RC->run();
	}
	else if(action == GLFW_PRESS && key == GLFW_KEY_F8 && mods&GLFW_MOD_CONTROL){
		PythonBindings::reloadAndInitMainScript(RC, scene);
	}
	else if(action == GLFW_PRESS && key == GLFW_KEY_F8){
		PythonBindings::reloadMainScript(RC, scene);
	}
	else if(action == GLFW_PRESS && key == GLFW_KEY_F9){
		PythonBindings::executeSubScript();
	}
	else if(action == GLFW_PRESS && key == GLFW_KEY_F6){
		RC->pause();
	}
	else if(action == GLFW_PRESS && key == GLFW_KEY_F12){
		globalSettings ^= DRAW_PERF;
	}

}
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods){
	Helper::processMouse(button, action, mods);
	ui.mouseKeyInput(button, action);
	Editor::processMouse(button, action, mods);
	double m_x, m_y;
	glfwGetCursorPos(window, &m_x, &m_y);
	if(button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS){
		mouselClick = glm::vec2(m_x, m_y);
		ui.setlClick(true);
		LeftMousePressed = true;
		lClick = true;
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_RELEASE){
		LeftMousePressed = false;
	}
	if(button == GLFW_MOUSE_BUTTON_RIGHT and action == GLFW_PRESS){
		mouserClick = glm::vec2(m_x, m_y);
		rClick = !rClick;
		ui.setrClick(true);
		RightMousePressed = true;
		rClick = true;
	}
	if(button == GLFW_MOUSE_BUTTON_RIGHT and action == GLFW_RELEASE){
		RightMousePressed = false;
	}

	if(button == GLFW_MOUSE_BUTTON_MIDDLE and action == GLFW_PRESS){
		MiddleMousePressed = true;
	}
	if(button == GLFW_MOUSE_BUTTON_MIDDLE and action == GLFW_RELEASE){
		MiddleMousePressed = false;
	}
}
void dropCallback(GLFWwindow *window, int count, const char** paths){
	Helper::dropCallback(count, paths);
}
void resizeCallback(GLFWwindow *window, int width, int height){
	window_width = width;
	window_height = height;
	screenSize = glm::vec2(window_width, window_height);
	camera.m_aspect = ((float)window_width)/window_height;
	camera.m_width = window_width;
	camera.m_height = window_height;
	camera.m_angle = defaultAngle*camera.m_aspect*9.f/16.f;

	camera.setProjection();
	orthoMatrix = glm::ortho(0.f, window_width*1.f, 0.f, window_height*1.f);
	viewCenter = glm::vec4(window_width/2.f, window_height/2.f,0,0);

	ui.m_maxHorizontal = window_width;
	ui.m_maxVertical = window_height;

	Engine::clearScreenBuffers();
	Engine::resize();
}
void exitCallback(GLFWwindow *window){
	quit = true;
}

shared_ptr<RobotController> getRC(){
    return RC;
}
shared_ptr<Scene> getScene(){
    return scene;
}

void reloadWhatIsPossible(){

	auto &&styles = CFG::Load("../styles.yml");
	clearColor = styles["Graphic"]["clearColor"].asColor();
	for(int i=0; i<4; i++)
		GUIbackground[i] = styles["Graphic"]["GUIbackground"][i].asColor();
	for(int i=0; i<4; i++)
		GUIeditBox[i] = styles["Graphic"]["GUIeditBox"][i].asColor();
	for(int i=0; i<4; i++)
		GUIhover[i] = styles["Graphic"]["GUIhover"][i].asColor();
	for(int i=0; i<4; i++)
		GUIfontColor[i] = styles["Graphic"]["GUIfontColor"][i].asColor();

	auto LoadStyle = [&styles](UI::StyleID styleID, string styleName){
		auto &cfg = styles["Styles"][styleName];
		ui.m_styles[(int)styleID] = UI::Style{};
		ui.m_styles[(int)styleID].background = cfg["background"].asColor();
		ui.m_styles[(int)styleID].button     = cfg["button"].asColor();
		ui.m_styles[(int)styleID].hover      = cfg["hover"].asColor();
		ui.m_styles[(int)styleID].editBox    = cfg["editBox"].asColor();
		ui.m_styles[(int)styleID].fontColor  = cfg["fontColor"].asColor();
		ui.m_styles[(int)styleID].imageColor = cfg["imageColor"].asColor();
		// ui.m_styles[(int)styleID].label = cfg["label"].asColor();
		ui.m_styles[(int)styleID].font = cfg["font"].value;
		for(int i=0; i<4 && i<cfg["fancy"].size(); i++)
			ui.m_styles[(int)styleID].fancy[i] = cfg["fancy"][i].asColor();
	};
	ui.m_styles.clear();
	ui.m_styles.resize(5);
	LoadStyle(UI::StyleID::Basic, "Basic");
	LoadStyle(UI::StyleID::Light, "Light");
	LoadStyle(UI::StyleID::Dark, "Dark");
	LoadStyle(UI::StyleID::Label, "Label");

	ui.m_defaultColor = GUIbackground[0];
	ui.m_defaultFontColor = GUIfontColor[0];
	LightText = GUIfontColor[0];
	DarkText = GUIfontColor[1];


}
void initContext(CFG::Node &cfg){

	window_width = cfg["width"].asFloat();
	window_height = cfg["height"].asFloat();

	screenSize = glm::vec2(window_width, window_height);

	if (glfwInit() != GL_TRUE)
		Shut_Down(1);

	// glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	// glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);//albo CORE, ale wtedy nie dzia³a devil

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	if(cfg["fullscreen"].asBool()){
		window_width = mode->width;
		window_height = mode->height;
		window = glfwCreateWindow(window_width, window_height, "Manipulator3D", glfwGetPrimaryMonitor(), NULL);
	}
	else{
		window_width = min((int)window_width,mode->width);
		window_height = min((int)window_height,mode->height);

		window = glfwCreateWindow(window_width, window_height, "Manipulator3D", NULL, NULL);
		glfwSetWindowPos(window, mode->width/2-window_width/2, mode->height/2-window_height/2);
		glfwHideWindow(window);
	}
	if (!window){
		cerr<<"error: creating context"<<endl;
		glfwTerminate();
		Shut_Down(EXIT_FAILURE);
	}
	glfwSetWindowPos(window, cfg["posX"].asInt(), cfg["posY"].asInt());
	glfwSetWindowTitle(window, "Manipulator3D");
	int ww, wh;
	glfwGetFramebufferSize(window, &ww, &wh);
	// glViewport(0,0,window_width,window_height);
	glViewport(0,0,ww,wh);
	glfwMakeContextCurrent(window);

	camera.m_aspect = ((float)window_width)/window_height;
	defaultAngle = camera.m_angle = cfg["angle"].asFloat();
	camera.m_width = window_width;
	camera.m_height = window_height;
	camera.m_far = 50.f;
	camera.m_near = 0.1f;
	camera.setProjection();

	orthoMatrix = glm::ortho(0.f, window_width*1.f, 0.f, window_height*1.f);
	viewCenter = glm::vec4(window_width/2.f, window_height/2.f,0,0);

	{ // setup callbacks
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
		glfwSetDropCallback(window, dropCallback);
		glfwSetWindowCloseCallback(window, exitCallback);
		glfwSetWindowSizeCallback(window, resizeCallback);
	}

	if(Engine::initGlew())
		Shut_Down(1);

	ui.m_maxHorizontal = window_width;
	ui.m_maxVertical = window_height;
	ui.accu = 0.f;
	ui.frequency = 5;
	ui.m_UIContainer = new UIContainer();
}

