#pragma once
#include <boost/python.hpp>

#define NAM_END }
#define NAM_START {

namespace PythonBindings NAM_START
namespace bpl = boost::python;

extern bpl::object main;
extern bpl::object global;
extern bpl::object mainScript;

void init(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
void update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
void terminate(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);

std::vector<std::string> getScriptList();
bpl::object loadFile(std::string);

NAM_END