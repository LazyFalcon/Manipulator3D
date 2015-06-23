#pragma once
#include "Engine-DrawUtils.h"
class Graph
{
public:
	enum GraphStates
	{
		Data1D    = 0b1,
		Data2D    = 0b10,
		FixedSize = 0b100,
		Cyclic    = 0b1000, /// buffer isn't cleared
		Chunked   = 0b10000, /// after receiving x recorsd, buffer is cleared
		LastX     = 0b100000, /// shows last x records,
		Scaled    = 0b1000000,
	};

	bool getModified(){
		if(modified){
			modified = false;
			return true;
		}
		return false;
	}
	void push(const glm::vec2 &point);
	void push(float value);
	void update();
	void reset();
	void draw();
	void setBouds(const glm::vec4 &b){
		bounds = b;
	}
	glm::mat4 getProjection(){
		return glm::ortho(bounds.x, bounds.y, bounds.z, bounds.w);
	}
	int getCount(){
		// if(flags & LastX) return data.size() < maxSize ? data.size() : maxSize;
		// if(flags & Chunked) return valuePosition;
		return data.size();
	}
	Graph(std::string name, uint32_t flags, HexColor color, int maxSize = 0xFFFF) :
		name(name)
		,flags(flags)
		,dataColor(color)
		,maxSize(maxSize)
		// ,data(0xffff)
		{
			if((flags & FixedSize) != FixedSize){
				bounds.y = std::min(bounds.y, float(maxSize));
				if(flags & LastX) bounds.x = float(bounds.y-maxSize);


			}

		}

	std::string name;
	uint32_t flags;
	HexColor dataColor;

private:
	std::vector<glm::vec2> data;
	int maxSize {0};
	uint32_t valuePosition {0};

	glm::vec4 bounds; /// -x, +x, -y, +y
	bool modified = false;

};
/**
 *  zrobić tak by można było wyświetlać ostatnie x punktów, zapetlać, itp
 */



class Plot
{
public:
	void plot();
	void update();
	void push(Graph *graph){
		graphs.push_back(graph);
	}
	void remove(Graph *graph){
		for(auto it=graphs.begin(); it<graphs.end(); it++){
			if(*it == graph){
				graphs.erase(it);
				return;
			}
		}
	}
	/// creates FBO and texture
	void init();
	void setUpFBO();
	/// resizes texture
	void resize(glm::vec2 newDimensions);
	void move(glm::vec2 newDimensions){
		box.x = newDimensions.x;
		box.y = newDimensions.y;
	}

	Plot(const std::string &name, const glm::vec4 &box, HexColor color) : name(name), box(box), clearColor(colorHex(color))
	{}

	std::string name;
private:
	glm::vec4 box; /// screen position, dimensions
	std::vector<Graph*> graphs;
	uint32_t texID {0xffff};
	uint32_t fbo {0xffff};
	glm::vec4 clearColor;
	bool visible = true;
};

extern std::list<Plot*> plotList;
void initPlotList();
void drawPlotList();
