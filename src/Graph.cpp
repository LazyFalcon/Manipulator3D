#include "Utils/Includes.h"
#include "Utils/Utils.h"
#include "Graph.h"

void Graph::push(const glm::vec2 &point){
	data.push_back(point);
	modified = true;
	if((flags & FixedSize) != FixedSize){
		bounds.x = std::min(bounds.x, point.x);
		bounds.y = std::max(bounds.y, point.x);
		bounds.z = std::min(bounds.z, point.y);
		bounds.w = std::max(bounds.w, point.y);
	}
}
void Graph::push(float value){
	data.emplace_back(valuePosition++, value);
	modified = true;

	if(flags & Cyclic){
		valuePosition %= maxSize;
	}
	else if(flags & Chunked){
		valuePosition %= maxSize;
		if(valuePosition == 0) data.clear();
	}

	if((flags & FixedSize) != FixedSize){
		bounds.y = std::max(bounds.y, float(valuePosition));
		if(flags & LastX) bounds.x = float(bounds.y-maxSize);
		bounds.z = std::min(bounds.z, value);
		bounds.w = std::max(bounds.w, value);
	}
}

std::list<Plot*> plotList;
void initPlotList(){
	for(auto &it : plotList)
		it->init();
}
void drawPlotList(){
	for(auto &it : plotList)
		it->plot();
}

