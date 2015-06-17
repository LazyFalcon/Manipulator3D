#pragma once
#include <Utils/Includes.h>
#include <Utils/Utils.h>
#include <Utils/Camera.h>
#include "common.h"
#include "Engine.h"

#define NAM_START {
#define NAM_END }


extern HexColor colorTransparent;
extern HexColor colorWhite;
extern HexColor colorWhite80;
extern HexColor colorBlack;
extern HexColor colorWhite_4;
extern HexColor colorGreen;
extern HexColor colorRed;
extern HexColor colorYellow;
extern HexColor colorBlue;
extern HexColor colorOrange;
extern HexColor colorRedDarker;
extern HexColor GolorUIcolor;
extern HexColor GolorUIcolor2;
extern HexColor GUIbcgColor;
extern HexColor colorClearColor;
extern HexColor GUIcolor;
extern HexColor GUIcolor2;
extern HexColor clearColor;
extern HexColor GUIColors[10];
extern HexColor cRed;
extern HexColor cGreen;
extern HexColor cBlue;

extern PositionSaver g_mousePositions;
extern PositionSaver g_robotPositions;
extern PositionSaver g_robotDebugPositions;
extern LineBuffer		 g_lines;

namespace Engine NAM_START

struct LineStrip {
    vector<glm::vec4> points;
    HexColor color;
    int maxCount;
    int count = 0;
    int lineSize = 1;

    LineStrip(int n, HexColor c) : points(n), color(c), maxCount(n){}
    void operator()(const glm::vec4 &p, HexColor color = 0xffffffff){
        points[count] = p;

        count = (count+1)%maxCount;
    }
    void clear(){
        count = 0;
    }
};
struct LineSegments {
    std::vector<glm::vec4> points;
    std::vector<HexColor> colors;
    int maxCount;
    int count = 0;
    int lineSize = 1;

    LineSegments(int n) : points(n), colors(n), maxCount(n){}
    void operator()(const glm::vec4 &p1, const glm::vec4 &p2, HexColor color = 0xffffffff){
        points[count] = p1;
        points[count+1] = p2;
        colors[count] = color;

        count = (count+1)%maxCount;
    }
    void clear(){
        count = 0;
    }
};

extern LineSegments segments;
extern LineStrip strips;

/// For batching things to draw
void drawPattern2D(glm::vec2 position, glm::vec2 info, string name, HexColor color);
void drawShape2D(glm::vec4 info, string shape, HexColor color);
void drawMask2D(glm::vec4 info, string tex, HexColor color);
void drawPoint2D(glm::vec4 info, string shape, HexColor color);
void drawFilledShape2D(glm::vec4 info, string shape, HexColor color);
void drawText2D(glm::vec2 info, string text, string font, HexColor color);
void drawText2D(glm::vec2 info, string text, UI::FontRenderer &fontManager, HexColor color);
void drawStrings(vector <UI::TextInfo> m_strings);

void drawTexturedBox(GLuint texture, const glm::vec4 &box);

/// Debug drawing, push to namespace
void drawArrow(glm::vec4 vec, HexColor color, std::function<void(float)> callback);
void drawRotArrow(glm::vec4 vec, HexColor color, std::function<void(float)> callback);
void drawJoint();
void drawBBox();

/// utils
glm::vec4 perpendicular(glm::vec4 v);
glm::mat4 perpendicularTransform(glm::vec4 v, glm::vec4 p);

/// Drawing things
void drawASDFASDF(glm::vec4 point);
void drawLineStrip(std::vector<glm::vec4> &points, HexColor color, int size = 2);
void drawLines(std::vector<glm::vec4> &points, HexColor color, int size = 2);
void drawPoints(std::vector<glm::vec4> &points, HexColor color, float size = 5);

void drawLineStrip(const LineStrip &strip, bool clear);
void drawLineSegments(const LineSegments &segments, bool clear);
void drawAABB(const glm::vec4 &center, const glm::vec4 &dimensions, HexColor color, int lineSize);

NAM_END

