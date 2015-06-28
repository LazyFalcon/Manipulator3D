
#include <Utils/Includes.h>
#include "common.h"

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

#include <Utils/Utils.h>
#include <Utils/glUtils.h>
#include <Utils/IMGUI_V4.h>
#include <Utils/UIContainer.cpp>
#include <Utils/Camera.h>
#include "CFGParser.h"
#include "Timer.h"
#define _DebugLine_  std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";

using namespace std::chrono;
glm::vec4 		viewCenter =	glm::vec4(0.f,0.f,0.f,0.f);
glm::vec3 		Z =			glm::vec3(0.f,0.f,1.f);
glm::vec3 		X =			glm::vec3(1.f,0.f,0.f);
glm::vec3 		Y =			glm::vec3(0.f,1.f,0.f);
glm::vec4 		Zz =		glm::vec4(0.f,0.f,1.f,0.f);
glm::vec4 		Xx =		glm::vec4(1.f,0.f,0.f,0.f);
glm::vec4 		Yy =		glm::vec4(0.f,1.f,0.f,0.f);
glm::vec4 		CameraUp;
glm::vec4 		CameraRight;
glm::vec4 		CameraNormal;
const float 	PI = 3.141592f;
const float 	pi = 3.141592f;
const double  dpi = 3.141592653589793;
const float 	toRad = pi/180;
const float 	toDeg = 180/pi;
bool 					GUI = true;
glm::mat4 		identity = glm::mat4(1);
bool 					rClick = false;
float					g_scrollPos = 0;
float					g_scrollDel = 0;
float					scrollMov = 0;
float					mouse_x, mouse_y;
// glm::vec4 		mousePosition(0);
glm::vec2 		mousePosition(700, 350);
glm::vec2 		mouseTranslation(0);
glm::vec2     mouseMoveVector(0);
float         mouseMoveLen(0);
bool					LeftMousePressed = false;
bool					RightMousePressed = false;
bool					signal10ms = false;
glm::mat4 		orthoMatrix;
GLFWwindow 		*window;
float 				window_width = 1400, window_height = 700;
glm::vec2     screenSize;
bool 					quit = false;
bool 					UI::GetInput = false;
int64_t 			globalSettings;
float 				g_timeStep = 10.f;

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
#include "Mech.h"
int robotPositionsCounter = 0;
int robotPositionsMax = 100;
vector<glm::vec4> robotPositions(robotPositionsMax);
#include "ResourceLoader.h"
Resources *globalResources;
#include "Scene.h"
Scene scene;
#include "Widgets.h"
#include "Engine.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "RobotController.h"
RobotController g_RC;
#include "PathCreator.h"
#include "SomeTests.h"
#include "BigSplineTest.h"
#include "Menu.h"
#include "Menu-SideBar.h"
unique_ptr<ManuSideBar> manuSideBar;
CFG::Node cfg_settings;

void loadResources();
void mainLoop();
void shutDown(int returnCode);

void BADBADBADRobotIKRealtimeTest(Robot &robot);

void scrollCallback(GLFWwindow* window, double xOff, double yOff);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void reloadWhatIsPossible();
void initContext(CFG::Node &cfg);

void Shut_Down(int return_code){
	glfwTerminate();
	exit(return_code);
}

// resize callback

glm::vec3 camPosition(0,0,0);

int main(){
	logger::init();
	// globalSettings |= LIGHTS | DRAW_COLORS | DRAW_XY_GRID;
	// globalSettings |= DRAW_XY_GRID;
	globalSettings |= LIGHTS;
	globalSettings |= DRAW_COLORS;
	cfg_settings = CFG::Load("../settings.yml");
	initContext(cfg_settings["Window"]);
	{ // load res
		globalResources = new Resources();
		auto &&resources = CFG::Load("../resources.yml");
		ResourceLoader loader(globalResources);
		loader.loadResources(resources["Main"]);
		loader.loadFonts(resources["Fonts"]);
	}
	if(true){ // load def scene
		scene.resources = new Resources();
		ResourceLoader loader(scene.resources);
		auto &&resources = CFG::Load("../models/stanowisko.yml");
		loader.loadScene(scene, resources);
		cout<<"__"<<scene.units.size()<<endl;
	}
	{ // setup callbacks
		glfwSetScrollCallback(window, scrollCallback);
		glfwSetKeyCallback(window, keyCallback);
		glfwSetMouseButtonCallback(window, mouseButtonCallback);
	}
	camera.init();
	// camera.camOffset = glm::vec3(0,0,-3);
	camera.camOffset = glm::vec3(0,0,-0.5);
	// camera.cameraType = CAMERA_QUATERNION_BASED;
	BigSplineTest::init();
	Engine::init(cfg_settings["Graphic"]);
	Engine::initGrids();
	reloadWhatIsPossible();
	ui.m_imageSet = &(globalResources->imageSets["Menu"]);
	ui.setDefaultFont("ui_12", 12);

	scene.robot.chain[0]->value = 45*toRad;
	scene.robot.chain[3]->value = 30*toRad;


	// for(auto &it : scene.robot.chain)
	// steeringConsole.buttons.push_back(PlusMinusWidget(it));

	 g_RC.robot = &(scene.robot);
	 g_RC.solver = new JT1;
	 RCTest(g_RC);
	glfwShowWindow(window);
	mainLoop();
	Engine::clear();
	// logger::close();
	delete globalResources;
	return 0;
}

void fastLoop(float step){
	scene.robot.update(step);
	g_RC.update(step);
}
void renderLoop(){
	// Engine::plotGraphs();
	Engine::generateShadowMap(scene);
	Engine::setup(scene);
	Engine::renderScene(scene);
	Engine::copyDepth(scene);
	Engine::renderLights(scene);
	Engine::applyLights(scene);
	Engine::postprocess(scene);
	Engine::drawOutline(scene);

	Engine::drawLineStrip(g_RC.getCommand()->getPath(), 0x0000b0a0);
	//Engine::drawLineStrip(g_RC.commands.front()->m_interpolator->points, 0x00b000a0);

	//if (g_RC.state != RCStates::Pause)
	//	Engine::drawLines(g_lines.points, 0x00b0b0ff);
	// g_lines.counter = 0;
	// Engine::drawPoints(g_robotPositions.positions, 0xFFE10060);
	// Engine::drawPoints(g_mousePositions.positions, 0xFF3000ff, 8);
	// if (g_RC.state != RCStates::Pause)
		// Engine::drawLineStrip((*g_RC.commandIter)->getPath(), 0xf0b000ff);

	// BigSplineTest::draw();
	Engine::drawGrids();
	// Engine::drawASDFASDF(scene.robot.endEffector.position);
	Engine::finalize(scene);
	Engine::renderGUI(ui);
	Engine::renderShapes();
	glfwSwapBuffers(window);
}
void prerequisites(){
	manuSideBar = make_unique<ManuSideBar>();

}
void updates(float dt){
	// manuSideBar->run();
	// BigSplineTest::update(dt);
	//SomeTests();
}
void mainLoop(){
	Timer<float, std::ratio<1,1000>,30> timer;
	Timer<uint32_t, std::ratio<1,1000>,100> msecTimer;
	Timer<uint32_t, std::ratio<1,1000>,1> msecCounter;
	Timer<double, std::ratio<1,1000>,60> precisetimer;
	float timeAccumulator = 0.f;
	// float step = 1.f/120.f;
	float step = g_timeStep;
	float dt = 0.f;
	uint32_t msdt = 0;
	float accu10ms = 0.f;
	glm::vec3 tmpAxis(0);
	float tmpAngle(0);
	float tmpAngle2(0);
	// alert("Welcome\nbhaf haf aj");

	// PathCreator pcr((BSpline*)(((MoveCommand*)g_RC.commands.front().get())->interpolator));

	prerequisites();

	_DebugLine_
	while(!quit){
		dt = timer();
		msdt = msecTimer();
		timeAccumulator += dt;
		accu10ms += dt;
		signal10ms = false;
		if(accu10ms > 0.01f){
			accu10ms = 0;
			signal10ms = true;
		}

		ui.begin();
		if(LeftMousePressed){
			ui.setMouseLRepeat();
		}
		if(RightMousePressed){
			ui.setMouseRRepeat();
		}
		ui.setMouse(mouse_x, mouse_y);
		ui.updateCounter(dt);
		ui.updateCounter(msdt);

		double m_x, m_y;
		glfwGetCursorPos(window, &m_x, &m_y);
		mouse_x = m_x;
		mouse_y = m_y;
		mouse_y = window_height - mouse_y;
		mouseTranslation = glm::vec2(mouse_x, mouse_y) - mousePosition;
		mouseMoveVector = glm::normalize(mouseTranslation);
		mouseMoveLen = glm::length(mouseTranslation);
		mousePosition = glm::vec2(mouse_x, mouse_y);

		while(timeAccumulator >= step && !quit){ // fixed step loop
			timeAccumulator -= step;
			fastLoop(step);
		}

		camera.setCamPosition(camPosition);
		camera.setMatrices();
		camera.updateCamera(dt);


		// if (g_RC.state != RCStates::Pause)
			// pcr.update();

		UI::GetInput = ui.textEditor.state();
		updates(dt);
		MainMenu();
		ui.table(UI::LayoutVertical | UI::AlignLeft | UI::AlignBottom );
			ui.rect().text(msecTimer.getString()+"ms").font("ui_12"s)();
			ui.rect().text("rot_z "+std::to_string(camera.rot_z)).font("ui_12"s)();
			ui.rect().text("rot_x "+std::to_string(camera.rot_x)).font("ui_12"s)();
			ui.rect().text("pos "+glm::to_string(camera.eyePosition)).font("ui_12"s)();
		ui.endTable();

		ui.end();
		renderLoop();
		g_scrollDel = 0.0;
		glfwPollEvents();
	}
}
void scrollCallback(GLFWwindow* window, double xOff, double yOff){
	g_scrollDel = yOff;
	g_scrollPos += yOff;
	camera.zoomStep(-yOff*0.4);
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
	if(key == GLFW_KEY_KP_ENTER)
			key = GLFW_KEY_ENTER;
	if(mods == GLFW_MOD_ALT && key == GLFW_KEY_F4)
		quit = true;

	if(key == 'R' && (mods & GLFW_MOD_CONTROL) && action == GLFW_PRESS){
		ResourceLoader loader(scene.resources);
		loader.reloadShader("EnviroShade", "EnviroShade", "EnviroShade");
		loader.reloadShader("EnviroDefColorOnly", "EnviroDef", "EnviroDefColorOnly");
	}

	ui.keyInput(key, action, mods);
	if(UI::GetInput) return;
	if(key == GLFW_KEY_SPACE && action == GLFW_PRESS){
		reloadWhatIsPossible();
	}
	if(key == 'P' && action == GLFW_PRESS){
		g_RC.run();
	}

	ui.keyInput(key, action, mods);
	Editor::polylineEditor.processKeys(key, action, mods);

	float targetStep = 0.1;
	float targetStepPerSec = targetStep/1;

	if(key == GLFW_KEY_UP && action == GLFW_PRESS)
		robotTarget += glm::vec4(targetStep,0,0,0);
	if(key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		robotTarget += glm::vec4(-targetStep,0,0,0);

	if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		robotTarget += glm::vec4(0,targetStep,0,0);
	if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		robotTarget += glm::vec4(0,-targetStep,0,0);

	if(key == GLFW_KEY_HOME && action == GLFW_PRESS)
		robotTarget += glm::vec4(0,0,targetStep,0);
	if(key == GLFW_KEY_END && action == GLFW_PRESS)
		robotTarget += glm::vec4(0,0,-targetStep,0);

	if(key == GLFW_KEY_UP && action == GLFW_REPEAT)
		robotTarget += glm::vec4(targetStepPerSec,0,0,0);
	if(key == GLFW_KEY_DOWN && action == GLFW_REPEAT)
		robotTarget += glm::vec4(-targetStepPerSec,0,0,0);

	if(key == GLFW_KEY_LEFT && action == GLFW_REPEAT)
		robotTarget += glm::vec4(0,targetStepPerSec,0,0);
	if(key == GLFW_KEY_RIGHT && action == GLFW_REPEAT)
		robotTarget += glm::vec4(0,-targetStepPerSec,0,0);

	if(key == GLFW_KEY_HOME && action == GLFW_REPEAT)
		robotTarget += glm::vec4(0,0,targetStepPerSec,0);
	if(key == GLFW_KEY_END && action == GLFW_REPEAT)
		robotTarget += glm::vec4(0,0,-targetStepPerSec,0);
	if(key == GLFW_KEY_KP_1 && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
		camera.setOrientation(pi/2, pi);
	else if(key == GLFW_KEY_KP_1 && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL)
		camera.setOrientation(pi/2, 0);

	else if(key == GLFW_KEY_KP_3 && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
		camera.setOrientation(pi/2, pi+pi/2);
	else if(key == GLFW_KEY_KP_3 && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL)
		camera.setOrientation(pi/2, pi/2);

	else if(key == GLFW_KEY_KP_7 && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
		camera.setOrientation(pi, 0);
	else if(key == GLFW_KEY_KP_7 && action == GLFW_PRESS && mods != GLFW_MOD_CONTROL)
		camera.setOrientation(0, 0);
	else if(key == GLFW_KEY_KP_5 && action == GLFW_PRESS){
		if(camera.cameraProjection == PERSPECTIVE_PROJECTION)
				camera.cameraProjection = ORTHO_PROJECTION;
			else
				camera.cameraProjection = PERSPECTIVE_PROJECTION;
		camera.setProjection();
	}
}
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
	ui.mouseKeyInput(button, action);
	Editor::polylineEditor.processMouse(button, action, mods);
	if(button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_PRESS){
		glfwPollEvents();
		ui.setlClick(true);
		LeftMousePressed = true;
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT and action == GLFW_RELEASE){
		LeftMousePressed = false;
	}
	if(button == GLFW_MOUSE_BUTTON_RIGHT and action == GLFW_PRESS){
		rClick = !rClick;
		ui.setrClick(true);
		RightMousePressed = true;
	}
	if(button == GLFW_MOUSE_BUTTON_RIGHT and action == GLFW_RELEASE){
		RightMousePressed = false;
	}
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
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
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

		window = glfwCreateWindow(window_width, window_height, "DezerteR's game", NULL, NULL);
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
	camera.m_angle = cfg["angle"].asFloat();
	camera.m_width = window_width;
	camera.m_height = window_height;
	// camera.m_far = 200.f;
	camera.m_far = 50.f;
	camera.m_near = 0.1f;
	camera.setProjection();

	orthoMatrix = glm::ortho(0.f, window_width*1.f, 0.f, window_height*1.f);
	viewCenter = glm::vec4(window_width/2.f, window_height/2.f,0,0);

	// glfwSetKeyCallback(window, keyCallback);
	// glfwSetMouseButtonCallback(window, mouseButtonCallback);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);


	if(Engine::initGlew())
		Shut_Down(1);

	ui.m_maxHorizontal = window_width;
	ui.m_maxVertical = window_height;
	ui.accu = 0.f;
	ui.frequency= 5.f;
	ui.m_UIContainer = new UIContainer();
}

