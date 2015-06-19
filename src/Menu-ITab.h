#pragma once

class ITab
{
public:
	virtual void run() = 0;

	virtual ~ITab(){}
	ITab(const std::string &icon) :
		icon(icon)
		{}

	const std::string icon;
};

