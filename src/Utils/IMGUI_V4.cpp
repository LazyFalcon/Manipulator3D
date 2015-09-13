#include "IMGUI_V4.h"

extern const float pi;

namespace UI NAM_START
extern bool GetInput;
extern int g_UILayer;

/**
 * Box.xy: position
 * Box.z: circle radius
*/
IMGUI& IMGUI::circleShape(const Box &circle){

	this->m_box = Box(circle.x, circle.y, 20, 20);
	this->m_hover = (glm::distance2(circle.xy(), mousePos) < circle.z*circle.z) && !captureMouse;
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	return *this;
}

/**
 * Box.xy: start position
 * Box.zw: end position
 *
*/
IMGUI& IMGUI::rotatedBoxShape(Box &startEnd, float distance, float &proportion){
	bool hovered = false;
	float lengthSquared = glm::distance2(startEnd.xy(), startEnd.zw());

	float t = glm::dot(mousePos-startEnd.xy(), startEnd.zw() - startEnd.xy())/lengthSquared;
	proportion = t;

	if(t < 0.f)
		hovered = glm::distance2(mousePos, startEnd.xy()) < distance*distance;
	else if(t > 1.f)
		hovered = glm::distance2(mousePos, startEnd.xy()) < distance*distance;
	else {
		glm::vec2 projection = startEnd.xy() + t*(startEnd.zw() - startEnd.xy());
		hovered = glm::distance2(mousePos, projection) < distance*distance;
	}

	this->m_box = Box((startEnd.x + startEnd.z)/2, (startEnd.y + startEnd.w)/2, 20, 20);
	this->m_lClicked = hovered && this->m_lClick;
	this->m_rClicked = hovered && this->m_rClick;

	return *this;
}
IMGUI& IMGUI::customShape(std::function<bool(glm::vec2)> &fun){
	bool hovered = fun(mousePos);

	this->m_box = Box(0,0,0,0);
	this->m_lClicked = hovered && this->m_lClick;
	this->m_rClicked = hovered && this->m_rClick;

	return *this;
}
IMGUI& IMGUI::customShape(bool hovered){
	this->m_box = Box(0,0,0,0);
	this->m_lClicked = hovered && this->m_lClick;
	this->m_rClicked = hovered && this->m_rClick;

	return *this;
}
IMGUI& IMGUI::image(const std::string &name){
	this->m_image = m_imageSet->set[name];
	this->m_box = currentBox().getSpawnPoint(this->m_image.rect);
	this->m_image.rect = this->m_box;
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;
	this->m_imageEnbl = true;
	return *this;
}
IMGUI& IMGUI::image(int x, int y, const std::string &name){
	this->m_image = m_imageSet->set[name];
	this->m_image.rect[0] = x;
	this->m_image.rect[1] = y;
	this->m_box = this->m_image.rect;

	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;
	this->m_imageEnbl = true;
	this->m_buttonFlags |= NoInsertion;
	return *this;
}
IMGUI& IMGUI::image(glm::vec2 v, const std::string &name){
	this->m_image = m_imageSet->set[name];
	this->m_image.rect[0] = v.x;
	this->m_image.rect[1] = v.y;
	this->m_box = this->m_image.rect;

	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;
	this->m_imageEnbl = true;
	this->m_buttonFlags |= NoInsertion;
	return *this;
}
IMGUI& IMGUI::rect(){

	this->m_box = currentBox().getSpawnPoint(this->m_box);
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	return *this;
}
IMGUI& IMGUI::rect(Box r){

	this->m_box = r;
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	this->m_buttonFlags |= NoInsertion;
	return *this;
}
IMGUI& IMGUI::rect(int x, int y, int w, int h){

	this->m_box = Box(x,y,w,h);
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;
	this->m_buttonFlags |= NoInsertion;
	return *this;
}
IMGUI& IMGUI::rect(int w, int h){

	this->m_box = currentBox().getSpawnPoint(Box(0,0,w,h));
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;
	return *this;
}
IMGUI& IMGUI::rect(int w, int h, HexColor color){
	this->m_box = currentBox().getSpawnPoint(Box(0,0,w,h));
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	m_UIContainer->push(Label, color, this->m_box, g_UILayer);

	this->m_forceNoDraw = true;

	return *this;
}
IMGUI& IMGUI::rect(HexColor color){
	this->m_box = currentBox().getSpawnPoint(Box(0,0,20,20));
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	m_UIContainer->push(Label, color, this->m_box, g_UILayer);

	this->m_forceNoDraw = true;

	return *this;
}
IMGUI& IMGUI::rect(Box r, HexColor color){

	this->m_box = r;
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	m_UIContainer->push(Label, color, this->m_box, g_UILayer);

	this->m_forceNoDraw = true;
	this->m_buttonFlags |= NoInsertion;

	return *this;
}
IMGUI& IMGUI::rect(int x, int y, int w, int h, HexColor color){

	this->m_box = Box(x,y,w,h);
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	m_UIContainer->push(Label, color, this->m_box, g_UILayer);

	this->m_buttonFlags |= NoInsertion;
	this->m_forceNoDraw = true;

	return *this;
}

IMGUI& IMGUI::setBox(const Box &box){
	this->m_box = box;
	return *this;
}
IMGUI& IMGUI::size(float x, float y){
	auto &parentBox = currentBox().m_box;
	if(x>-1 && x<1){
		x = floor(abs(parentBox.z * x));
	}
	if(y>-1 && y<1){
		y = floor(abs(parentBox.w * y));
	}
	this->m_box.z = x;
	this->m_box.w = y;

	return *this;
}
IMGUI& IMGUI::offset(float x, float y){
	float u(x),v(y);
	auto &parentBox = currentBox().m_box;
	if(x>-1 && x<1){
		u += parentBox.z*x;
	}
	if(y>-1 && y<1){
		v += parentBox.w*y;
	}
	if(x<0){
		u += parentBox.z;
	}
	if(y<0){
		v += parentBox.w;
	}

	this->m_box.x += floor(u);
	this->m_box.y += floor(v);

	// oh sheisse..
	this->m_hover = hasHover(this->m_box);
	this->m_lClicked = this->m_hover && this->m_lClick;
	this->m_rClicked = this->m_hover && this->m_rClick;

	return *this;
}

IMGUI& IMGUI::switcher(int &state, int val){
	if(this->m_lClicked)
		state = val;
	this->m_active = (state == val);
	return *this;
}
IMGUI& IMGUI::switcher(bool &state, bool val){
	if(this->m_lClicked)
		state = val;
	this->m_active = (state == val);
	return *this;
}
IMGUI& IMGUI::switcher(std::string &state, const std::string &val){
	if(this->m_lClicked)
		state = val;
	this->m_active = (state  == val);

	return *this;
}
IMGUI& IMGUI::activeElement(const std::string &image){
	if(this->m_active){
		this->m_image.uvs = m_imageSet->set[image].uvs;
	}
	return *this;
}
IMGUI& IMGUI::button(bool &state){
	if(this->m_lClicked)
		state = !state;;
	this->m_active = state;
	return *this;
}

IMGUI& IMGUI::onlClick(std::function<void(void)>fun){
	if(this->m_lClicked)
		fun();
	return *this;
}
IMGUI& IMGUI::onlClick(std::function<void(Box r)>fun){
	if(this->m_lClicked)
		fun(this->m_lastBox);
	return *this;
}
IMGUI& IMGUI::onrClick(std::function<void(Box r)>fun){
	if(this->m_lClicked)
		fun(this->m_lastBox);
	return *this;
}
IMGUI& IMGUI::onrClick(std::function<void(void)>fun){
	if(this->m_rClicked)
		fun();
	return *this;
}

IMGUI& IMGUI::onlPressed(std::function<void(void)>fun){
	if(this->m_hover && this->mouseLPressed)
		fun();
	return *this;
}
IMGUI& IMGUI::onlPressed(std::function<void(void)>fun, uint32_t freq){
	if(this->m_hover && this->mouseLPressed && (timeFromstart%freq == 0))
		fun();
	return *this;
}
IMGUI& IMGUI::onlPressed(std::function<void(Box)>fun){
	if(this->m_hover && this->mouseLPressed)
		fun(this->m_lastBox);
	return *this;
}
IMGUI& IMGUI::onrPressed(std::function<void(void)>fun){
	if(this->m_hover && this->mouseRPressed)
		fun();
	return *this;
}
// IMGUI& IMGUI::onrRepeat(std::function<void(void)>fun){
	// if(this->m_hover && this->mouseRRepeat)
		// fun();
	// return *this;
// }
// IMGUI& IMGUI::onlRepeat(std::function<void(void)>fun){
	// if(this->m_hover && this->mouseLRepeat)
		// fun();
	// return *this;
// }



IMGUI& IMGUI::onHover(std::function<void(void)>fun){
	if(this->m_hover)
		fun();
	return *this;
}
IMGUI& IMGUI::onHover(std::function<void(Box rect)>fun){
	if(this->m_hover)
		fun(this->m_lastBox);
	return *this;
}

IMGUI& IMGUI::onSpecial(std::function<void(void)>fun){
	if(this->m_special)
		fun();
	return *this;
}
IMGUI& IMGUI::onEverytime(std::function<void(Box rect)>fun){
	fun(this->m_lastBox);
	return *this;
}
IMGUI& IMGUI::onActive(std::function<void(void)>fun){
	if(this->m_active)
		fun();
	return *this;
}
IMGUI& IMGUI::onActive(std::function<void(Box rect)>fun){
	if(this->m_active)
		fun(this->m_lastBox);
	return *this;
}
IMGUI& IMGUI::onEdition(std::function<void(void)>fun){
	if(this->m_underEdition)
		fun();
	return *this;
}

IMGUI& IMGUI::text(const std::string &text, const std::string &font, int flag, int caretPosition){
	this->m_flag |= flag;
	this->m_text = text;
	this->m_caretPosition = caretPosition;
	m_textOffset += fonts[font].push(m_text, this->m_box.xy() + glm::vec2(m_textOffset+3, floor(this->m_box.w/2.f - fonts[font].generator->m_height/2)), m_fontColor, this->m_caretPosition);

	if(this->m_flag & CenterText){
		fonts[font].generator->move( (this->m_box.z - m_textOffset)/2-3, 0);
	}
	else if(this->m_flag & TextToRight){
		fonts[font].generator->move( (this->m_box.z - m_textOffset)-3, 0);
	}
	if(LastTextHeight > 0)
		fonts[font].generator->move( 0, LastTextHeight/2);

	return *this;
}
IMGUI& IMGUI::text(const std::string &text, int flag, int caretPosition){
	this->m_flag |= flag;
	this->m_text = text;
	this->m_caretPosition = caretPosition;
	m_textOffset += fonts[m_font].push(m_text, this->m_box.xy() + glm::vec2(m_textOffset+3, floor(this->m_box.w/2.f - fonts[m_font].generator->m_height/2.f)), m_fontColor, this->m_caretPosition);

	if(this->m_flag & CenterText){
		fonts[m_font].generator->move( (this->m_box.z - m_textOffset)/2-3, 0);
	}
	else if(this->m_flag & TextToRight){
		fonts[m_font].generator->move( (this->m_box.z - m_textOffset)-5, 0);
	}
	return *this;
}
IMGUI& IMGUI::text(const std::u16string &text, const std::string &font, int flag, int caretPosition){
	this->m_flag |= flag;
	this->m_caretPosition = caretPosition;
	m_textOffset += fonts[font].push(text, this->m_box.xy() + glm::vec2(m_textOffset+3, floor(this->m_box.w/2.f - fonts[font].generator->m_height/2)), m_fontColor, this->m_caretPosition);

	if(this->m_flag & CenterText){
		fonts[font].generator->move( (this->m_box.z - m_textOffset)/2-3, 0);
	}
	else if(this->m_flag & TextToRight){
		fonts[font].generator->move( (this->m_box.z - m_textOffset)-3, 0);
	}
	return *this;
}
IMGUI& IMGUI::text(const std::u16string &text, int flag, int caretPosition){
	this->m_flag |= flag;
	this->m_caretPosition = caretPosition;
	m_textOffset += fonts[m_font].push(text, this->m_box.xy() + glm::vec2(m_textOffset+3, floor(this->m_box.w/2.f - fonts[m_font].generator->m_height/2)), m_fontColor, this->m_caretPosition);

	if(this->m_flag & CenterText){
		fonts[m_font].generator->move( (this->m_box.z - m_textOffset)/2-3, 0);
	}
	else if(this->m_flag & TextToRight){
		fonts[m_font].generator->move( (this->m_box.z - m_textOffset)-3, 0);
	}
	return *this;
}

IMGUI& IMGUI::color(HexColor hexColor){
	this->m_color = hexColor;
	this->m_fontColor = hexColor;
	this->m_forceColor = true;
	return *this;
}
IMGUI& IMGUI::font(const std::string &font){
	this->m_font = font;
	return *this;
}
IMGUI& IMGUI::getRect(Box &r){
	r = this->m_box;
	return *this;
}

IMGUI& IMGUI::mouseOffset(Box &out){
	if(this->m_hover){
		out = this->m_box - Box(focusPoint, 0,0);
		out.z = 0;
		out.w = 0;
	}
	return *this;
}
IMGUI& IMGUI::mouseOffset(glm::vec2 &out){
	if(this->m_hover){
		out = this->m_box.xy() - focusPoint;
	}
	return *this;
}
template<typename T>
std::string prettyPrint(T &value, int base){
	if(base == 10)
		return to_string(value);
	std::stringstream stream;

	if(base == 2)
		return "error";
	else if(base == 16)
		stream <<"0x"<< std::setfill ('0')<< std::setw(sizeof(T)*2)<< std::hex << value;
	return stream.str();
}

template<typename T>
IMGUI& IMGUI::edit(T &value, int base){
	// magoc :P
	this->m_editBox = true;
	this->m_underEdition = textEditor.compare(value);

	if(this->m_underEdition)
		printTextEditorValue();
	else
		text(std::to_string(value));

	processTE(value, base);

	return *this;

}
IMGUI& IMGUI::edit(double &value, int flag){
	// magoc :P
	this->m_editBox = true;
	this->m_underEdition = textEditor.compare(value);

	if(this->m_underEdition)
		printTextEditorValue();
	else
		text(std::to_string(value), flag);

	processTE(value);

	return *this;

}
IMGUI& IMGUI::edit(float &value, int flag){
	// magoc :P
	this->m_editBox = true;
	this->m_underEdition = textEditor.compare(value);

	if(this->m_underEdition)
		printTextEditorValue();
	else
		text(std::to_string(value), flag);

	processTE(value);

	return *this;

}
IMGUI& IMGUI::edit(std::string &value, int flag){
	// magic :P
	this->m_editBox = true;
	this->m_underEdition = textEditor.compare(value);

	if(this->m_underEdition)
		printTextEditorValue();
	else
		text(value, flag);

	processTE(value);

	return *this;

}
IMGUI& IMGUI::edit(std::string &value, std::function<void(void)>callOnEnter, int flag){
	this->m_editBox = true;
	this->m_underEdition = textEditor.compare(value);

	if(this->m_underEdition)
		printTextEditorValue();
	else
		text(value, flag);

	if(processTE(value))
				callOnEnter();

	return *this;
}

IMGUI& IMGUI::setFlag(int flag){
	this->m_flag |= flag;
	return *this;
}
IMGUI& IMGUI::slider(float &value, float min, float max){
	auto r = this->m_box;

	float x = r.x+3.f;
	float y = r.y+r.w/2.f - 3.f;

	float slideLen = r.z - 6.f;
	Box slide = Box(x, y, slideLen, 6.f);

	float sliderW = r.w - 4.f;
	Box slider_ = Box(
			x+slideLen*(value - min)/(max - min)-3.f
			,r.y+2.f
			,6.f
			,sliderW
		);

	if(this->m_hover && (m_lClick || mouseLPressed)){
		value = glm::clamp((mousePos.x - slide.x)/slide.z, 0.f, 1.f)*(max-min)+min;
		slide += Box(-1,-1,2,2);
	}


	m_UIContainer->push(UI::Hover, m_styles[m_style].fancy[0], slide, g_UILayer);
	m_UIContainer->push(UI::Hover, m_styles[m_style].fancy[1], slider_, g_UILayer);
	return *this;
}
IMGUI& IMGUI::slider(int64_t &value, int64_t min, int64_t max){
	auto r = this->m_box;

	float x = r.x+3.f;
	float y = r.y+r.w/2.f - 3.f;

	float slideLen = r.z - 6.f;
	Box slide = Box(x, y, slideLen, 6.f);

	float sliderW = r.w - 4.f;
	Box slider_ = Box(
			x+slideLen*(value - min)/(max - min)-3.f
			,r.y+2.f
			,6.f
			,sliderW
		);

	if(this->m_hover && (m_lClick || mouseLPressed)){
		value = glm::clamp((mousePos.x - slide.x)/slide.z, 0.f, 1.f)*(max-min)+min;
		slide += Box(-1,-1,2,2);
	}


	m_UIContainer->push(UI::Hover, m_styles[m_style].fancy[0], slide, g_UILayer);
	m_UIContainer->push(UI::Hover, m_styles[m_style].fancy[1], slider_, g_UILayer);
	return *this;
}
IMGUI& IMGUI::operator () (int flags){
	if((m_buttonFlags & NoInsertion) != NoInsertion)
		currentBox().insertRect(this->m_box);

	if(this->m_lClicked || this->m_rClicked)
		this->m_box += Box(-2,-2,4,4);

		if(!this->m_hover && (m_buttonFlags & NoInsertion) != NoInsertion) flags &= UI::ClearHover;
				m_UIContainer->push(flags, m_styles[m_style], this->m_box, g_UILayer);

	if(this->m_imageEnbl){
		this->m_image.color = this->m_forceColor ? m_color : m_styles[m_style].imageColor;
		m_UIContainer->push(UI::Image, this->m_image);
	}
	if(flags & UI::CaptureMouse && this->m_hover){
		if(this->m_lClicked || this->m_rClicked){
			this->m_lClick = false;
			this->m_rClick = false;
			this->focusPoint = glm::vec2(-500, -500);
			this->captureMouse = true;
		}
		if(this->mouseLPressed){
			this->captureMouse = true;
		}
				this->mouseOverButton = true;
	}
	restoreDefaults();
	return *this;
}
void IMGUI::restoreDefaults(){
	m_buttonFlags = 0;
	m_imageEnbl = false;
	m_text = "";
	m_caretPosition = -1;
	m_color = m_styles[m_style].background;
	m_style = m_boxStack[m_boxIndex].m_style;
	m_font = m_styles[m_style].font;
	m_fontColor = m_styles[m_style].fontColor;
	m_lastBox = m_box;
	//m_box = Box(0, 0, 0, 0);
	m_box = Box(0, 0, 120, 24);
	m_flag = 0;
	m_special = false;
	m_editBox = false;
	m_forceColor = false;
	m_forceNoDraw = false;
	m_textOffset = 0;
}

// ------------------------------------------------------------
void IMGUI::setlClick(bool click){
	mouselClick = true;
	m_lClick = true;
}
void IMGUI::setrClick(bool click){
	m_rClick = true;
}
void IMGUI::setMouse(float x, float y){
	focusPoint = glm::vec2(x,y);
	mousePos = glm::vec2(x,y);
		captureMouse = false;
		mouseOverButton = false;
}
bool IMGUI::overlay(Box &rect, Box &point){
	return 	point.x >= rect.x && point.x <= rect.x+rect.z &&
					point.y >= rect.y && point.y <= rect.y+rect.w;
}
bool IMGUI::hasHover(Box rect){
	if(currentLayer == maxLayer)
		return 	(focusPoint.x >= rect.x && focusPoint.x < rect.x+rect.z &&
					focusPoint.y >= rect.y && focusPoint.y < rect.y+rect.w) && !captureMouse;
	return false;
}
bool IMGUI::outOfTable(){
	auto &&rect = fixRect(m_boxStack[m_boxIndex+1].m_box);
	// if(currentLayer == maxLayer)
		// return this->m_lClick && !(focusPoint.x >= rect.x && focusPoint.x < rect.x+rect.z &&
		return !(focusPoint.x >= rect.x && focusPoint.x < rect.x+rect.z &&
					focusPoint.y >= rect.y && focusPoint.y < rect.y+rect.w);


}
Box IMGUI::fixRect(Box rect){
	if(rect.z < 0.f)
		rect = Box(rect.x+rect.z, rect.y, -rect.z, rect.w);
	if(rect.w < 0.f)
		rect = Box(rect.x, rect.y+rect.w, rect.z, -rect.w);

	return rect;
}
// ------------------------------------------------------------

void IMGUI::begin(){
	draw = true;
	m_boxIndex = 0;
	layerToDraw = 0;
	tmpMaxLayer = 0;
	currentLayer = 0;
	m_force = false;
	m_style = 0;
	m_boxStack[m_boxIndex].m_style = 0;
	m_boxStack[0].m_box = Box(0,0,1400,700);
	m_figures.clear();
	m_rects.clear();
	MS50IMPULSE = false;
	MS100IMPULSE = false;
	for(int i=0; i<5; i++){
		m_specRectsColor[i].clear();
		m_specRects[i].clear();
	}
	m_collRects.clear();
	m_images.clear();
	// this->captureMouse = false;
	if(mouselClick)
		accu = 0;
	if(accu > 1000/frequency){
		accu = 0;
		counter = true;
	}
	if(accu2 > 500){
		accu2 = 0;
		MS500OSCILATOR = !MS500OSCILATOR;
	}
	restoreDefaults();
}
void IMGUI::end(){
	if(m_boxIndex)
		std::cout<<"box index error"<<std::endl;
	m_key = -1;
	m_mouseKey = -1;
	m_force = false;
	drawLayer = layerToDraw;
	maxLayer = tmpMaxLayer;
	mouselClick = false;
	m_lClick = false;
	m_rClick = false;
	mouseLPressed = false;
	mouseRPressed = false;
	counter = false;
	m_collRects.clear();
	// endBox();
}

IMGUIBox& IMGUI::table(int flags){
	auto spawnPosition = placeGroup(flags);
	spawnPosition.z = 0;
	spawnPosition.w = 0;
	return table(spawnPosition, flags);
}
IMGUIBox& IMGUI::table(Box spawnPosition, int flags){
	m_group.m_box = spawnPosition;
	m_group.m_border = 0;
	m_group.m_flags = flags;
	m_group.m_currStart = m_group.m_box;
	//m_group.m_currStart.x = 0;
	//m_group.m_currStart.y = 0;
	m_group.m_freeRect = fixRect(spawnPosition);
	// m_boxStack[0] = m_group;
	spawnPosition.z = 0;
	spawnPosition.w = 0;
	return m_boxStack[++m_boxIndex].box(flags, spawnPosition, this);
}
IMGUIBox& IMGUI::endTable(){
	endBox();
	m_fontSize = m_defaultFontSize;
	m_font = m_defaultFont;
	m_indentation = 0;
	IMGUIBox &group = m_boxStack[m_boxIndex+1];
	float b = group.m_border;

	Box rect = group.m_box;
	if((group.m_flags & FixedSize) != FixedSize){
		if(group.m_flags & LayoutVertical)
			rect.w = (abs(group.m_box.y - group.m_currStart.y)+b)*sign(rect.w);

		else if(group.m_flags & LayoutHorizontal)
			rect.z = (abs(group.m_box.x - group.m_currStart.x)+b)*sign(rect.z);
	}

	rect = fixRect(rect);
	if((group.m_flags & AbsolutePosition) != AbsolutePosition)
		m_collRects.push_back(rect);
	return group;
}

IMGUIBox& IMGUI::box(int flags){
	Box r = Box(0,0,0,0);
	r = m_boxStack[m_boxIndex].getSpawnPoint(r);
	return m_boxStack[++m_boxIndex].box(flags, r, this);
}
IMGUIBox& IMGUIBox::box(int flags, Box spawnPosition, IMGUI *_imgui){
	imgui = _imgui;
	int parentFlags = (imgui->m_group.m_flags | ClearLayout) & ~ClearLayout;

	m_box = spawnPosition;
	m_border = 0;
	m_flags = flags | parentFlags;
	m_currStart = m_box;
	m_currStart.z = 0;
	m_currStart.w = 0;

	m_style = imgui->parentBox().m_style;

	if(m_flags & Draw){
		m_rectIdx = imgui->m_UIContainer->size(UI::BigBox, g_UILayer);
		imgui->m_UIContainer->push(UI::BigBox, imgui->m_styles[m_style].background, spawnPosition, g_UILayer);

	}

	return *this;
}
IMGUIBox& IMGUIBox::size(float x, float y){
	auto &parentBox = imgui->parentBox().m_box;
	if(x>-1 && x<1){
		x = floor(parentBox.z * x);
	}
	if(y>-1 && y<1){
		y = floor(parentBox.w * y);
	}
	m_box.z = x;
	m_box.w = y;
	m_flags |= FixedSize;

	return *this;
}
IMGUIBox& IMGUIBox::position(float x, float y){
	float u(x),v(y);
	auto &parentBox = imgui->parentBox().m_box;
	if(x>-1 && x<1){
		u = floor(parentBox.z*x);
	}
	if(y>-1 && y<1){
		v = floor(parentBox.w*y);
	}
	m_currStart.x = parentBox.x + u;
	m_currStart.y = parentBox.y + v;
	m_box.x = parentBox.x + u;
	m_box.y = parentBox.y + v;
	return *this;
}
IMGUIBox& IMGUIBox::overridePosition(float x, float y){
	float u(x),v(y);
	auto &parentBox = imgui->parentBox().m_box;

	m_flags |= AbsolutePosition;

	m_currStart.x = x;
	m_currStart.y = y;
	m_box.x = x;
	m_box.y = y;
	return *this;
}
IMGUIBox& IMGUIBox::offset(float x, float y){
	m_flags |= RelativePosition;
	float u(x),v(y);
	auto &parentBox = imgui->parentBox().m_box;
	if(x>-1 && x<1){
		u += floor(parentBox.z*x);
	}
	if(y>-1 && y<1){
		v += floor(parentBox.w*y);
	}
	if(x<0){
		u += parentBox.z;
	}
	if(y<0){
		v += parentBox.w;
	}
	m_currStart.x += u;
	m_currStart.y += v;
	m_box.x += u;
	m_box.y += v;
	return *this;
}
IMGUIBox& IMGUIBox::style(StyleID s){
	m_style = (int)s;
	return *this;
}
IMGUIBox& IMGUIBox::border(int b){
	m_border = b;
	return *this;
}
IMGUIBox& IMGUIBox::operator()(){
	return *this;
}

IMGUIBox& IMGUI::endBox(){
	IMGUIBox &group = m_boxStack[m_boxIndex];
	float b = group.m_border;

	Box rect = group.m_box;
	// if(!(group.m_flags & FixedSize)){
		if(group.m_flags & LayoutVertical)
			rect.w = std::max((abs(group.m_box.y - group.m_currStart.y)+b), abs(group.m_box.w))*sign(rect.w);

		else if(group.m_flags & LayoutHorizontal)
			rect.z = std::max((abs(group.m_box.x - group.m_currStart.x)+b), abs(group.m_box.z))*sign(rect.z);
	// }

	rect = fixRect(rect);

	if(group.m_flags & Draw){
		m_UIContainer->change(UI::BigBox, group.m_rectIdx, m_styles[m_style].background, rect, g_UILayer);

	}

	m_boxStack[m_boxIndex].m_box = rect;
	m_boxIndex--;
	if((m_boxStack[m_boxIndex+1].m_flags & AbsolutePosition) != AbsolutePosition)
		m_boxStack[m_boxIndex].insertRect(rect+Box(-1,-1,0.5f,1));
	return group;
}
IMGUIBox& IMGUIBox::onGroupHover(std::function<void(Box rect)>fun){
	imgui->onGroupHover(fun);
	return *this;
}
IMGUIBox& IMGUIBox::onGrouplClick(std::function<void(Box rect)>fun){
	imgui->onGrouplClick(fun);
	return *this;
}
bool IMGUIBox::onGroupGrab(std::function<void(Box rect)>fun){
	return imgui->onGroupGrab(fun);
}

Box IMGUIBox::getSpawnPoint(const Box &r){
	Box rect(r);

	float border = m_border;
	float h = r.w;
	float w = r.z;

	if(m_flags & LayoutVertical){
		float mod;
		if(m_flags & AlignTop)
			mod = -1.f;
		else if(m_flags & AlignBottom)
			mod = 1.f;

		if(m_flags & AlignLeft){
			rect = m_currStart + Box(border,mod*border,w, h);
		}
		else if(m_flags & AlignRight){
			rect = m_currStart + Box(-border-w,mod*border,w, h);
		}
		if(mod == -1.f){
			rect = Box(rect.x, rect.y - rect.w, rect.z, rect.w);
		}
	}

	else if(m_flags & LayoutHorizontal){
		float mod;
		if(m_flags & AlignLeft)
			mod = 1.f;
		else if(m_flags & AlignRight)
			mod = -1.f;

		if(m_flags & AlignBottom){
			rect = m_currStart + Box(mod*border,1*border,w, h);
		}
		else if(m_flags & AlignTop){
			rect = m_currStart + Box(mod*border,-1*border-h,w, h);
		}
		if(mod == -1.f){
			rect = Box(rect.x-rect.z, rect.y, rect.z, rect.w);
		}
	}

	return rect;
}
Box IMGUIBox::insertRect(const Box &r){ // r is fixed
	Box rect;

	glm::vec2 r1 =  m_currStart.xy() - r.xy();
	glm::vec2 r2 =  m_currStart.xy() - (r.xy() + r.zw());
	float border = 0;
	// float w = r.z;
	// float h = r.w;
	float w = std::max(abs(r1.x), abs(r2.x))+1;
	float h = std::max(abs(r1.y), abs(r2.y))+1;

	if(m_flags & LayoutVertical){

		float mod = 0;
		if(m_flags & AlignTop)
			mod = -1.f;
		else if(m_flags & AlignBottom)
			mod = 1.f;

		if(m_flags & AlignLeft){
			m_box.z = max(m_box.z, w+2*border);
		}
		else if(m_flags & AlignRight){
			m_box.z = min(m_box.z, -w-2*border);
		}

		m_currStart += Box(0,(border + h-1)*mod,0,0);

		if(!(m_flags & FixedSize))
			m_box += Box(0,0,0,(border + h-1)*mod);

	}

	else if(m_flags & LayoutHorizontal){
		float mod=1.f;
		if(m_flags & AlignLeft)
			mod = 1.f;
		else if(m_flags & AlignRight)
			mod = -1.f;

		if(m_flags & AlignBottom){
			m_box[3] = max(m_box[3], h+2*border);

		}
		else if(m_flags & AlignTop){
			m_box[3] = min(m_box[3], -h-2*border);
		}

		m_currStart += Box((border + w-1)*mod,0,0,0);

		if(!(m_flags & FixedSize))
			m_box += Box(0,0,(border + w-1)*mod,0);
	}

	return rect;
}
Box IMGUIBox::placeRect(const Box &r){
	auto rect = getSpawnPoint(r);
	insertRect(rect);
	return rect;
}

bool IMGUI::findCollision(Box &r, Box &out){
	for(auto &it : m_collRects){

		if(overlay(it, r)){
			out = it;
			return true;
		}
	}
	return false;
}
Box IMGUI::placeGroup(int flags){
	Box pos, out;

	Box m_freeRect = Box(0,0,m_maxHorizontal,m_maxVertical);

	if(flags & LayoutVertical){
		if(flags & AlignLeft && flags & AlignTop){
			pos = Box(m_freeRect.x, m_freeRect.y+m_freeRect.w,0,0);
			while(findCollision(pos, out)){
				if(out.y + out.w < 300)
					pos += Box(out.z+1,0,0,0);
				else
					pos -= Box(0,out.w+1,0,0);

			}
			pos.z = m_freeRect.z - pos.x + m_freeRect.x;
			pos.w = m_freeRect.y - pos.y;
		}
		else if(flags & AlignRight && flags & AlignTop){
			pos = Box(m_freeRect.x+m_freeRect.z, m_freeRect.y+m_freeRect.w,0,0);
			while(findCollision(pos, out)){
				if(out.z < out.w)
					pos -= Box(out.z+1,0,0,0);
				else
					pos -= Box(0,out.w+1,0,0);

			}
			pos.z = m_freeRect.x - pos.x;
			pos.w = m_freeRect.y - pos.y;
		}
		else if(flags & AlignLeft && flags & AlignBottom){
			pos = Box(m_freeRect.x, m_freeRect.y,0,0);
			while(findCollision(pos, out)){
				if(out.z < out.w)
					pos += Box(out.z+1,0.f,0.f,0.f);
				else
					pos += Box(0.f,out.w+1,0.f,0.f);
			}
			pos.z = m_freeRect.x+m_freeRect.z - pos.x;
			pos.w = m_freeRect.y+m_freeRect.w - pos.y;
		}
		else if(flags & AlignRight && flags & AlignBottom){
			pos = Box(m_freeRect.x+m_freeRect.z, m_freeRect.y,0,0);
			while(findCollision(pos, out)){
				if(out.y + out.w > 250)
					pos -= Box(out.z+1,0,0,0);
				else
					pos += Box(0,out.w+1,0,0);

			}
			pos.z = m_freeRect.x - pos.x;
			pos.w = m_freeRect.y+m_freeRect.w - pos.y;
		}


	}

	else if(flags & LayoutHorizontal){

		if(flags & AlignLeft && flags & AlignTop){
			pos = Box(m_freeRect.x, m_freeRect.y+m_freeRect.w,0,0);
			while(findCollision(pos, out)){
				if(out.x + out.z > 300)
					pos -= Box(0,out.w+1,0,0);
				else
					pos += Box(out.z+1,0,0,0);

			}
			pos.z = m_freeRect.z - pos.x + m_freeRect.x;
			pos.w = m_freeRect.y - pos.y;
		}
		else if(flags & AlignRight && flags & AlignTop){
			pos = Box(m_freeRect.x+m_freeRect.z, m_freeRect.y+m_freeRect.w,0,0);
			while(findCollision(pos, out)){
				if(out.w < out.z){
					pos -= Box(0,out.w+1,0,0);
					}
				else{
					pos -= Box(out.z+1,0,0,0);
					}

			}
			pos.z = m_freeRect.x - pos.x;
			pos.w = m_freeRect.y - pos.y;
		}
		else if(flags & AlignLeft && flags & AlignBottom){
			pos = Box(m_freeRect.x, m_freeRect.y,0,0);
			while(findCollision(pos, out)){
				if(out.x + out.z > 300)
					pos += Box(0.f,out.w+1,0.f,0.f);
				else
					pos += Box(out.z+1,0.f,0.f,0.f);

			}
			pos.z = m_freeRect.x+m_freeRect.z - pos.x;
			pos.w = m_freeRect.y+m_freeRect.w - pos.y;
		}
		else if(flags & AlignRight && flags & AlignBottom){
			pos = Box(m_freeRect.x+m_freeRect.z, m_freeRect.y,0,0);
			while(findCollision(pos, out)){
				if(out.x + out.z < 300)
					pos += Box(0,out.w+1,0,0);
				else
					pos -= Box(out.z+1,0,0,0);

			}
			pos.z = m_freeRect.x - pos.x;
			pos.w = m_freeRect.y+m_freeRect.w - pos.y;
		}




	}

	return pos;
}

void IMGUI::printTextEditorValue(){
		if(MS500OSCILATOR)
			this->m_caretPosition = this->m_text.size() + textEditor.caretPosition();
		text(this->textEditor.value(),0,this->m_caretPosition);
}
template<typename T>
bool IMGUI::processTE(T &value, int base){
		bool out = false;
	if(!this->m_underEdition && this->m_lClicked || this->force()){
		this->textEditor.setValueToEdit(value, base);
	}
	else if(this->m_underEdition && this->m_lClick && !this->m_hover){
		this->textEditor.breakEdition();
	}

	onEnter([this, &out]{
		this->textEditor.finishEdition();
		this->m_special = true;
				out = true;
	});
		return out;
}
template<typename T>
bool IMGUI::processTE(T &value){
		bool out = false;
	if( (!this->m_underEdition && this->m_lClicked) || this->force()){
		this->textEditor.setValueToEdit(value);
	}
	else if(this->m_underEdition && this->m_lClick && !this->m_hover){
		this->textEditor.breakEdition();
	}

	onEnter([this, &out]{
		this->textEditor.finishEdition();
		this->m_special = true;
				out = true;
	});
		return out;
}

TextEditor::TextEditor(){
	m_caretPosition = 0;
	m_typeInfo = TypeInfo::EMPTY;
}

template<typename T>
void TextEditor::setValueToEdit(T &value, int base){
	valuePointer = (void*)&value;
	// currentString = std::to_string(value);
	currentString = prettyPrint(value, base);
	m_state = true;
	m_caretPosition = currentString.size();
	m_typeInfo = recognizeType<T>();
	m_base = base;
}
template<typename T>
void TextEditor::setValueToEdit(T &value){
	valuePointer = (void*)&value;
	currentString = std::to_string(value);
	m_state = true;
	m_caretPosition = currentString.size();
	m_typeInfo = TypeInfo::INT32_t;
	m_base = 10;
}
template<>
void TextEditor::setValueToEdit<double>(double &value){
	valuePointer = (void*)&value;
	currentString = std::to_string(value);
	m_state = true;
	m_caretPosition = currentString.size();
	m_typeInfo = TypeInfo::DOUBLE;
}
template<>
void TextEditor::setValueToEdit<float>(float &value){
	valuePointer = (void*)&value;
	currentString = std::to_string(value);
	m_state = true;
	m_caretPosition = currentString.size();
	m_typeInfo = TypeInfo::FLOAT;
}
template<>
void TextEditor::setValueToEdit<std::string>(std::string &value){
	valuePointer = (void*)&value;
	currentString = value;
	m_state = true;
	m_caretPosition = currentString.size();
	m_typeInfo = TypeInfo::STRING;
}
bool TextEditor::state(){
	return m_state;
}
std::string TextEditor::value(){
	return currentString;
}
void TextEditor::finishEdition(){
	m_state = false;
	if(m_typeInfo == TypeInfo::INT32_t)
		*(int32_t*)valuePointer = std::stoi(currentString,nullptr, m_base);
	if(m_typeInfo == TypeInfo::UINT32_t)
		*(uint32_t*)valuePointer = std::stoi(currentString,nullptr, m_base);
	if(m_typeInfo == TypeInfo::INT64_t)
		*(int64_t*)valuePointer = std::stoi(currentString,nullptr, m_base);
	if(m_typeInfo == TypeInfo::UINT64_t)
		*(uint64_t*)valuePointer = std::stoi(currentString,nullptr, m_base);
	else if(m_typeInfo == TypeInfo::FLOAT)
		*(float*)valuePointer = parseExpr(currentString);
	else if(m_typeInfo == TypeInfo::DOUBLE){
		std::cout<<"Duuuuuupa"<<endl;
		*(double*)valuePointer = std::stod(currentString);
	}
	else if(m_typeInfo == TypeInfo::STRING){
		// swap(*(std::string*)valuePointer, currentString);
		*(std::string*)valuePointer = currentString;
	}
	m_typeInfo = TypeInfo::EMPTY;
	valuePointer = nullptr;
}
void TextEditor::breakEdition(){
	m_typeInfo = TypeInfo::EMPTY;
	m_state = false;
	valuePointer = nullptr;
}
int TextEditor::caretPosition(){
	return m_caretPosition;
}
size_t goToLeft(const std::string &s, int end){
		int start = end-1;
		for(start; start>=0; start--){
				char c = s[start];
				if(('a'>c || 'z'<c) && ('A'>c || 'Z'<c) && ('0'>c || '9'<c)){
						return glm::clamp(start, 0, int(s.size()-1));
				}
		}
}
size_t goToRight(const std::string &s, int start){
		int end = start+1;
		for(end; end<s.size(); end++){
				char c = s[end];
				if(('a'>c || 'z'<c) && ('A'>c || 'Z'<c) && ('0'>c || '9'<c)){
						return glm::clamp(start, 0, int(s.size()-1));
				}
		}
}
void TextEditor::input(u32 key, u32 action, u32 mod){
	char input = 0;

	if(key == GLFW_KEY_ENTER && action == GLFW_PRESS && !GetInput)
		GetInput = true;
	else if(GetInput && (action == GLFW_PRESS || action == GLFW_REPEAT)){

		// if(key == GLFW_KEY_ENTER)
			// finishEdition();
		if(key == GLFW_KEY_ESCAPE)
			breakEdition();

		if(mod == GLFW_MOD_CONTROL){
			// if(key == GLFW_KEY_DELETE  && u32(m_caretPosition)<currentString.size()){
			if(key == GLFW_KEY_DELETE){
								auto newPos = goToRight(currentString, m_caretPosition);
				currentString.erase(m_caretPosition, newPos-m_caretPosition);
				// m_caretPosition = glm::clamp(m_caretPosition, 0, (int)currentString.size());
			}
			// else if(key == GLFW_KEY_BACKSPACE && m_caretPosition>0){
			else if(key == GLFW_KEY_BACKSPACE){
								auto newPos = goToLeft(currentString, m_caretPosition);
				currentString.erase(newPos, m_caretPosition-newPos);
				m_caretPosition = newPos;
				// m_caretPosition = glm::clamp(m_caretPosition, 0, (int)currentString.size());
			}
			else if(key == GLFW_KEY_RIGHT){
				m_caretPosition = goToRight(currentString, m_caretPosition);
						}
			else if(key == GLFW_KEY_LEFT){
				m_caretPosition = goToLeft(currentString, m_caretPosition);
						}
			else if(key == 'V'){
								const char* clipboard = getClipboard();
				currentString.insert(m_caretPosition, clipboard);
						}
		}
		else {
			if(key == GLFW_KEY_DELETE  && u32(m_caretPosition)<currentString.size()){
				currentString.erase(m_caretPosition,1);
				m_caretPosition = glm::clamp(m_caretPosition, 0, (int)currentString.size());
			}
			if(key == GLFW_KEY_BACKSPACE && m_caretPosition>0){
				m_caretPosition -= 1;
				currentString.erase(m_caretPosition,1);
				m_caretPosition = glm::clamp(m_caretPosition, 0, (int)currentString.size());
			}
			if(key == GLFW_KEY_RIGHT)
				m_caretPosition++;
			if(key == GLFW_KEY_LEFT)
				m_caretPosition--;
		}
		if(mod != GLFW_MOD_SHIFT && key<=127){
			if(key>='A' && key<='Z')
				input = key+32;
			else
				input = key;
		}
		//SHIFTEd
		if(mod == GLFW_MOD_SHIFT && key<=127){
			if(key>='A' && key<='Z')
				input = key;//a->A

			else switch(key){
				case '1':	input = '!'; break;
				case '2':	input = '@'; break;
				case '3':	input = '#'; break;
				case '4':	input = '$'; break;
				case '5':	input = '%'; break;
				case '6':	input = '^'; break;
				case '7':	input = '&'; break;
				case '8':	input = '*'; break;
				case '9':	input = '('; break;
				case '0':	input = ')'; break;
				case '[':	input = '{'; break;
				case ']':	input = '}'; break;
				case ';':	input = ':'; break;
				case '\'':input = '"'; break;
				case ',':	input = '<'; break;
				case '.':	input = '>'; break;
				case '/': input = '?'; break;
				case '-':	input = '_'; break;
				case '=':	input = '+'; break;
				case '`':	input = '~'; break;
			}
		}

		// numeric key
		if(key>=GLFW_KEY_KP_0 && key<=GLFW_KEY_KP_ADD ){
			switch(key){
				case GLFW_KEY_KP_0:	input = '0'; break;
				case GLFW_KEY_KP_1:	input = '1'; break;
				case GLFW_KEY_KP_2:	input = '2'; break;
				case GLFW_KEY_KP_3:	input = '3'; break;
				case GLFW_KEY_KP_4:	input = '4'; break;
				case GLFW_KEY_KP_5:	input = '5'; break;
				case GLFW_KEY_KP_6:	input = '6'; break;
				case GLFW_KEY_KP_7:	input = '7'; break;
				case GLFW_KEY_KP_8:	input = '8'; break;
				case GLFW_KEY_KP_9:	input = '9'; break;

				case GLFW_KEY_KP_DECIMAL:	input = ','; break;
				case GLFW_KEY_KP_DIVIDE:	input = '/'; break;
				case GLFW_KEY_KP_MULTIPLY:	input = '*'; break;
				case GLFW_KEY_KP_SUBTRACT:	input = '-'; break;
				case GLFW_KEY_KP_ADD :	input = '+'; break;
			}
		}
		m_caretPosition = glm::clamp(m_caretPosition, 0, (int)currentString.size());
		if(input){
			// if((m_typeInfo == INT32_t && input >='0' && input<='9') || (m_typeInfo == FLOAT && ((input>='0' && input<='9') || input =='.' || input ==',') ) || m_typeInfo == STRING){
			if(true){

				currentString.insert(m_caretPosition, &input, 1);
				m_caretPosition++;
			}
		}
	}

}
template<typename T>
bool TextEditor::compare(T &value){
	return &value == this->valuePointer;
}

float TextEditor::parseExpr(string &str){
	if(str == "pi"s)
		return pi;
	else if(str == "pi/2"s)
		return pi/2;
	else if(str == "pi/4"s)
		return pi/4;
	else if(str == "2pi"s)
		return pi*2;
	else if(str == "-pi"s)
		return -pi;
	else if(str == "-pi/2"s)
		return -pi/2;
	else if(str == "-pi/4"s)
		return -pi/4;
	else if(str == "-2pi"s)
		return -pi*2;

	return stof(str);
}

template<>
TypeInfo recognizeType<int32_t>(){
	return TypeInfo::INT32_t;
}
template<>
TypeInfo recognizeType<uint32_t>(){
	return TypeInfo::UINT32_t;
}
template<>
TypeInfo recognizeType<int64_t>(){
	return TypeInfo::INT64_t;
}
template<>
TypeInfo recognizeType<uint64_t>(){
	return TypeInfo::UINT64_t;
}
template<>
TypeInfo recognizeType<std::string>(){
	return TypeInfo::STRING;
}
template<>
TypeInfo recognizeType<float>(){
	return TypeInfo::FLOAT;
}
template<>
TypeInfo recognizeType<double>(){
	return TypeInfo::DOUBLE;
}

NAM_END
