#pragma once
#include <boost/python.hpp>

#define NAM_END }
#define NAM_START {

namespace PythonBindings NAM_START
namespace bpl = boost::python;

extern bpl::object main;
extern bpl::object global;
extern bpl::object mainScript;

void init(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene, const std::string &name = "SampleScript");
void update(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
void terminate();

void loadMainScript(const string &name, shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
void reloadMainScript(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);
void reloadAndInitMainScript(shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);


void handleInput(int key, int action, int mod, shared_ptr<RobotController> &rc, shared_ptr<Scene> &scene);

std::vector<std::string> getScriptList();
bpl::object loadFile(std::string);

NAM_END