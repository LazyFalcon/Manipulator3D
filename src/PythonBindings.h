#pragma once
#include <boost/python.hpp>

#define NAM_END }
#define NAM_START {

namespace PythonBindings NAM_START
namespace bpl = boost::python;

void init();
void update();
void terminate();

std::vector<std::string> getScriptList();
bpl::object loadFile(std::string);

NAM_END