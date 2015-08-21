#include "Robot.h"
#include "Robot-Commands.h"
#include "RobotController.h"
#include "Editor/MoveCommandBuilder.h"
#include "Editor/ExecuteCommandBuilder.h"

#include <Python.h>
#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include "PythonBindings.h"

#define NAM_END }
#define NAM_START {

namespace PythonBindings NAM_START

BOOST_PYTHON_MODULE(glm){
    bpl::def("manipulateVector", &manipulateVector);

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

    bpl::class_<glm::mat4>("mat4", bpl::init<float>())
        .def(bpl::init<float>())
        .def(bpl::self + glm::mat4())
        .def(bpl::self - glm::mat4())
        .def(bpl::self * glm::mat4())
        .def(bpl::self * glm::vec4())
        .def(bpl::self * float())
        ;
    bpl::class_<glm::quat>("quat", bpl::init<float, float, float, float>())
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

BOOST_PYTHON_MODULE(commandBuilders){
    MoveCommandBuilder& (*interpolator_string)(const std::string& name) = &MoveCommandBuilder::interpolator;
    MoveCommandBuilder& (*interpolator_enum)(Interpolator value) = &MoveCommandBuilder::interpolator;
    MoveCommandBuilder& (*interpolator_ptr)(shared_ptr<IInterpolator> &value) = &MoveCommandBuilder::interpolator;
    MoveCommandBuilder& (*solver_string)(const std::string& name) = &MoveCommandBuilder::solver;


    bpl::class_<MoveCommandBuilder, std::shared_ptr<MoveCommandBuilder>>("MoveCommandBuilder")
        .def_readwrite("moveCommand", &MoveCommandBuilder::moveCommand)
        .def("init", &MoveCommandBuilder::init)
        .def("velocity", &MoveCommandBuilder::velocity)
        .def("jointVelocity", &MoveCommandBuilder::jointVelocity)
        .def("acceleration", &MoveCommandBuilder::acceleration)
        .def("time", &MoveCommandBuilder::time)
        .def("interpolator", interpolator_string)
        .def("interpolator", interpolator_enum)
        .def("interpolator", interpolator_ptr)
        .def("solver", solver_string)
        .def("finish", &MoveCommandBuilder::finish)
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
    bpl::class_<IInterpolator, std::shared_ptr<IInterpolator>>("IInterpolator", bpl::init<const std::vector<glm::vec4> &, Interpolator>(), const std::string &)
        .def("firstPoint", &IInterpolator::firstPoint)
        .def("nextPoint", &IInterpolator::nextPoint)
        .def("generatePath", &IInterpolator::generatePath)
        .def("reset", &IInterpolator::reset)
        .def_readwrite("type", &IInterpolator::type)
        .def_readwrite("finished", &IInterpolator::finished)
        .def_readwrite("name", &IInterpolator::name)
        .def_readwrite("points", &IInterpolator::points)
        ;

}
/**
 * raw pointers:
 http://stackoverflow.com/questions/5055443/boost-python-how-to-pass-a-c-class-instance-to-a-python-class/5056462#5056462
 * http://stackoverflow.com/questions/3881457/boostpython-howto-call-a-function-that-expects-a-pointer
 */
BOOST_PYTHON_MODULE(scene){
    bpl::class_<Entity, std::shared_ptr<Entity>>("Entity")
        .def_readonly("ID", &RobotController::ID)
        .def_readwrite("position", &RobotController::position)
        .def_readwrite("quat", &RobotController::quat)
        .def("rgBody", &RobotController::rgBodyRef)
        ;
    bpl::class_<Scene, std::shared_ptr<Scene>>("Scene")
        .def("getObject", &RobotController::getObject)
        ;
}
BOOST_PYTHON_MODULE(robotController){
    bpl::enum_<RCStates>("RCStates")
        .value("Run", RCStates::Run)
        .value("Pause", RCStates::Pause)
        .value("Stop", RCStates::Stop)
        ;

    void (*insertCommand_ptr)(shared_ptr<ICommand> ptr) = &MoveCommandBuilder::insertCommand;
    bpl::class_<RobotController, std::shared_ptr<RobotController>>("RobotController")
        .def("move", &RobotController::move)
        .def("wait", &RobotController::wait)
        .def("execute", &RobotController::execute)
        .def("insertCommand", insertCommand_ptr)
        .def("run", &RobotController::run)
        .def("pause", &RobotController::pause)
        .def("stop", &RobotController::stop)
        .def("next", &RobotController::next)
        .def("prev", &RobotController::prev)
        .def("savePosition", &RobotController::savePosition)
        .def("restorePosition", &RobotController::restorePosition)
        .def("grabObject", &RobotController::grabObject)
        .def("releaseObject", &RobotController::releaseObject)
        .def_readwrite("robot", &RobotController::robot)
        .def_readwrite("state ", &RobotController::state )
        // .def_readwrite("commands", &MoveCommandBuilder::commands)
        ;

    bpl::class_<std::vector<Module>>("ModuleVec")
        .def(bpl::vector_indexing_suite<std::vector<Module>>())
        ;

    void (*goTo_1_ptr)(float, double) = &Robot::goTo;
    void (*goTo_2_ptr)(const std::vector<double>&) = &Robot::goTo;
    bpl::class_<Robot, std::shared_ptr<Robot>>("Robot")
        .def("goTo", &Robot::goTo_1_ptr)
        .def("goTo", &Robot::goTo_2_ptr)
        .def_readwrite("chain", &Robot::chain)
        ;

    bpl::class_<Module, std::shared_ptr<Module>>("Module")
        .def("getMainAxis", &Module::getMainAxis)
        .def("getMainAxis3", &Module::getMainAxis3)
        .def_readwrite("axis", &Robot::axis)
        .def_readwrite("vecToA", &Robot::vecToA)
        .def_readwrite("vecToB", &Robot::vecToB)
        .def_readwrite("name", &Robot::name)
        .def_readwrite("entity", &Robot::entity)
        .def_readwrite("min", &Robot::min)
        .def_readwrite("max", &Robot::max)
        .def_readwrite("value", &Robot::value)
        .def_readwrite("maxVelocty", &Robot::maxVelocty)
        .def_readwrite("maxAcceleration", &Robot::maxAcceleration)
        .def_readwrite("lastVelocity", &Robot::lastVelocity)
        .def_readwrite("lastAcceleration", &Robot::lastAcceleration)
        ;
}

bpl::object main;
bpl::object global;
bpl::object mainScript;

void init(){
    Py_Initialize();
    try {
        initcpp();
        initcpp_vehicle();
        main = bpl::import("__main__");
        global = bpl::object(main.attr("__dict__"));
        bpl::str script(
            "import sys, os.path\n"
            "path = os.path.dirname(%r)\n"
            "sys.path.insert(0, path)"
            % bpl::str("module")
            );
        bpl::object result = bpl::exec(script, global, global);
        mainScript = bpl::import("testScript");

  } catch (bpl::error_already_set) {
    PyErr_Print();
  }

  vehicle_ptr = std::make_shared<Vehicle>();
  tank_ptr = std::make_shared<Tank>();
}

void update(){
    try {
        mainScript.attr("update")(vehicle_ptr, 0.01);

      } catch (bpl::error_already_set) {
        PyErr_Print();
      }
}

void terminate(){
    Py_Finalize();
}

NAM_END