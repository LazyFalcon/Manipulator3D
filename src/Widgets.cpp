#include "Widgets.h"

namespace wxg NAM_START
void editBox(const std::string &text, float &value, int len){
	ui.box(UI::LayoutHorizontal);
		ui.rect(len/2, 22).text(text)();
		ui.rect(len/2, 22).edit(value)(UI::EditBox);
	ui.endBox();
}


void Window::run(){
	if(beginWindow()) return;
	mainBody();
	endWindow();
}
bool Window::beginWindow(){
	if(minimizable.minimized) return true;

	ui.table(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::FixedPos)
		.position(position)
		.style(style);

		ui.box(UI::LayoutHorizontal | UI::Draw).style(UI::StyleID::Label);
			ui.rect(size-24,24).text(title)();
			minimizable.button(24,24);
		ui.endBox();
	return false;
}

void Window::endWindow(){
	movable.move();
	ui.endTable();
}


NAM_END
