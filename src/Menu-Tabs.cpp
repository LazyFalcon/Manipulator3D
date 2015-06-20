#pragma once
#include "Menu-SideBar.h"
#include "Robot-CommandBuilders.h"


class CommandBuilderTab : public ITab
{
public:
	CommandBuilderTab(const std::string &iconName) : ITab(iconName), moveBuilder(){moveBuilder.init();}
	void run();

private:
	MoveBuilder moveBuilder;

};

class CommanListTab : public ITab
{
public:
	CommanListTab(const std::string &iconName) : ITab(iconName){}
	void run(){}
};

class PolylineBuilderTab : public ITab
{
public:
	PolylineBuilderTab(const std::string &iconName) : ITab(iconName){}
	void run(){}
};

class PolylineListTab : public ITab
{
public:
	PolylineListTab(const std::string &iconName) : ITab(iconName){}
	void run(){}
};

class PlotPropertyListTab : public ITab
{
public:
	PlotPropertyListTab(const std::string &iconName) : ITab(iconName){}
	void run(){}
};


