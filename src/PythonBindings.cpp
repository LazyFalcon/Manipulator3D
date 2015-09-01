#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/BaseStructs.h>
#include "IInterpolator.h"
#include "Robot.h"
#include "Robot-Commands.h"
#include "RobotController.h"
#include "CFGparser.h"
#include "ResourceLoader.h"
#include "Helper.h"
#include "Editor/MoveCommandBuilder.h"
#include "Editor/ExecuteCommandBuilder.h"
#include "Editor/WaitCommandBuilder.h"
#include "DataRecorder.h"

#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "PythonBindings.h"

#define NAM_END }
#define NAM_START {

namespace PythonBindings NAM_START

BOOST_PYTHON_MODULE(glm_export){

	bpl::class_<std::vector<glm::vec4>>("Vec4Vec")
		.def(bpl::vector_indexing_suite<std::vector<glm::vec4>>())
		;

	std::string (*to_string_vec3)(const glm::vec3&) = &to_string;
	std::string (*to_string_vec4)(const glm::vec4&) = &to_string;

	glm::mat4 (*cpp_translate)(const glm::vec3&) = &glm::translate;
	glm::mat4 (*cpp_rotate)(float, const glm::vec3&) = &glm::rotate;
	glm::vec4 (*cpp_normalize4)(const glm::vec4&) = &glm::normalize;
	glm::vec3 (*cpp_normalize3)(const glm::vec3&) = &glm::normalize;
	float (*cpp_distance4)(const glm::vec4&, const glm::vec4&) = &glm::distance;
	float (*cpp_distance3)(const glm::vec3&, const glm::vec3&) = &glm::distance;
	float (*cpp_dot4)(const glm::vec4&, const glm::vec4&) = &glm::dot;
	float (*cpp_dot3)(const glm::vec3&, const glm::vec3&) = &glm::dot;
	float (*cpp_len4)(const glm::vec4&) = &glm::length;
	float (*cpp_len3)(const glm::vec3&) = &glm::length;

	float (*cpp_pitch)(const glm::quat&) = &glm::pitch;
	glm::vec3 (*cpp_eulerAngles)(const glm::quat&) = &glm::eulerAngles;
	// glm::quat (*cpp_angleAxis)(float, const glm::vec3&) = &glm::angleAxis;

	bpl::def("translate", cpp_translate);
	bpl::def("rotate", cpp_rotate);
	bpl::def("normalize", cpp_normalize3);
	bpl::def("normalize", cpp_normalize4);
	bpl::def("distance", cpp_distance4);
	bpl::def("distance", cpp_distance3);
	bpl::def("dot", cpp_dot4);
	bpl::def("dot", cpp_dot3);
	bpl::def("length", cpp_len4);
	bpl::def("length", cpp_len3);

	bpl::def("eulerAngles", cpp_eulerAngles);
	// bpl::def("angleAxis", cpp_angleAxis);

	bpl::class_<glm::mat4>("mat4", bpl::init<float>())
		.def(bpl::init<float>())
		.def(bpl::self + glm::mat4())
		.def(bpl::self - glm::mat4())
		.def(bpl::self * glm::mat4())
		.def(bpl::self * glm::vec4())
		.def(bpl::self * float())
		;
	bpl::class_<glm::quat>("quat", bpl::init<float, float, float, float>())
		.def(bpl::init<float, glm::vec3>())
		.def_readwrite("x", &glm::quat::x)
		.def_readwrite("y", &glm::quat::y)
		.def_readwrite("z", &glm::quat::z)
		.def_readwrite("w", &glm::quat::w)
		.def(bpl::self + glm::quat())
		.def(bpl::self * glm::quat())
		.def(bpl::self * glm::vec4())
		;
	bpl::class_<glm::vec4>("vec4", bpl::init<float, float, float, float>())
		.def(bpl::init<float>())
		.def_readwrite("x", &glm::vec4::x)
		.def_readwrite("y", &glm::vec4::y)
		.def_readwrite("z", &glm::vec4::z)
		.def_readwrite("w", &glm::vec4::w)
		.def("xyz", &glm::vec4::xyz)
		.def(bpl::self + glm::vec4())
		.def(bpl::self - glm::vec4())
		.def(bpl::self * glm::vec4())
		.def(bpl::self / glm::vec4())
		.def(bpl::self / float())
		.def(bpl::self * float())
		.def("__str__", to_string_vec4)
		;
	bpl::class_<glm::vec3>("vec3", bpl::init<float, float, float>())
		.def(bpl::init<float>())
		.def_readwrite("x", &glm::vec3::x)
		.def_readwrite("y", &glm::vec3::y)
		.def_readwrite("z", &glm::vec3::z)
		.def(bpl::self + glm::vec3())
		.def(bpl::self - glm::vec3())
		.def(bpl::self * glm::vec3())
		.def(bpl::self / glm::vec3())
		.def(bpl::self * float())
		.def("__str__", to_string_vec3)
		;
}
/*
class IInterpolatorWrap : public IInterpolator, public bpl::wrapper<IInterpolator>
{
public:
	glm::vec4 firstPoint(){return glm::vec4(0);}
	glm::vec4 nextPoint(){return glm::vec4(0);}
	void generatePath(){}
	void reset(){}
	void drawParams(){}
}; */

BOOST_PYTHON_MODULE(commandBuilders_export){
	MoveCommandBuilder& (MoveCommandBuilder::*interpolator_ptr)(IInterpolatorContainer&) = &MoveCommandBuilder::interpolator;
	MoveCommandBuilder& (MoveCommandBuilder::*solver_string)(const std::string&) = &MoveCommandBuilder::solver;
	MoveCommandBuilder& (MoveCommandBuilder::*finish_ptr)(shared_ptr<RobotController>) = &MoveCommandBuilder::finish;

	// bpl::class_<std::vector<double>>("doubleVec")
		// .def(bpl::vector_indexing_suite<std::vector<double>>())
		// ;

	bpl::class_<MoveCommandBuilder, std::shared_ptr<MoveCommandBuilder>>("MoveCommandBuilder", bpl::init<>())
		.def_readwrite("moveCommand", &MoveCommandBuilder::moveCommand)
		.def("init", &MoveCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &MoveCommandBuilder::name, bpl::return_internal_reference<>())
		.def("velocity", &MoveCommandBuilder::velocity, bpl::return_internal_reference<>())
		.def("startO", &MoveCommandBuilder::startO, bpl::return_internal_reference<>())
		.def("endO", &MoveCommandBuilder::endO, bpl::return_internal_reference<>())
		.def("jointVelocity", &MoveCommandBuilder::jointVelocity, bpl::return_internal_reference<>())
		.def("acceleration", &MoveCommandBuilder::acceleration, bpl::return_internal_reference<>())
		.def("time", &MoveCommandBuilder::time, bpl::return_internal_reference<>())
		.def("interpolator", interpolator_ptr, bpl::return_internal_reference<>())
		.def("solver", solver_string, bpl::return_internal_reference<>())
		.def("finish", finish_ptr, bpl::return_internal_reference<>())
		;

	SingleJointMoveCommandBuilder& (SingleJointMoveCommandBuilder::*singlefinish_ptr)(shared_ptr<RobotController>) = &SingleJointMoveCommandBuilder::finish;
	bpl::class_<SingleJointMoveCommandBuilder, std::shared_ptr<SingleJointMoveCommandBuilder>>("SingleJointMoveCommandBuilder", bpl::init<>())
		.def("init", &SingleJointMoveCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &SingleJointMoveCommandBuilder::name, bpl::return_internal_reference<>())
		.def("set", &SingleJointMoveCommandBuilder::set, bpl::return_internal_reference<>())
		.def("velocity", &SingleJointMoveCommandBuilder::velocity, bpl::return_internal_reference<>())
		.def("jointVelocity", &SingleJointMoveCommandBuilder::jointVelocity, bpl::return_internal_reference<>())
		.def("acceleration", &SingleJointMoveCommandBuilder::acceleration, bpl::return_internal_reference<>())
		.def("finish", singlefinish_ptr, bpl::return_internal_reference<>())
		;

    FollowObjectBuilder& (FollowObjectBuilder::*target_1)(glm::vec4 &t) = &FollowObjectBuilder::target;
    FollowObjectBuilder& (FollowObjectBuilder::*target_2)(shared_ptr<Entity> &obj) = &FollowObjectBuilder::target;
	FollowObjectBuilder& (FollowObjectBuilder::*followfinish_ptr)(shared_ptr<RobotController>) = &FollowObjectBuilder::finish;
	bpl::class_<FollowObjectBuilder, std::shared_ptr<FollowObjectBuilder>>("FollowObjectBuilder", bpl::init<>())
		.def("init", &FollowObjectBuilder::init, bpl::return_internal_reference<>())
		.def("name", &FollowObjectBuilder::name, bpl::return_internal_reference<>())
		.def("target", target_1, bpl::return_internal_reference<>())
		.def("target", target_2, bpl::return_internal_reference<>())
		.def("velocity", &FollowObjectBuilder::velocity, bpl::return_internal_reference<>())
		.def("jointVelocity", &FollowObjectBuilder::jointVelocity, bpl::return_internal_reference<>())
		.def("acceleration", &FollowObjectBuilder::acceleration, bpl::return_internal_reference<>())
		.def("finish", followfinish_ptr, bpl::return_internal_reference<>())
		;

	WaitCommandBuilder& (WaitCommandBuilder::*waitfinish_ptr)(shared_ptr<RobotController>&) = &WaitCommandBuilder::finish;

	bpl::class_<ExecutePythonCommandBuilder, std::shared_ptr<ExecutePythonCommandBuilder>>("ExecutePythonCommandBuilder", bpl::init<>())
		.def("init", &ExecutePythonCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &ExecutePythonCommandBuilder::name, bpl::return_internal_reference<>())
		.def("callback", &ExecutePythonCommandBuilder::callback, bpl::return_internal_reference<>())
		.def("finish", &ExecutePythonCommandBuilder::finish, bpl::return_internal_reference<>())
		;
	bpl::class_<WaitCommandBuilder, std::shared_ptr<WaitCommandBuilder>>("WaitCommandBuilder", bpl::init<>())
		.def("init", &WaitCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &WaitCommandBuilder::name, bpl::return_internal_reference<>())
		.def("time", &WaitCommandBuilder::time, bpl::return_internal_reference<>())
		.def("finish", waitfinish_ptr, bpl::return_internal_reference<>())
		;

	bpl::enum_<SolverType>("SolverType")
		.value("JT0", SolverType::JT0)
		.value("JT1", SolverType::JT1)
		.value("JT2", SolverType::JT2)
		.value("CCD", SolverType::CCD)
		;
	bpl::enum_<Interpolator>("Interpolator")
		.value("Empty", Interpolator::Empty)
		.value("Open", Interpolator::Open)
		.value("Closed", Interpolator::Closed)
		.value("Linear", Interpolator::Linear)
		.value("BezierCurve", Interpolator::BezierCurve)
		.value("BSpline", Interpolator::BSpline)
		.value("NURBS", Interpolator::NURBS)
		.value("HermiteCardinal", Interpolator::HermiteCardinal)
		.value("HermiteFiniteDifference", Interpolator::HermiteFiniteDifference)
		.value("HermiteFiniteDifferenceClosed", Interpolator::HermiteFiniteDifferenceClosed)
		;
	bpl::class_<IInterpolatorContainer>("IInterpolatorContainer")
		.def_readonly("name", &IInterpolatorContainer::name)
		;

	bpl::def("addInterpolator", &addInterpolatorByContainer);
	// bpl::class_<IInterpolator, std::shared_ptr<IInterpolator>>("IInterpolator", bpl::init<const std::vector<glm::vec4>&, Interpolator, const std::string&>())
	// bpl::class_<IInterpolatorWrap, std::shared_ptr<IInterpolatorWrap>, boost::noncopyable>("IInterpolator", bpl::no_init)
			// .def("firstPoint", &IInterpolator::firstPoint)
			// .def("nextPoint", &IInterpolator::nextPoint)
			// .def("generatePath", &IInterpolator::generatePath)
			// .def("reset", &IInterpolator::reset)
			// .def_readwrite("type", &IInterpolator::type)
			// .def_readwrite("finished", &IInterpolator::finished)
			// .def_readwrite("name", &IInterpolator::name)
			// .def_readwrite("points", &IInterpolator::points)
			;

}
BOOST_PYTHON_MODULE(scene_export){
	bpl::class_<Entity, std::shared_ptr<Entity>>("Entity")
		.def_readonly("ID", &Entity::ID)
		.def_readwrite("position", &Entity::position)
		.def_readwrite("quat", &Entity::quat)
		// .def("rgBody", &Entity::rgBodyRef, bpl::return_value_policy<bpl::manage_new_object>() )
		;
	bpl::class_<Scene, std::shared_ptr<Scene>>("Scene", bpl::no_init)
		.def("get", &Scene::get, bpl::return_value_policy<bpl::reference_existing_object>())
		;
	bpl::class_<std::vector<std::shared_ptr<Entity>>>("EntityVec")
		.def(bpl::vector_indexing_suite<std::vector<std::shared_ptr<Entity>>>())
		;
}
BOOST_PYTHON_MODULE(robotController_export){
	void (RobotController::*insertCommand_ptr)(shared_ptr<ICommand> ptr) = &RobotController::insertCommand;
	bpl::class_<RobotController, std::shared_ptr<RobotController>>("RobotController")
		.def("insertCommand", insertCommand_ptr)
		.def("run", &RobotController::run)
		.def("pause", &RobotController::pause)
		.def("stop", &RobotController::stop)
		.def("next", &RobotController::next)
		.def("prev", &RobotController::prev)
		.def("savePosition", &RobotController::savePosition)
		.def("popPosition", &RobotController::popPosition)
		.def("peekPosition", &RobotController::peekPosition)
		.def("grabObject", &RobotController::grabObject)
		.def("releaseObject", &RobotController::releaseObject)
		.def("clean", &RobotController::clean)
		.def("wait", &RobotController::wait, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("move", &RobotController::move, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("pyExec", &RobotController::pyExec, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("jointMove", &RobotController::jointMove, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("follow", &RobotController::follow, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("getRobot", &RobotController::getRobot, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("getRobotJ", &RobotController::getRobotJ)
		.def_readonly("robot", &RobotController::robot)
		.def_readwrite("state ", &RobotController::state )
		.def_readwrite("commands", &RobotController::commands)
		;
	bpl::class_<std::vector<double>>("DoubleVec")
		.def(bpl::vector_indexing_suite<std::vector<double>>())
		;
	bpl::class_<Robot, std::shared_ptr<Robot>, boost::noncopyable>("Robot")
		.def("getModuleCount", &Robot::getModuleCount)
		.def("module", &Robot::module, bpl::return_value_policy<bpl::reference_existing_object>())
		.def_readonly("velocity", &Robot::endEffectorVelocity)
		.def_readonly("acceleration", &Robot::endEffectorAcceleration)
		;
	bpl::class_<Module, boost::noncopyable>("Module")
		.def_readwrite("value", &Module::value)
		.def_readwrite("targetValue", &Module::targetValue)
		.def_readwrite("maxVelocty", &Module::maxVelocty)
		.def_readwrite("maxAcceleration", &Module::maxAcceleration)
		.def_readwrite("lastVelocity", &Module::lastVelocity)
		.def_readwrite("lastAcceleration", &Module::lastAcceleration)
		.def_readwrite("axis", &Module::axis)
		;


}
BOOST_PYTHON_MODULE(helper_export){
	using namespace Helper;
	// bpl::def("getPositionUnderMouse", &getPositionUnderMouse);
	// bpl::def("getNormalUnderMouse", &getNormalUnderMouse);
	// bpl::def("getObjectUnderMouse", &getObjectUnderMouse, bpl::return_value_policy<bpl::reference_existing_object>());

	// bpl::def("savePoint", &savePoint);
	// bpl::def("getPoint", &getPoint);
	// bpl::def("saveGroup", &saveGroup);
	// bpl::def("getGroup", &getGroup);
    bpl::scope().attr("Press") = GLFW_PRESS;
    bpl::scope().attr("Release") = GLFW_RELEASE;
    bpl::scope().attr("Repeat") = GLFW_REPEAT;
    bpl::scope().attr("Pause") = GLFW_KEY_PAUSE;
    bpl::scope().attr("Enter") = GLFW_KEY_ENTER;
    bpl::scope().attr("Esc") = GLFW_KEY_ESCAPE;
    bpl::scope().attr("Ctrl") = GLFW_MOD_CONTROL;
    bpl::scope().attr("Shift") = GLFW_MOD_SHIFT;
    bpl::scope().attr("Alt") = GLFW_MOD_ALT;
    bpl::scope().attr("F1") = GLFW_KEY_F1;
    bpl::scope().attr("F2") = GLFW_KEY_F2;
    bpl::scope().attr("F3") = GLFW_KEY_F3;
    bpl::scope().attr("F4") = GLFW_KEY_F4;
    bpl::scope().attr("F5") = GLFW_KEY_F5;
    bpl::scope().attr("F6") = GLFW_KEY_F6;
    bpl::scope().attr("F7") = GLFW_KEY_F7;
    // bpl::scope().attr("F8") = GLFW_KEY_F8;
    bpl::scope().attr("F9") = GLFW_KEY_F9;
    bpl::scope().attr("F10") = GLFW_KEY_F10;
    bpl::scope().attr("F11") = GLFW_KEY_F11;
    bpl::scope().attr("F12") = GLFW_KEY_F12;
}

std::unordered_map<std::string, int> keyMaps = {
    {}

};

bpl::object main;
bpl::object global;
bpl::object mainScript;
std::string mainScriptName;

void handleInput(int key, int action, int mod, shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	try {
		mainScript.attr("handleInput")(key, action, mod, rc, scene);
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
        std::cin.ignore();
	}
}

void init(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, const std::string &name){
	Py_Initialize();
	try {
		initglm_export();
		initscene_export();
		initrobotController_export();
		initcommandBuilders_export();
		inithelper_export();
		main = bpl::import("__main__");
		global = bpl::object(main.attr("__dict__"));
		bpl::str script(
				"import sys, os.path\n"
				"path = os.path.dirname(%r)\n"
				"sys.path.insert(0, path)"
				% bpl::str("module")
				);
		bpl::object result = bpl::exec(script, global, global);
		mainScriptName = name;
		mainScript = bpl::import(mainScriptName.c_str());
		mainScript.attr("init")(rc, scene);

	}
	catch (bpl::error_already_set) {
		PyErr_Print();
        terminate();
	}
}

void loadMainScript(const string &name, shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	try {
		mainScriptName = name;
		mainScript = bpl::import(name.c_str());
		mainScript.attr("init")(rc, scene);
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
				terminate();
	}
}

void reloadMainScript(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	try {
		mainScript = bpl::import(mainScriptName.c_str());
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
        terminate();
	}
}

void reloadAndInitMainScript(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	try {
		mainScript = bpl::import(mainScriptName.c_str());
		mainScript.attr("init")(rc, scene);
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
				terminate();
	}
}

void update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	try {
		mainScript.attr("update")(rc, scene, 0.01);
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
        std::cin.ignore();
	}
}

void terminate(){
	Py_Finalize();
}

NAM_END