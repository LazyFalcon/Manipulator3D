#include "Widgets.h"

float pointSize = 16.f;

// taki sprytny handler dla punktu
class PointController {
public:
	glm::vec4 *m_point;
	PointController():m_point(nullptr){}
	PointController(glm::vec4 &point):m_point(&point){}
	// PointController(PointController &controller):m_point(controller.m_point){}
	void operator = (PointController &controller){
		m_point = controller.m_point;
	}
	glm::vec4 move(glm::vec2 delta){
		auto tmp = *m_point;
		*m_point = camera.eyePlaneIntersection(camera.surfaceNormalToCamera(*m_point), delta);

		return *m_point - tmp;
	}
	glm::vec4& operator*(){
		return *m_point;
	}


	glm::vec4 getRect(){
		glm::vec3 screenPos = glm::project((*m_point).xyz(), camera.ViewMatrix, camera.ProjectionMatrix, glm::vec4(0,0,camera.m_width, camera.m_height));
		return glm::vec4(screenPos.x-pointSize/2, screenPos.y-pointSize/2, pointSize, pointSize);
	}
	bool operator == (const PointController &controller){
		return  m_point == controller.m_point;
	}
};


class PathCreator {
	enum State {
		None = 0x0,
		KnotEditing = 0x1,
		ControlEditing = 0x2,
		Control_ProportionalDistance = 0x4,
		Control_PreserveDistance = 0x8,
		Control_ReverseDistance = 0x10,


	}state;
public:
	BSpline *spline;
	vector<PointController> controlPoints;
	vector<PointController> knots;
	PointController markedKnot;
	glm::vec2 positionOffset;
	glm::vec4 boxPosition;
	wxg::Movable movable;
	wxg::MinimizableLeft minimizable;
	wxg::DropDownList list;

	PathCreator(BSpline *spline) : spline(spline), movable(boxPosition), boxPosition(1400-200, 500,0,0), minimizable(&boxPosition, glm::vec4(window_width-25, window_height-170,0,0), "PathCreator"s, 200) {
		list = wxg::DropDownList{"CurveType"s, {"BSpline"s, "NURBS"s, "Hermite"s}, 200};
		init();
	}
	// PathCreator(const PathCreator &creator) : spline(PathCreator.spline){}

	void init(){
		for(auto &bezier : spline->beziers)
			knots.emplace_back(bezier);
	}
	void update(){
		controlPoints.clear();
		knots.clear();
		for(int i=2; i<(int)knots.size(); i+=3){
			auto &control_g = knots[glm::clamp(i, 0, (int)(knots.size()-1))];
			// ui.rect(control_g.getRect()).mouseOffset(positionOffset)
			auto r = control_g.getRect();
			ui.image(r.x, r.y, "Control").mouseOffset(positionOffset)
			.color(0x00ff00FF)
			.onlClick([this, &control_g]{
				markedKnot = control_g;
			})
			.onlPressed([this, &control_g, i]{
				if(markedKnot == control_g && i>0){
					glm::vec4 tmp = *control_g;
					float distGtoKnot = glm::distance(*knots[i+1], *control_g);
					control_g.move({0,0});
					if(i<knots.size()-2){
						glm::vec4 vecGToKnot = glm::normalize(tmp - *knots[i+1]);
						float distFtoKnot = glm::distance(*knots[i+1], *knots[i+2]);
						float distGChangeRatio = glm::distance(*knots[i+1], *control_g)/distGtoKnot;
						*knots[i+2] = *knots[i+1] - vecGToKnot*distFtoKnot*distGChangeRatio;

					}
				}
					spline->generatePath();
			})(UI::CaptureMouse);
			g_lines(*control_g, *knots[ std::min(i+2, (int)knots.size()-1) ]);
		}
		for(int i=0; i<knots.size(); i+=3){
			auto &knot_e = knots[i];
			auto r = knot_e.getRect();
			ui.image(r.x, r.y, "Knot").mouseOffset(positionOffset)
			.color(0xC71585FF)
			.onlClick([this, &knot_e]{
				markedKnot = knot_e;
			})
			.onlPressed([this, &knot_e, i]{
				if(markedKnot == knot_e){
					glm::vec4 delta = knot_e.move({0,0});
					if(i > 0)
						*knots[i-1] += delta;
					if(i < knots.size()-1)
						*knots[i+1] += delta;
					spline->generatePath();
				}
			})(UI::CaptureMouse);
		}


		for(int i=1; i<(int)knots.size(); i+=3){
			auto &control_f = knots[glm::clamp(i, 0, (int)(knots.size()-1))];
			auto r = control_f.getRect();
			ui.image(r.x, r.y, "Control").mouseOffset(positionOffset)
			.color(0x00ff00FF)
			.onlClick([this, &control_f]{
				markedKnot = control_f;
			})
			.onlPressed([this, &control_f, i]{
				if(markedKnot == control_f && i>0){
					glm::vec4 tmp = *control_f;
					float distFtoKnot = glm::distance(*knots[i-1], *control_f);
					control_f.move({0,0});
					if(i > 1){
						glm::vec4 vecFToKnot = glm::normalize(tmp - *knots[i-1]);
						float distGtoKnot = glm::distance(*knots[i-1], *knots[i-2]);
						float distFChangeRatio = glm::distance(*knots[i+1], *control_f)/distFtoKnot;
						*knots[i-2] = *knots[i-1] - vecFToKnot*distGtoKnot*distFChangeRatio;

					}
				}
					spline->generatePath();
			})(UI::CaptureMouse);
		}
		drawWidget();
	}
	void clear(){
		controlPoints.clear();
		knots.clear();
	}
	void drawWidget(){
		if(minimizable.draw())
			return;
		ui.table(boxPosition, UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::FixedPos);
			list.draw();
			ui.box(UI::LayoutVertical);


				ui.rect(150,18).text("Some random \ntext here"s).font("ui_12"s)();
			ui.endBox();
			movable.move();
		ui.endTable();

	}
};

