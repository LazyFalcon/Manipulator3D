#include "Utils/Includes.h"
#include "Utils/Utils.h"
#include "Utils/BaseStructs.h"
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

#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#define _DebugLine_ std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";

#include "PythonBindings.h"

#define NAM_END }
#define NAM_START {

shared_ptr<RobotController> getRC();
shared_ptr<Scene> getScene();
namespace PythonBindings NAM_START


glm::quat glm_angleAxis(float angle, glm::vec3 axis){
	return glm::quat(cos(angle*0.5f), glm::normalize(axis));
}
glm::quat glm_quatreverse(glm::quat q){
	return -q;
}

BOOST_PYTHON_MODULE(Manipulator3D){

	bpl::class_<std::vector<glm::vec4>>("Vec4Vec")
		.def(bpl::vector_indexing_suite<std::vector<glm::vec4>>());
	bpl::class_<std::vector<double>>("DoubleVec")
		.def(bpl::vector_indexing_suite<std::vector<double>>());

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
	bpl::def("quatRev", glm_quatreverse);

	bpl::def("eulerAngles", cpp_eulerAngles);
	bpl::def("angleAxis", glm_angleAxis);

	// bpl::class_<glm::mat4>("mat4", bpl::init<float>())
		// .def(bpl::init<float>())
		// .def(bpl::self + glm::mat4())
		// .def(bpl::self - glm::mat4())
		// .def(bpl::self * glm::mat4())
		// .def(bpl::self * glm::vec4())
		// .def(bpl::self * float())
		// ;
	bpl::class_<glm::quat>("quat", bpl::init<float, float, float, float>())
		.def(bpl::init<float, glm::vec3>())
		.def(bpl::init<glm::vec3>())
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
	MoveCommandBuilder& (MoveCommandBuilder::*interpolator_ptr)(IInterpolatorContainer&) = &MoveCommandBuilder::interpolator;
	MoveCommandBuilder& (MoveCommandBuilder::*solver_string)(const std::string&) = &MoveCommandBuilder::solver;
	MoveCommandBuilder& (MoveCommandBuilder::*solver_enum)(SolverType) = &MoveCommandBuilder::solver;
	MoveCommandBuilder& (MoveCommandBuilder::*orientation_quat)(glm::quat) = &MoveCommandBuilder::orientation;
	MoveCommandBuilder& (MoveCommandBuilder::*orientation_vec3)(glm::vec3) = &MoveCommandBuilder::orientation;
	MoveCommandBuilder& (MoveCommandBuilder::*finish_ptr)(shared_ptr<RobotController>) = &MoveCommandBuilder::finish;

	bpl::class_<MoveCommandBuilder, std::shared_ptr<MoveCommandBuilder>>("MoveCommandBuilder", bpl::init<>())
		.def_readwrite("moveCommand", &MoveCommandBuilder::moveCommand)
		.def("init", &MoveCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &MoveCommandBuilder::name, bpl::return_internal_reference<>())
		.def("velocity", &MoveCommandBuilder::velocity, bpl::return_internal_reference<>())
		.def("startO", &MoveCommandBuilder::startO, bpl::return_internal_reference<>())
		.def("endO", &MoveCommandBuilder::endO, bpl::return_internal_reference<>())
		.def("jointVelocity", &MoveCommandBuilder::jointVelocity, bpl::return_internal_reference<>())
		.def("acceleration", &MoveCommandBuilder::acceleration, bpl::return_internal_reference<>())
		.def("treshold", &MoveCommandBuilder::treshold, bpl::return_internal_reference<>())
		.def("time", &MoveCommandBuilder::time, bpl::return_internal_reference<>())
		.def("interpolator", interpolator_ptr, bpl::return_internal_reference<>())
		.def("solver", solver_string, bpl::return_internal_reference<>())
		.def("solver", solver_enum, bpl::return_internal_reference<>())
		/// for goTo command
		.def("to", &MoveCommandBuilder::to, bpl::return_internal_reference<>())
		.def("orientation", orientation_quat, bpl::return_internal_reference<>())
		.def("orientation", orientation_vec3, bpl::return_internal_reference<>())
		.def("offset", &MoveCommandBuilder::offset, bpl::return_internal_reference<>())
		.def("pid", &MoveCommandBuilder::pid, bpl::return_internal_reference<>())

		.def("finish", finish_ptr, bpl::return_internal_reference<>())
		.def("insert", &MoveCommandBuilder::insert, bpl::return_internal_reference<>())
		;

	SingleJointMoveCommandBuilder& (SingleJointMoveCommandBuilder::*singlefinish_ptr)(shared_ptr<RobotController>) = &SingleJointMoveCommandBuilder::finish;
	bpl::class_<SingleJointMoveCommandBuilder, std::shared_ptr<SingleJointMoveCommandBuilder>>("SingleJointMoveCommandBuilder", bpl::init<>())
		.def("init", &SingleJointMoveCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &SingleJointMoveCommandBuilder::name, bpl::return_internal_reference<>())
		.def("set", &SingleJointMoveCommandBuilder::set, bpl::return_internal_reference<>())
		.def("velocity", &SingleJointMoveCommandBuilder::velocity, bpl::return_internal_reference<>())
		.def("jointVelocity", &SingleJointMoveCommandBuilder::jointVelocity, bpl::return_internal_reference<>())
		.def("acceleration", &SingleJointMoveCommandBuilder::acceleration, bpl::return_internal_reference<>())
		.def("insert", &SingleJointMoveCommandBuilder::insert, bpl::return_internal_reference<>())
		.def("finish", singlefinish_ptr, bpl::return_internal_reference<>())
		;

	FollowObjectBuilder& (FollowObjectBuilder::*target_1)(glm::vec4 &t) = &FollowObjectBuilder::target;
	FollowObjectBuilder& (FollowObjectBuilder::*target_2)(shared_ptr<Entity> &obj) = &FollowObjectBuilder::target;
	FollowObjectBuilder& (FollowObjectBuilder::*followfinish_ptr)(shared_ptr<RobotController>) = &FollowObjectBuilder::finish;
	// bpl::class_<FollowObjectBuilder, std::shared_ptr<FollowObjectBuilder>>("FollowObjectBuilder", bpl::init<>())
		// .def("init", &FollowObjectBuilder::init, bpl::return_internal_reference<>())
		// .def("name", &FollowObjectBuilder::name, bpl::return_internal_reference<>())
		// .def("target", target_1, bpl::return_internal_reference<>())
		// .def("target", target_2, bpl::return_internal_reference<>())
		// .def("velocity", &FollowObjectBuilder::velocity, bpl::return_internal_reference<>())
		// .def("jointVelocity", &FollowObjectBuilder::jointVelocity, bpl::return_internal_reference<>())
		// .def("acceleration", &FollowObjectBuilder::acceleration, bpl::return_internal_reference<>())
		// .def("insert", &FollowObjectBuilder::insert, bpl::return_internal_reference<>())
		// .def("finish", followfinish_ptr, bpl::return_internal_reference<>())
		// ;

	WaitCommandBuilder& (WaitCommandBuilder::*waitfinish_ptr)(shared_ptr<RobotController>&) = &WaitCommandBuilder::finish;

	bpl::class_<ExecuteCommandBuilder, std::shared_ptr<ExecuteCommandBuilder>>("ExecuteCommandBuilder", bpl::init<>())
		.def("init", &ExecuteCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &ExecuteCommandBuilder::name, bpl::return_internal_reference<>())
		.def("insert", &ExecuteCommandBuilder::insert, bpl::return_internal_reference<>())
		.def("finish", &ExecuteCommandBuilder::finish, bpl::return_internal_reference<>())
		;

	bpl::class_<ExecutePythonCommandBuilder, std::shared_ptr<ExecutePythonCommandBuilder>>("ExecutePythonCommandBuilder", bpl::init<>())
		.def("init", &ExecutePythonCommandBuilder::init, bpl::return_internal_reference<>())
		.def("name", &ExecutePythonCommandBuilder::name, bpl::return_internal_reference<>())
		.def("fun", &ExecutePythonCommandBuilder::fun, bpl::return_internal_reference<>())
		.def("callback", &ExecutePythonCommandBuilder::callback, bpl::return_internal_reference<>())
		.def("insert", &ExecutePythonCommandBuilder::insert, bpl::return_internal_reference<>())
		.def("finish", &ExecutePythonCommandBuilder::finish, bpl::return_internal_reference<>())
		;
	// bpl::class_<WaitCommandBuilder, std::shared_ptr<WaitCommandBuilder>>("WaitCommandBuilder", bpl::init<>())
		// .def("init", &WaitCommandBuilder::init, bpl::return_internal_reference<>())
		// .def("name", &WaitCommandBuilder::name, bpl::return_internal_reference<>())
		// .def("time", &WaitCommandBuilder::time, bpl::return_internal_reference<>())
		// .def("insert", &WaitCommandBuilder::insert, bpl::return_internal_reference<>())
		// .def("finish", waitfinish_ptr, bpl::return_internal_reference<>())
		// ;

	bpl::enum_<SolverType>("SolverType")
		.value("JT0", SolverType::JT0)
		.value("JT1", SolverType::JT1)
		.value("JT2", SolverType::JT2)
		.value("CCD", SolverType::CCD)
		;
	bpl::enum_<CommandReturnAction::CommandReturn>("CommandReturnAction")
		.value("GoToNext", CommandReturnAction::GoToNext)
		.value("Delete", CommandReturnAction::Delete)
		.value("GoToPrevious", CommandReturnAction::GoToPrevious)
		.value("DelAndForward", CommandReturnAction::DelAndForward)
		.value("DelAndBack", CommandReturnAction::DelAndBack)
		;
	bpl::enum_<Interpolator>("Interpolator")
		.value("Empty", Interpolator::Empty)
		.value("Open", Interpolator::Open)
		.value("Closed", Interpolator::Closed)
		.value("Linear", Interpolator::Linear)
		.value("Simple", Interpolator::Simple)
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

	bpl::class_<Entity, std::shared_ptr<Entity>, boost::noncopyable>("Entity", bpl::no_init)
	// bpl::class_<std::shared_ptr<Entity>, boost::noncopyable>("Entity", bpl::no_init)
		.def_readonly("ID", &Entity::ID)
		.def_readwrite("position", &Entity::position)
		.def_readwrite("quat", &Entity::quat)
		;
	bpl::class_<std::vector<std::shared_ptr<Entity>>>("EntityVec")
		.def(bpl::vector_indexing_suite<std::vector<std::shared_ptr<Entity>>, true>())
		;

	bpl::class_<Scene, std::shared_ptr<Scene>>("Scene", bpl::no_init)
		// .def("get", &Scene::get, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("get", &Scene::get)
		.def_readwrite("fixedLoopStep", &Scene::fixedLoopStep)
		;
	bpl::def("getScene", getScene);
	bpl::def("getRC", getRC);

	void (RobotController::*pushCommand_ptr)(shared_ptr<ICommand> ptr) = &RobotController::pushCommand;
	bpl::class_<RobotController, std::shared_ptr<RobotController>>("RobotController")
		.def("pushCommand", pushCommand_ptr)
		.def("run", &RobotController::run)
		.def("pause", &RobotController::pause)
		.def("stop", &RobotController::stop)
		.def("next", &RobotController::next)
		.def("prev", &RobotController::prev)
		.def("savePosition", &RobotController::savePosition)
		.def("popPosition", &RobotController::popPosition)
		.def("peekPosition", &RobotController::peekPosition)
		.def("clean", &RobotController::clean)
		.def("grab", &RobotController::grab, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("release", &RobotController::release, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("wait", &RobotController::wait, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("move", &RobotController::move, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("pyExec", &RobotController::pyExec, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("jointMove", &RobotController::jointMove, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("follow", &RobotController::follow, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("goTo", &RobotController::goTo, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("getRobot", &RobotController::getRobot, bpl::return_value_policy<bpl::reference_existing_object>())
		.def("getRobotJ", &RobotController::getRobotJ)
		.def_readonly("robot", &RobotController::robot)
		.def_readwrite("state ", &RobotController::state )
		.def_readwrite("commands", &RobotController::commands)
		;

	bpl::class_<SystemSettings>("SystemSettings")
		.def_readwrite("positionPrecision", &SystemSettings::positionPrecision)
		.def_readwrite("orientationPrecision", &SystemSettings::orientationPrecision)
		.def_readwrite("solverIterationLimit", &SystemSettings::solverIterationLimit)
		.def_readwrite("useRobotConstraints", &SystemSettings::useRobotConstraints)
		.def_readwrite("enableCollisions", &SystemSettings::enableCollisions)
		;
	bpl::class_<Robot, std::shared_ptr<Robot>, boost::noncopyable>("Robot")
		.def("getModuleCount", &Robot::getModuleCount)
		.def("module", &Robot::module, bpl::return_value_policy<bpl::reference_existing_object>())
		.def_readwrite("config", &Robot::config)
		;
	bpl::class_<Module, boost::noncopyable>("Module")
		.def_readwrite("value", &Module::value)
		.def_readwrite("targetValue", &Module::targetValue)
		.def_readwrite("maxVelocty", &Module::maxVelocty)
		.def_readwrite("maxAcceleration", &Module::maxAcceleration)
		.def_readwrite("lastVelocity", &Module::lastVelocity)
		.def_readwrite("lastAcceleration", &Module::lastAcceleration)
		.def_readwrite("axis", &Module::axis)
		.def_readwrite("vecToA", &Module::vecToA)
		.def_readwrite("vecToB", &Module::vecToB)
		;

	using namespace Helper;

	bpl::class_<FrameRecordedData>("FrameRecordedData")
		.def_readonly("IKIterationTime", &FrameRecordedData::IKIterationTime)
		.def_readonly("IKIterarationCount", &FrameRecordedData::IKIterarationCount)
		.def_readonly("IKPositionError", &FrameRecordedData::IKPositionError)
		.def_readonly("IKOrientationError", &FrameRecordedData::IKOrientationError)
		.def_readonly("EffectorDelta", &FrameRecordedData::EffectorDelta)
		.def_readonly("EffectorVelocity", &FrameRecordedData::EffectorVelocity)
		.def_readonly("EffectorAcceleration", &FrameRecordedData::EffectorAcceleration)
		.def_readonly("FrameTime", &FrameRecordedData::FrameTime)
		.def_readonly("EffectorPosition", &FrameRecordedData::EffectorPosition)
		.def_readonly("EffectorOrientation", &FrameRecordedData::EffectorOrientation)
		.def_readonly("RobotJoints", &FrameRecordedData::RobotJoints)
		;
	bpl::def("getRecord", &record, bpl::return_value_policy<bpl::reference_existing_object>());


	bpl::def("getPositionUnderMouse", &Helper::getPositionUnderMouse);
	bpl::def("getNormalUnderMouse", &Helper::getNormalUnderMouse);
	// bpl::def("getObjectUnderMouse", &Helper::getObjectUnderMouse, bpl::return_value_policy<bpl::reference_existing_object>());
	bpl::def("unselect", &Helper::unselect);
	bpl::def("getSelection", &Helper::getSelection, bpl::return_value_policy<bpl::reference_existing_object>());
	bpl::def("getCursor", &Helper::getCursor, bpl::return_value_policy<bpl::reference_existing_object>());
	bpl::def("setCursor", &Helper::setCursor);
	bpl::def("moveCursor", &Helper::moveCursor);
	bpl::def("restoreCursorPos", &Helper::restoreCursorPos);

	// bpl::def("getGroup", &Helper::getGroup, bpl::return_value_policy<bpl::reference_existing_object>());
	// bpl::def("appendToSelection", &Helper::appendToSelection);

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

    // bpl::class_<std::vector<glm::vec4>>("XVec")
            // .def(bpl::vector_indexing_suite<std::vector<glm::vec4>>())
            // ;
    // bpl::class_<std::vector<float>>("FloatVec")
            // .def(bpl::vector_indexing_suite<std::vector<float>>())
            // ;
    // bpl::class_<std::vector<double>>("DoubleVec")
            // .def(bpl::vector_indexing_suite<std::vector<double>>())
            // ;

    // bpl::def("getF", &getFloat, bpl::return_value_policy<bpl::reference_existing_object>());
    bpl::def("getD", &getDouble, bpl::return_value_policy<bpl::reference_existing_object>());
    // bpl::def("getVec", &getVec4, bpl::return_value_policy<bpl::reference_existing_object>());

    // bpl::def("store", &storeFloat);
    bpl::def("store", &storeDouble);
    bpl::def("saveRobot", &saveRobot);
    bpl::def("loadRobot", &loadRobot);
    // bpl::def("store", &storeVec4);
}

bpl::object main;
bpl::object global;
bpl::object mainScript;
bpl::object subScript;
std::string mainScriptName;
std::string subScriptName;

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
		initManipulator3D();
		main = bpl::import("__main__");
		// global = bpl::object(main.attr("__dict__"));
		global = main.attr("__dict__");
		bpl::str script(
				"import sys, os.path\n"
				"path = os.path.dirname(%r)\n"
				"sys.path.insert(0, path)"
				% bpl::str("../python/")
				);
		bpl::object result = bpl::exec(script, global, global);
		mainScriptName = name;
		// mainScript = bpl::import(mainScriptName.c_str());
		// mainScript.attr("init")(rc, scene);

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
		global[name.c_str()] = mainScript;
		mainScript = global[name.c_str()];
		mainScript.attr("init")(rc, scene);
		cout<<"[ Main script loaded ] " + mainScriptName<<endl;
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
				terminate();
	}
}

void reloadMainScript(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	try {
        string scr = "reload("+mainScriptName+")";

		bpl::object result = bpl::exec(scr.c_str(), global, global);
		mainScript = bpl::import(mainScriptName.c_str());

		cout<<"[ Main script reloaded ] " + mainScriptName<<endl;
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
				terminate();
	}
}

void reloadAndInitMainScript(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene){
	try {
        string scr = "reload("+mainScriptName+")";
		bpl::object result = bpl::exec(scr.c_str(), global, global);
		mainScript = bpl::import(mainScriptName.c_str());
		global[mainScriptName.c_str()] = mainScript;
		mainScript.attr("init")(rc, scene);

		cout<<"[ Main script reloaded and started ] " + mainScriptName<<endl;
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
		reloadMainScript(rc, scene);
        exit(0);
	}
}

void terminate(){
	Py_Finalize();
}

void executeSubScript(){
	try {
        string scr = "reload("+subScriptName+")";
        bpl::object result = bpl::exec(scr.c_str(), global, global);
        subScript = bpl::import(subScriptName.c_str());
	}
	catch (bpl::error_already_set) {
		PyErr_Print();
        std::cin.ignore();
        exit(0);
	}
}
void executeSubScript(const std::string &name){
    if(subScriptName == name){
        string scr = "reload("+subScriptName+")";
        bpl::object result = bpl::exec(scr.c_str(), global, global);
    }
    else {
        subScriptName = name;
        subScript = bpl::import(subScriptName.c_str());
		global[subScriptName.c_str()] = subScript;
    }
    subScript = bpl::import(subScriptName.c_str());

}


NAM_END