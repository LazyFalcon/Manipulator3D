﻿#include "Utils/Includes.h"
#include "Utils/Utils.h"
#include "Utils/BaseStructs.h"
#include "Utils/IMGUI_V4.h"
#include "Robot.h"
#include "JacobianTransposed.h"
#include "IInterpolator.h"
#include "Widgets.h"

#define _DebugLine_  cerr<<"line: "<<__LINE__<<"\n";


/// -------------------------------- LINEAR --------------------------------
float Linear::distanceToEnd() const {
	if(section != maxSections-1) return 1000.f;
	return glm::distance(glm::mix(points[maxSections-1], points[maxSections], position), points[maxSections]);
}
glm::vec4 Linear::nextPoint(){
	if(section > maxSections){
		finished = true;
		return points[maxSections];
	}
	auto out = glm::mix(points[section], points[section+1], position);
	position = glm::clamp(position + singleStepLength, 0.0, (double)maxSections);
	finished = position == (double)maxSections;

	//auto out = points[section] + sectionStep*step;
	//step++;
	//if(sectionMaxSteps <= step)
	//	nextSection();
	return out;
}

void Linear::nextSection(){
	section++;
	step = 0;
	sectionStep = points[section+1] - points[section];
	sectionMaxSteps = glm::length(sectionStep)/singleStepLength;
	sectionStep /= sectionMaxSteps;
}
bool Linear::generatePath(){
    if(points.size() < 2) return false;
	visualisation.clear();
	visualisation.reserve(100);
	auto singleStepLengthTmp = singleStepLength;
	singleStepLength = 0.01;
	maxSections = points.size()-1;
	section = -1;
	step = 0;
	nextSection();
	finished = false;

	while(!finished){
		visualisation.push_back(nextPoint());
	}

	singleStepLength = singleStepLengthTmp;
    return true;
}
void Linear::reset(){
	finished = false;
	section = -1;
	nextSection();
}
void Linear::genNextStep(){
}
void Linear::drawParams(){
	wxg::editBox("step: ", singleStepLength, 160);
	ui.rect(120,20).text("smooth corers: false")();
	ui.rect(120,20).text("smooth radius")();
}

float Simple::distanceToEnd() const { return 1000; }
glm::vec4 Simple::nextPoint(){
	if(currentPoint >= maxPoints){
		finished = true;
		return points[maxPoints];
	}
	return points[currentPoint++];
}
bool Simple::generatePath(){
    return true;
}
void Simple::reset(){
	finished = false;
	currentPoint = 0;
}
void Simple::drawParams(){
}

/// -------------------------------- BEZIERCURVE --------------------------------
float BezierCurve::distanceToEnd() const {
	double left = 1.0 - position;

	glm::vec4 pts[4] = {
		eval(position),
		eval(position + left/3.0),
		eval(position + 2.0*left/3.0),
		eval(1),
	};

	float distance = 0;
	distance += glm::distance(pts[0], pts[1]);
	distance += glm::distance(pts[1], pts[2]);
	distance += glm::distance(pts[2], pts[3]);

	return distance;
}
glm::vec4 BezierCurve::nextPoint(){
	position = period(position + singleStepLength, 0, 1);

	return eval(position);
}
glm::vec4 BezierCurve::eval(double param) const {
	auto t = param;
	int n = points.size()-1;
	glm::vec4 out(0);
	float sum = 0;
	for(int i = 0; i<=n; i++){
			float tmp = Berenstein(t, n, i);
			sum += tmp;
			out += tmp * points[i];
	}
	return out/sum;
}
float BezierCurve::Berenstein(float t, int n, int i) const {
	// t - parametr
	// n - stopień krzywej
	// i - indeks punktu
	if(i<0 || i >n)
		return 0;
	return factorial(n)/factorial(n-i)/factorial(i)*pow(t, i)*pow(1.f-t, n-i);
}
float BezierCurve::factorial(int k) const {
	switch (k){
		case 0 : return 1;
		case 1 : return 1;
		case 2 : return 2;
		case 3 : return 6;
		case 4 : return 24;
		case 5 : return 120;
		case 6 : return 720;
		case 7 : return 5040;
		case 8 : return 40320;
		default : return 0;
	}
}
void BezierCurve::calculateLength(){}
bool BezierCurve::generatePath(){
	visualisation.clear();
    if(points.size() < 3) return false;
	auto singleStepLengthTmp = singleStepLength;
	singleStepLength = 0.01;
	for(float k=0.f; k<=1; k+=0.01f){
		visualisation.push_back(eval(k));
	}
	singleStepLength = singleStepLengthTmp;
    return true;
}
void BezierCurve::reset(){
	position = 0;
}
void BezierCurve::drawParams(){
	// wxg::editBox("step: ", singleStepLength, 160);
	// for(u32 i=0; i<weights.size(); i++)
		// wxg::editBox("weight "+std::to_string(i)+": ", weights[i], 160);
}

/// -------------------------------- BSPLINE --------------------------------
float BSpline::distanceToEnd() const {
	double left = (double)numOfBeziers - position;

	glm::vec4 pts[4] = {
		eval(position),
		eval(position + left/3.0),
		eval(position + 2.0*left/3.0),
		eval(numOfBeziers),
	};

	float distance = 0;
	distance += glm::distance(pts[0], pts[1]);
	distance += glm::distance(pts[1], pts[2]);
	distance += glm::distance(pts[2], pts[3]);

	return distance;
}
glm::vec4 BSpline::nextPoint(){
	position = glm::clamp(position + singleStepLength, 0.0, (double)numOfBeziers);
	finished = position == (double)numOfBeziers;
	return eval(position);
}
void BSpline::calculateLength(){}
/// daloby sie to wyliczac w runtime? bez cachowania bezierów?
bool BSpline::generatePath(){
	makeNurbs();
    if(points.size() < 4) return false;
	visualisation.clear();
	auto singleStepLengthTmp = singleStepLength;
	singleStepLength = 0.01;
	for(float k=0.f; k<=numOfBeziers; k+=0.01f){
		visualisation.push_back(eval(k));
	}
	singleStepLength = singleStepLengthTmp;
    return true;
}
void BSpline::makeNurbs(){
	beziers.clear();
	// http://pl.wikipedia.org/wiki/Krzywa_B-sklejana
	// Bezier is made from points{e,f,g,e+1}
	//dla size=8 => m=5, 5xbezier, wszystkich controlsów jest bezier*3+1 0 0
	int degree = 3;
	int m = points.size() - degree;
	numOfBeziers = points.size()-degree;
	vector<glm::vec4> f(m);
	vector<glm::vec4> g(m);
	vector<glm::vec4> e(m+1);

	for(int i=1; i<=m-2; ++i){
		f[i] = (2.f*points[i+1] + points[i+2])/3.f;
		g[i] = (points[i+1] + 2.f*points[i+2])/3.f;
	}

	f[0] = points[1];
	g[0] = (points[1] + points[2])/2.f;
	f[m-1] = (points[m] + points[m+1])/2.f;
	g[m-1] = points[m+1];

	for(int i=1; i<m; ++i)
		e[i] = (g[i-1] + f[i])/2.f;
	e[0] = points[0];
	e[m] = points[m+2];

	for(int i=0; i<m; i++){
		f[i].w = weight;
		g[i].w = weight;
		beziers.push_back(e[i]);
		beziers.push_back(f[i]);
		beziers.push_back(g[i]);
	}
	beziers.push_back(e.back());

}

glm::vec4 BSpline::eval(double param) const {
	int n = 3;
	double integral;
	double t = modf(param, &integral);
	int start = floor(param)*3;
	glm::vec4 out(0);
	float sum = 0;
	for(int i = start; i<start+4; i++){
			float tmp = Berenstein(t, n, i-start) * beziers[i].w;
			sum += tmp;
			out += tmp * beziers[i];
	}
	return out/sum;
}
float BSpline::Berenstein(float t, int n, int i) const {
	// t - parametr
	// n - stopień krzywej
	// i - indeks punktu
	if(i<0 || i >n)
		return 0;
	return factorial(n)/factorial(n-i)/factorial(i)*pow(t, i)*pow(1.f-t, n-i);
}
float BSpline::factorial(int k) const {
	switch (k){
		case 0 : return 1;
		case 1 : return 1;
		case 2 : return 2;
		case 3 : return 6;
		case 4 : return 24;
		case 5 : return 120;
		case 6 : return 720;
		case 7 : return 5040;
		case 8 : return 40320;
		default : return 0;
	}
}
void BSpline::reset(){
	position = 0;
}

std::vector<BezierCurve> BSpline::split(){return std::vector<BezierCurve>{};}
void BSpline::drawParams(){
	wxg::editBox("weight: ", weight, 160); /// a może slider? kolowy?
}

/// -------------------------------- NURBS --------------------------------

/// -------------------------------- HERMITECARDINAL --------------------------------
float HermiteCardinal::distanceToEnd() const {
	double left = (double)numOfSegments - position;

	glm::vec4 pts[4] = {
		eval(position),
		eval(position + left/3.0),
		eval(position + 2.0*left/3.0),
		eval(numOfSegments),
	};

	float distance = 0;
	distance += glm::distance(pts[0], pts[1]);
	distance += glm::distance(pts[1], pts[2]);
	distance += glm::distance(pts[2], pts[3]);

	return distance;
}
glm::vec4 HermiteCardinal::nextPoint(){
	position = glm::clamp(position + singleStepLength, 1.0, (double)(numOfSegments));
	finished = position == (double)(numOfSegments);
	return eval(position);
}
void HermiteCardinal::reset(){
	position = 0;
}
bool HermiteCardinal::generatePath(){
    numOfSegments = points.size()-2;
    if(numOfSegments < 4) return false;

	visualisation.clear();
	visualisation.reserve(numOfSegments*100);

	auto singleStepLengthTmp = singleStepLength;
	singleStepLength = 0.01;

	for(float k=1.f; k<=numOfSegments; k+=0.01f){
		visualisation.push_back(eval(k));
	}

	singleStepLength = singleStepLengthTmp;
	return true;
}
void HermiteCardinal::calculateLength(){}
/// tension/weight proporcjonalny do odleglosci?
glm::vec4 HermiteCardinal::eval(double param) const {
	double integral;
	double t = modf(param, &integral);
	double t3 = t*t*t;
	double t2 = t*t;
	int i = integral;

	auto &p0 = points[i-1];
	auto &p1 = points[i];
	auto &p2 = points[i+1];
	auto &p3 = points[i+2];

	auto m1 = float(1-tension)*(p2-p0)/weight;
	auto m2 = float(1-tension)*(p3-p1)/weight;

	return float(2*t3 - 3*t2 + 1)*p1 + float(t3 - 2*t2 + t)*m1 + float(-2*t3 + 3*t2)*p2 + float(t3 - t2)*m2;
}
void HermiteCardinal::drawParams(){
	wxg::editBox("weight: ", weight, 160);
	wxg::editBox("tension: ", tension, 160);
}

/// -------------------------------- HERMITEFINITEDIFFERENCE --------------------------------
float HermiteFiniteDifference::distanceToEnd() const {
	double left = (double)numOfSegments - position;

	glm::vec4 pts[4] = {
		eval(position),
		eval(position + left/3.0),
		eval(position + 2.0*left/3.0),
		eval(numOfSegments),
	};

	float distance = 0;
	distance += glm::distance(pts[0], pts[1]);
	distance += glm::distance(pts[1], pts[2]);
	distance += glm::distance(pts[2], pts[3]);

	return distance;
}
glm::vec4 HermiteFiniteDifference::nextPoint(){
	position = glm::clamp(position + singleStepLength, 0.0, (double)(numOfSegments));
	finished = position == (double)(numOfSegments);
	return eval(position);
}
void HermiteFiniteDifference::reset(){
	position = 0;
}
bool HermiteFiniteDifference::generatePath(){
    numOfSegments = points.size()-1;
    if(numOfSegments < 4) return false;

	visualisation.clear();
	visualisation.reserve(numOfSegments*100);

	auto singleStepLengthTmp = singleStepLength;
	singleStepLength = 0.01;

	for(float k=0.f; k<=numOfSegments; k+=0.01f){
		visualisation.push_back(eval(k));
	}

	singleStepLength = singleStepLengthTmp;
    return true;
}
void HermiteFiniteDifference::calculateLength(){}
glm::vec4 HermiteFiniteDifference::eval(double param) const {
	double integral;
	double t = modf(param, &integral);
	double t3 = t*t*t;
	double t2 = t*t;
	int i = integral;

	if(t == 0.f)
		return points[i];
	if(i == numOfSegments)
		return points[numOfSegments];

	auto &p1 = points[i];
	auto &p2 = points[i+1];
	auto m1 = (p2 - p1)/weight; // p2-p1  + (p1-p0)
	auto m2 = (p2 - p1)/weight; // (p3-p2)  + p2-p1

	if(i!=0){
		auto &p0 = points[i-1];
		m1 += (p1 - p0)/weight;
	}
	if(i < numOfSegments-1){
		auto &p3 = points[i+2];
		m2 += (p3 - p2);
	}

	return float(2*t3 - 3*t2 + 1)*p1 + float(t3 - 2*t2 + t)*m1 + float(-2*t3 + 3*t2)*p2 + float(t3 - t2)*m2;
}
void HermiteFiniteDifference::drawParams(){
	wxg::editBox("weight: ", weight, 160);
	}

/// -------------------------------- HERMITEFINITEDIFFERENCECLOSED --------------------------------
float HermiteFiniteDifferenceClosed::distanceToEnd() const {
	double left = (double)numOfSegments - position;

	glm::vec4 pts[4] = {
		eval(position),
		eval(position + left/3.0),
		eval(position + 2.0*left/3.0),
		eval(numOfSegments),
	};

	float distance = 0;
	distance += glm::distance(pts[0], pts[1]);
	distance += glm::distance(pts[1], pts[2]);
	distance += glm::distance(pts[2], pts[3]);

	return distance;
}
glm::vec4 HermiteFiniteDifferenceClosed::nextPoint(){
	position = glm::clamp(position + singleStepLength, 0.0, (double)(numOfSegments));
	finished = position == (double)(numOfSegments);
	return eval(position);
}
void HermiteFiniteDifferenceClosed::reset(){
	position = 0;
}
bool HermiteFiniteDifferenceClosed::generatePath(){
    numOfSegments = points.size();
    if(numOfSegments < 4) return false;

	visualisation.clear();
	visualisation.reserve(numOfSegments*100);

	auto singleStepLengthTmp = singleStepLength;
	singleStepLength = 0.01;


	for(float k=0.f; k<=numOfSegments; k+=0.01f){
		visualisation.push_back(eval(k));
	}

	singleStepLength = singleStepLengthTmp;
    return true;
}
void HermiteFiniteDifferenceClosed::calculateLength(){}
glm::vec4 HermiteFiniteDifferenceClosed::eval(double param) const {
	double integral;
	double t = modf(param, &integral);
	double t3 = t*t*t;
	double t2 = t*t;
	int i = integral;

	if(t == 0.f)
		return points[i];
	if(i == numOfSegments)
		return points[0];

	auto &p0 = points[(i-1+numOfSegments)%numOfSegments];
	auto &p1 = points[i];
	auto &p2 = points[(i+1)%numOfSegments];
	auto &p3 = points[(i+2)%numOfSegments];
	auto m1 = (p2 - p1)/weight + (p1 - p0)/weight; // p2-p1  + (p1-p0)
	auto m2 = (p2 - p1)/weight + (p3 - p2)/weight; // (p3-p2)  + p2-p1
	return float(2*t3 - 3*t2 + 1)*p1 + float(t3 - 2*t2 + t)*m1 + float(-2*t3 + 3*t2)*p2 + float(t3 - t2)*m2;
}
void HermiteFiniteDifferenceClosed::drawParams(){
	wxg::editBox("weight: ", weight, 160);
	}

/// -------------------------------- FACTORY --------------------------------
// shared_ptr<IInterpolator> build(const std::string type, vector<glm::vec4> &points){}

std::list<shared_ptr<IInterpolator>> g_interpolators;

void addInterpolator(shared_ptr<IInterpolator> &interpolator){
	g_interpolators.push_back(interpolator);
}

shared_ptr<IInterpolator> addInterpolator(const std::string type, vector<glm::vec4> &points){}
IInterpolatorContainer addInterpolatorByContainer(Interpolator type, const vector<glm::vec4> &points, const string &name){
	shared_ptr<IInterpolator> interpolator = addInterpolator(type, points, name);
	return {interpolator, interpolator->name};
}
shared_ptr<IInterpolator> addInterpolator(Interpolator type, const vector<glm::vec4> &points, const string &name){
	shared_ptr<IInterpolator> out;
	if(type == Interpolator::Linear){
		out = make_shared<Linear>(points);
	}
	else if(type == Interpolator::Simple){
		out = make_shared<Simple>(points);
	}
	else if(type == Interpolator::BezierCurve){
		out = make_shared<BezierCurve>(points);
	}
	else if(type == Interpolator::BSpline){
		out = make_shared<BSpline>(points);
	}
	// else if(type == Interpolator::NURBS){
		// out = make_shared<NURBS>(points);
	// }
	else if(type == Interpolator::HermiteCardinal){
		out = make_shared<HermiteCardinal>(points);
	}
	else if(type == Interpolator::HermiteFiniteDifference){
		out = make_shared<HermiteFiniteDifference>(points);
	}
	else if(type == Interpolator::HermiteFiniteDifferenceClosed){
		out = make_shared<HermiteFiniteDifferenceClosed>(points);
	}

	// out.build();
	out->name = name;
	addInterpolator(out);

	return out;
}

void removeInterpolator(shared_ptr<IInterpolator> &interpolator){
	g_interpolators.remove(interpolator);
}
std::list<shared_ptr<IInterpolator>>& getInterpolators(){
	return g_interpolators;
}
shared_ptr<IInterpolator>& getInterpolator(const std::string &name){
	for(auto &it : g_interpolators)
		if(it->name == name)
			return it;

}

shared_ptr<IInterpolator> InterpolatorFactory::build(Interpolator type, vector<glm::vec4> &points){
	shared_ptr<IInterpolator> out;
	if(type == Interpolator::Linear){
		out = make_shared<Linear>(points);
	}
	else if(type == Interpolator::BezierCurve){
		out = make_shared<BezierCurve>(points);
	}
	else if(type == Interpolator::BSpline){
		out = make_shared<BSpline>(points);
	}
	// else if(type == Interpolator::NURBS){
		// out = make_shared<NURBS>(points);
	// }
	else if(type == Interpolator::HermiteCardinal){
		out = make_shared<HermiteCardinal>(points);
	}
	else if(type == Interpolator::HermiteFiniteDifference){
		out = make_shared<HermiteFiniteDifference>(points);
	}
	else if(type == Interpolator::HermiteFiniteDifferenceClosed){
		out = make_shared<HermiteFiniteDifferenceClosed>(points);
	}

	// out.build();
	addInterpolator(out);

	return out;
}


const std::string sEmpty = "Linear";
const std::string sLinear = "Linear";
const std::string sSimple = "Simple";
const std::string sBezierCurve = "Bezier Curve";
const std::string sBSpline = "B-Spline";
const std::string sNURBS = "NURBS";
const std::string sHermiteCardinal = "Hermite Cardinal";
const std::string sHermiteFiniteDifference = "Hermite Finite Difference";
const std::string sHermiteFiniteDifferenceClosed = "Hermite Finite Difference - Closed";

const std::vector<std::pair<std::string, Interpolator>> interpolatorEnumWithName {
	{{sLinear                       }, {Interpolator::Linear}       }          ,
	{{sSimple                       }, {Interpolator::Simple}       }          ,
	{{sBezierCurve                  }, {Interpolator::BezierCurve} }           ,
	{{sBSpline                      }, {Interpolator::BSpline}    }            ,
	{{sNURBS                        }, {Interpolator::NURBS}     }             ,
	{{sHermiteCardinal              }, {Interpolator::HermiteCardinal}}        ,
	{{sHermiteFiniteDifference      }, {Interpolator::HermiteFiniteDifference}},
	{{sHermiteFiniteDifferenceClosed}, {Interpolator::HermiteFiniteDifferenceClosed}}
};

const std::string& InterpolatorTranslate(Interpolator type){
	if(type == Interpolator::Linear)
		return sLinear;
	if(type == Interpolator::Simple)
		return sSimple;
	if(type == Interpolator::BezierCurve)
		return sBezierCurve;
	if(type == Interpolator::BSpline)
		return sBSpline;
	if(type == Interpolator::NURBS)
		return sNURBS;
	if(type == Interpolator::HermiteCardinal)
		return sHermiteCardinal;
	if(type == Interpolator::HermiteFiniteDifference)
		return sHermiteFiniteDifference;
	if(type == Interpolator::HermiteFiniteDifferenceClosed)
		return sHermiteFiniteDifferenceClosed;
	return sEmpty;
}

