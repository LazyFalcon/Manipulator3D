#pragma once
#include <functional>
#include "Utils.h"
#include "Includes.h"
#include "FontRenderer.h"
#include "BaseStructs.h"
#include "UIContainer.h"


namespace UI NAM_START
typedef glm::vec4 Box;
extern int g_UILayer;
extern bool GetInput;

class IMGUI;
struct Style {
	HexColor background;
	HexColor button;
	HexColor hover;
	HexColor editBox;
	HexColor fontColor;
	HexColor imageColor;
	HexColor fancy[4];
	int padding;
	std::string font;
};
enum class StyleID : int32_t{
	Basic = 0,
	Light = 1,
	Dark = 2,
	Label = 3,
};

struct IMGUIBox {
	int m_flags{ 0 };
	Box m_box;
	Box m_currStart;
	Box m_freeRect;
	int m_border;
	int m_style;
	int m_rectIdx;
	IMGUI *imgui;

	IMGUIBox& box(int flags, Box spawnPosition, IMGUI *_imgui);
	IMGUIBox& size(float x = 0, float y = 0);
	IMGUIBox& size(const Box &box){ return size(box.z, box.w); }
	IMGUIBox& size(glm::vec2 box){ return position(box.x, box.y); }

	IMGUIBox& overridePosition(float x = 0, float y = 0); // box isn't inserted
	IMGUIBox& position(float x = 0, float y = 0);
	IMGUIBox& position(const Box &box){ return position(box.x, box.y); } // box isn't inserted
	IMGUIBox& position(glm::vec2 box){ return position(box.x, box.y); } // box isn't inserted

	IMGUIBox& offset(float x = 0, float y = 0); // makes box inserted
	IMGUIBox& offset(const Box &box){ return offset(box.x, box.y); }
	IMGUIBox& offset(glm::vec2 box){ return offset(box.x, box.y); }

	IMGUIBox& style(StyleID s = StyleID::Basic);
	IMGUIBox& border(int b = 0);
	IMGUIBox& operator()();


	// "derived" from imgui
	Box getSpawnPoint(const Box &r);
	Box insertRect(const Box &r);
	Box placeRect(const Box &r);

	IMGUIBox& onGroupHover(std::function<void(Box rect)>fun);
	IMGUIBox& onGrouplClick(std::function<void(Box rect)>fun);
	bool onGroupGrab(std::function<void(Box rect)>fun);
};

struct FigureInfo {
	Box r;
	GLuint textureID;
	GLuint shader;
	std::vector <std::pair<std::string, glm::mat4>> m_matrices;
	std::vector <std::pair<std::string, Box>> m_vec4;
	std::vector <std::pair<std::string, glm::vec3>> m_vec3;
	std::vector <std::pair<std::string, glm::vec2>> m_vec2;
	std::vector <std::pair<std::string, float>> m_float;
};

enum GroupFlags {
	LayoutVertical = 0x01,
	LayoutHorizontal = 0x02,

	AlignLeft = 0x04,
	AlignRight = 0x08,

	AlignTop = 0x10,
	AlignBottom = 0x20,
	Centered = 0x40,

	Draw = 0x100,
	Background = 0x200,

	HotFix1 = 0x400,
	CenterText = 0x1000,
	TextToLeft = 0x2000,
	TextToRight = 0x4000,
	FixedSize = 0x10000,
	FixedPos = 0x20000,
	FixedPos2 = 0x40000,
	NewLayer = 0x80000,
	RelativePosition = 0x100000,
	AbsolutePosition = 0x200000,
	NoInsertion = 0x400000,

	HorizonBottom = LayoutHorizontal | AlignLeft | AlignBottom,
	HorizonTop = LayoutHorizontal | AlignLeft | AlignTop,
	VertBottom = LayoutVertical | AlignLeft | AlignBottom,
	VertTop = LayoutVertical | AlignLeft | AlignTop,

	ClearLayout = LayoutHorizontal | LayoutVertical | Draw | FixedSize | FixedPos,
};

enum DrawFlags : int {
	None = 0,
	Label = 0x1,
	Hover = 0x2,
	Editable = 0x4,
	CaptureMouse = 0x8,
	Image = 0x10,
	BigBox = 0x20,
	ClearHover = ~Hover,

	Hoverable = CaptureMouse | Hover,
	EditBox = Editable | CaptureMouse,
	Button = Label | Hover | CaptureMouse,

};

enum class TypeInfo : int32_t{
	EMPTY, INT32_t, UINT32_t, INT64_t, UINT64_t, STRING, FLOAT, DOUBLE
};
class TextEditor {
	enum TextEditorFlags {
		EMPTY = 0, STRING = 1, FLOAT = 6, INT = 10, HEX = 16, BINARY = 2, SYMBOLIC = 6,
	};
public:
	TypeInfo m_typeInfo;
	TextEditor();
	template<typename T>
	void setValueToEdit(T &value, int base);
	template<typename T>
	void setValueToEdit(T &value);

	float parseExpr(string &str);

	template<typename T>
	bool compare(T &value);

	std::string value();
	bool state();
	void finishEdition();
	void breakEdition();
	int caretPosition();
	void input(u32 key, u32 action, u32 mod);
	const char* getClipboard(){
		return glfwGetClipboardString(window);
	}

	void *valuePointer;
	std::string currentString;
	GLFWwindow *window;
private:
	int m_base;
	bool m_state;
	int m_caretPosition;
};

/// ---------------------------- IMGUI
class IMGUI {
public:
	void printTextEditorValue();
	template<typename T>
	bool processTE(T &value, int base);
	template<typename T>
	bool processTE(T &value);

	IMGUIBox& box(int flags);
	IMGUIBox& endBox();

	IMGUIBox& table(int flags);
	IMGUIBox& table(Box position, int flags);
	IMGUIBox&  endTable();

	IMGUIBox& parentBox(){
		return m_boxStack[m_boxIndex - 1];
	}
	IMGUIBox& currentBox(){
		return m_boxStack[m_boxIndex];
	}

	IMGUI& circleShape(const Box &circle);
	IMGUI& rotatedBoxShape(Box &startEnd, float distance, float &proportion);
	IMGUI& customShape(std::function<bool(glm::vec2)> &fun);
	IMGUI& customShape(bool hovered);

	IMGUI& image(const std::string &name);
	IMGUI& image(int x, int y, const std::string &name);
	IMGUI& image(glm::vec2 v, const std::string &name);
	IMGUI& rect();
	IMGUI& rect(Box r);
	IMGUI& rect(HexColor r);
	IMGUI& rect(int x, int y, int w, int h);
	IMGUI& rect(int w, int h);
	IMGUI& rect(int w, int h, HexColor color);
	IMGUI& rect(Box r, HexColor color);
	IMGUI& rect(int x, int y, int w, int h, HexColor color);

	IMGUI& setBox(const Box &box);
	// for proportional editing
	IMGUI& size(float x = 0, float y = 0);
	IMGUI& offset(float x = 0, float y = 0);

	IMGUI& switcher(int &state, int val);
	IMGUI& switcher(bool &state, bool val);
	IMGUI& switcher(std::string &state, const std::string &val);
	template<typename T>
	IMGUI& switcher(T &state, const T &val){
		if(this->m_lClicked)
			state = val;
		this->m_active = (state == val);
		return *this;
	}
	template<typename T>
	IMGUI& radio(T &currentValue, const T &value){
		if(this->m_lClicked){
			if(currentValue == value) currentValue = T(0);
			else currentValue = value;
		}
		this->m_active = (currentValue == value);
		return *this;
	}
	IMGUI& activeElement(const std::string &image);
	IMGUI& button(bool &state);
	IMGUI& onlClick(std::function<void(void)>fun);
	IMGUI& onlClick(std::function<void(Box rect)>fun);
	IMGUI& onrClick(std::function<void(Box rect)>fun);
	IMGUI& onrClick(std::function<void(void)>fun);

	IMGUI& onlPressed(std::function<void(void)>fun);
	IMGUI& onlPressed(std::function<void(void)>fun, uint32_t freq);
	IMGUI& onlPressed(std::function<void(Box)>fun);
	IMGUI& onrPressed(std::function<void(void)>fun);

	IMGUI& onHover(std::function<void(void)>fun);
	IMGUI& onSpecial(std::function<void(void)>fun);
	IMGUI& onHover(std::function<void(Box rect)>fun);
	IMGUI& onEverytime(std::function<void(Box rect)>fun);
	IMGUI& onActive(std::function<void(void)>fun);
	IMGUI& onActive(std::function<void(Box rect)>fun);
	IMGUI& onEdition(std::function<void(void)>fun);

	IMGUI& text(const std::string &text, const std::string &font, int flag = 0x2000, int caretPosition = -1);
	IMGUI& text(const std::u16string &text, const std::string &font, int flag = 0x2000, int caretPosition = -1);
	IMGUI& text(const std::u16string &text, int flag = 0x2000, int caretPosition = -1);
	IMGUI& text(const std::string &text, int flag = 0x2000, int caretPosition = -1);

	IMGUI& color(HexColor hexColor);
	IMGUI& font(const std::string &font);
	IMGUI& getRect(Box &r);
	IMGUI& mouseOffset(Box &out);
	IMGUI& mouseOffset(glm::vec2 &out);

	template<typename T>
	IMGUI& edit(T &value, int base = 10);
	IMGUI& edit(float &value, int flag = 0);
	IMGUI& edit(double &value, int flag = 0);
	IMGUI& edit(std::string &value, int flag = 0);
	IMGUI& edit(std::string &value, std::function<void(void)>callOnEnter, int flag = 0);

	IMGUI& setFlag(int flag);
	IMGUI& slider(float &value, float min = 0.f, float max = 1.f);
	IMGUI& slider(int64_t &value, int64_t min, int64_t max);
	IMGUI& operator () (int flags = UI::None);

	void restoreDefaults();
	TextEditor textEditor;
	float m_maxHorizontal;
	float m_maxVertical;
	void begin();
	void end();

	void mouseKeyInput(int key, int action){
		if (action == GLFW_PRESS){
			m_mouseKey = key;
			m_mouseAction = action;
		}
	}
	void keyInput(int key, int action, int mod){
		if(action == GLFW_PRESS){
			m_key = key;
			m_action = action;
			m_mod = mod;
		}
		if(textEditor.state())
			textEditor.input(key, action, mod);
	}
	bool keyAction(int key, bool *state){
		if (key == m_key && currentLayer >= drawLayer){
			*state = !*state;
			m_key = -1;
			return true;
		}
		return false;
	}
	bool keyAction(int key, std::function<void(void)>fun){
		if (key == m_key && currentLayer >= drawLayer){
			fun();
			m_key = -1;
			return true;
		}
		return false;
	}
	bool mouseKeyAction(int key, bool *state){
		if (key == m_mouseKey && currentLayer >= drawLayer){
			*state = !*state;
			m_mouseKey = -1;
			return true;
		}
		return false;
	}

	bool onRightClick(bool *state){
		return mouseKeyAction(GLFW_MOUSE_BUTTON_RIGHT, state);
	}
	bool onEnter(bool *state){
		return keyAction(GLFW_KEY_ENTER, state);
	}
	bool onESC(bool *state){
		return keyAction(GLFW_KEY_ESCAPE, state);
	}
	bool onESC(std::function<void(void)>fun){
		return keyAction(GLFW_KEY_ESCAPE, fun);
	}
	bool onEnter(std::function<void(void)>fun){
		return keyAction(GLFW_KEY_ENTER, fun);
	}

	bool tableHover(){
		return hasHover(fixRect(m_boxStack[m_boxIndex].m_box));
	}
	bool outOfTable();

	void onGroupHover(std::function<void(Box rect)>fun){
		if (hasHover(fixRect(m_boxStack[m_boxIndex].m_box)))
			fun(fixRect(m_boxStack[m_boxIndex].m_box));

	}
	void onGrouplClick(std::function<void(Box rect)>fun){
		if (hasHover(fixRect(m_boxStack[m_boxIndex].m_box))
			&& m_lClick)
			fun(fixRect(m_boxStack[m_boxIndex].m_box));
	}
	bool onGroupGrab(std::function<void(Box rect)>fun){
		if (hasHover(fixRect(m_boxStack[m_boxIndex].m_box)) && mouseLPressed){
			fun(fixRect(m_boxStack[m_boxIndex].m_box));
			return true;
		}
		return false;
	}
	bool click(){
		if (mouselClick){
			mouselClick = false;
			return true;
		}
		return false;
	}


	void setFont(const std::string &s){
		m_font = s;
	}
	void setfont(const std::string &s, int size){
		m_font = s;
		m_fontSize = size;
	}

	void forceClick(){
		m_force = true;
	}

	void switchDrawing(){
		draw = !draw;
	}

	void addImage(Icon &ico){
		m_images.push_back(ico);
	}

	void addRectToDraw(Box rect){
		if (currentLayer >= drawLayer && draw)
			m_rects.push_back(rect);
	}
	Box getBox(){
		return m_rects.back();
	}

	void drawOnlyTopLayer(){
		layerToDraw = std::max(currentLayer, layerToDraw);
	}
	void beginLayer(){
		currentLayer++;
		g_UILayer++;
		tmpMaxLayer = std::max(tmpMaxLayer, currentLayer);
	}
	void endLayer(){
		currentLayer--;
		g_UILayer--;
	}

	void border(int borderSize){
		m_boxStack[m_boxIndex].m_border = borderSize;
	}
	void indentation(int indentationLen){
		m_indentation = indentationLen;
	}

	void setlClick(bool click);
	void setrClick(bool click);
	void setMouseLPressed(){
		mouseLPressed = true;
	}
	void setMouseRPressed(){
		mouseRPressed = true;
	}
	void setMouse(float x, float y);

	Box fixRect(Box rect);
	bool captureMouse;
	bool mouseOverButton;
	// Box getSpawnPoint(const Box &r);
	// Box insertRect(const Box &r);
	bool hasHover(Box rect);
	Box placeRect(const Box &r);
	bool findCollision(Box &r, Box &out);
	Box placeGroup(int flags);
	bool overlay(Box &rect, Box &point);

	void updateCounter(float deltaTime){
	}
	void updateCounter(uint32_t deltaTime){
		accu += deltaTime;
		accu2 += deltaTime;
		timeFromstart += deltaTime;
	}

	void style(StyleID id){
		m_style = (int)id;
		m_boxStack[m_boxIndex].m_style = (int)id;
	}
	Style& getStyle(){
		return m_styles[m_style];
	}

	void addBox(Box &r){
		m_specRects[currentLayer].push_back(r);
		m_specRectsColor[currentLayer].push_back(0xFFFFFFFF);
	}
	void addBox(Box &r, HexColor color){
		m_specRects[currentLayer].push_back(r);
		m_specRectsColor[currentLayer].push_back(color);
	}

	std::vector <Box> m_rects;
	std::vector <std::pair<Box, std::string>> m_shapePoint;
	std::vector <Box> m_collRects;
	std::vector <HexColor> m_specRectsColor[5];
	std::vector <Box> m_specRects[5];
	std::vector <FigureInfo> m_figures;
	std::vector <Icon> m_images;
	bool lRepeat(){
		return mouseLPressed;
	}
	bool rRepeat(){
		return mouseRPressed;
	}
	bool lClicked(){
		return m_lClick;
	}
	bool lClick(){
		return m_lClick;
	}
	bool rClick(){
		return m_rClick;
	}
	u32 accu;
	u32 accu2;
	float frequency;
	ImageSet *m_imageSet;
	void setDefaultFont(std::string font, int size){
		m_font = font;
		m_defaultFont = font;
		m_defaultFontSize = size;
	}

	bool hover(){
		return m_hover;
	}

	HexColor m_defaultColor{ 0xb0b0ffff };
	HexColor m_defaultFontColor{ 0xffffffff };
	Box m_defaultRect{ 0, 0, 150, 22 };
	std::string m_defaultFont{ "impact12" };
	int m_defaultFontSize{ 12 };
	std::vector<Style> m_styles;
	Box m_box;
	int m_style;
	bool m_hover;
	bool m_lClick;
	bool m_rClick;
	bool m_lClicked;
	bool m_rClicked;
	bool m_imageEnbl;
	bool m_underEdition;
	bool m_active;
	bool m_special;
	bool m_editBox;
	bool m_forceNoDraw;
	bool m_forceColor;
	int m_flag;
	int m_buttonFlags;
	Icon m_image;
	std::string m_text;
	int m_caretPosition;
	HexColor m_color;
	HexColor m_fontColor;
	float m_textOffset{ 0 };

	float m_indentation{ 0 };
	float mc_indentation{ 0 };

	int m_fontSize{ 12 };
	std::string m_font{ "ui_12" };
	glm::vec2 mousePos;
	bool mouseLPressed;
	bool mouseRPressed;
	bool mouseLRepeat;
	bool mouseRRepeat;
	bool mouselClick;
	bool draw;
	glm::vec2 focusPoint;
	IMGUIBox m_group;
	IMGUIBox m_boxStack[20];
	int m_groupIndex;
	int m_boxIndex;

	bool m_force;
	bool counter;
	bool MS500OSCILATOR;
	bool MS50IMPULSE;
	bool MS100IMPULSE;
	uint32_t timeFromstart {0};


	int currentLayer;
	int tmpMaxLayer;
	int maxLayer;
	int drawLayer;
	int layerToDraw;
	int m_key, m_action, m_mod;
	int m_mouseKey, m_mouseAction;
	IUIContainer *m_UIContainer;
private:
	inline bool force(){
		if (m_force){
			m_force = false;
			return true;
		}
		return false;
	}

public:
	Box m_lastBox;
};

template<typename T>
TypeInfo recognizeType(){
	return TypeInfo::INT32_t;
}

template<typename T>
std::string prettyPrint(T &value, int base);

NAM_END
extern UI::IMGUI ui;
#define vertical(x) ui.box(UI::LayoutVertical); \
												x\
										ui.endBox();
#define horizontal(x) ui.box(UI::LayoutHorizontal); \
												x\
										ui.endBox();
