#pragma once
float cacheMx = 0.f;
float cacheMy = 0.f;
bool enableCameraZoom = false;

class Alert {
	bool active = false;
	string text = "";
public:
	~Alert(){
		// cerr<<"~Alert"<<endl;
	}
	void operator()(string _text){
		active = true;
		text = _text;
	}
	void show(){
		if(!active)
			return;
		ui.table(glm::vec4(window_width / 2 - 100, window_height / 2, 200, -100), UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::FixedSize | UI::FixedPos);
			ui.border(0);
			ui.rect(200, 50).text(text, "weblysleekB17"s, UI::CenterText)();
			ui.beginLayer();
			ui.rect(200, 50).text("OK", "weblysleekB17"s, UI::CenterText).button(active)(UI::Hoverable);
			ui.endLayer();

		ui.endTable();
	}

}alert;

class Button {
public:
	string name;
	string image;
	HexColor mainColor;
	std::function<void(void)>fun;

	std::vector <Button> children;
	int mark;
	glm::vec4 position;
	bool marked;
	bool operator()(){

			for(int id=0; id<children.size(); id++){
				auto &it = children[id];
				if (it.image != ""){
					ui.image(it.image).color(it.mainColor)(UI::Hoverable)
						.onlClick([id, &it, this](glm::vec4 r){
							mark = id;
							position = r;
							marked = true;
							it.fun();
						})
						.onHover([&it, id, this](glm::vec4 r){
							if (mark != id)ui.rect(r.x, r.y - 20, 100, 20).text(it.name)();
						});
				}
				else {
					ui.rect(150, 20).color(it.mainColor).font("ui_12").text(it.name);
					if (it.children.size())ui.text(u"\ue0e3", "sym_12", UI::TextToRight);
					ui(UI::Hoverable).onlClick([id,&it,this](glm::vec4 r){
							mark = id;
							position = r;
							position.x += 150;
							position.y += 21;
							marked = true;
							it.fun();
						});
				}
			}

			ui.endTable();


			if(!marked)
				return marked;
			if(ui.lClick() && ui.outOfTable())
				marked = false;
			ui.table(position,UI::LayoutVertical | UI::AlignTop | UI::AlignLeft | UI::Draw);
				marked |= children[mark]();

			return marked;
	}
};

Button TopBar {"","",colorWhite, []{}, std::vector<Button>{
		Button {"Settings", "Settings", colorWhite,  []{}, vector<Button>{
			Button {"Graphic","", colorWhite, []{alert("graphic settings"s);}},
			Button {"Lights","",  colorWhite, []{globalSettings ^= LIGHTS;}},
			Button {"Wireframe","",  colorWhite, []{}},
			Button {"Shadows","",  colorWhite, []{}},
			Button {"SSAO","",  colorWhite, []{}},
			Button {"DOF","",  colorWhite, []{}},
			Button {"HDR","",  colorWhite, []{}},
			Button {"MSAA","",  colorWhite, []{globalSettings ^= MSAA;}},
			Button {"God Rays","",  colorWhite, []{}},
			Button {"Object Outlining","",  colorWhite, []{}}, // http://www.learnopengl.com/#!Advanced-OpenGL/Stencil-testing
			Button {"Colors","",  colorWhite, []{globalSettings ^=  DRAW_COLORS;}},
			Button {"Normals","",  colorWhite, []{globalSettings ^= DRAW_NORMALS;}},
			Button {"Depth","",  colorWhite, []{globalSettings ^=   DRAW_DEPTH;}},
			Button {"Lights","",  colorWhite, []{globalSettings ^=  DRAW_LIGHTS;}},
		}},
		Button {"Scene", "Scene", colorWhite, []{}, std::vector<Button>{
			Button {"Load","",  colorWhite, []{}},
			Button {"Reload","",  colorWhite, []{}},
			Button {"Clear","",  colorWhite, []{}},
			Button {"Prevent Collisions","",  colorWhite, []{}, std::vector<Button>{
				Button {"All", "", colorWhite, []{}},
				Button {"Some", "", colorWhite, []{}},
				Button {"None", "", colorWhite, []{}},
			}},
			Button {"Export Data","",  colorWhite, []{}},
			Button {"Grids","",  colorWhite, []{}, std::vector<Button>{
				Button {"XY", "", colorWhite, []{globalSettings = globalSettings ^ DRAW_XY_GRID;}},
				Button {"XZ", "", colorWhite, []{globalSettings = globalSettings ^ DRAW_XZ_GRID;}},
				Button {"YZ", "", colorWhite, []{globalSettings = globalSettings ^ DRAW_YZ_GRID;}},
			}},
		}},
		Button {"Script", "File", colorWhite, []{}, std::vector<Button>{
			Button {"Load","",  colorWhite, []{}},
			Button {"Reload","",  colorWhite, []{}},
			Button {"Clear","",  colorWhite, []{}},
			Button {"Run","",  colorWhite, []{}},
			Button {"Pause","",  colorWhite, []{}},
			Button {"Stop","",  colorWhite, []{}},
		}},
		// Button {"Play", "Play", colorWhite, []{g_RC.run();}},
		// Button {"Pause", "Pause", colorWhite, []{g_RC.pause();}},
		// Button {"Stop", "Stop", colorWhite, []{g_RC.stop();}},
		// Button{ "Undo", "Undo", colorWhite, []{g_RC.prev(); } },
		// Button{ "Redo", "Redo", colorWhite, []{g_RC.next(); } },

		Button {"Play", "Play", colorWhite, []{BigSplineTest::run();}},
		Button {"Pause", "Pause", colorWhite, []{BigSplineTest::pause();}},
		Button {"Stop", "Stop", colorWhite, []{BigSplineTest::stop();}},
		Button{ "Previous", "Undo", colorWhite, []{BigSplineTest::prev(); } },
		Button{ "Next", "Redo", colorWhite, []{BigSplineTest::next(); } },
		Button {"Save", "Save", colorWhite, []{}},
		Button {"Load", "Load", colorWhite, []{}},
}};

class MovableBar {
public:
	glm::vec4 position;
	glm::vec2 distance;
	bool isMoving = false;
	MovableBar(glm::vec4 pos): position(pos){}
	~MovableBar(){
		// cerr<<"~MovableBar"<<endl;
	}
	void move(){
		isMoving = ui.onGroupGrab([this](glm::vec4){
			if(isMoving){
				position.x = mousePosition.x - distance.x;
				position.y = mousePosition.y - distance.y;
			}
			else {
				isMoving = true;
				distance = mousePosition - position.xy();
			}
			ui.captureMouse = true;
		});
	}

};

class SideBarPoints : public MovableBar  {
	bool minimized = true;
public:
	SideBarPoints(): MovableBar(glm::vec4(1400-160, 700-200,0,0)) {}
	~SideBarPoints(){
		// cerr<<"~SideBarPoints"<<endl;
	}

	void operator ()(){
		ui.image(minimized?window_width-12:position.x+150, minimized?window_height-212:position.y-10, minimized?"Maximize"s:"Minimize"s)()
		.onlClick([this]{
			minimized = !minimized;
		});
		if(minimized)
			return;

		ui.table(position, UI::LayoutVertical | UI::AlignTop | UI::AlignLeft  | UI::FixedPos);
		ui.border(2);

			ui.box(UI::LayoutHorizontal | UI::AlignLeft );
			ui.border(1);
				ui.image("AddPoint"s)();
				ui.image("RemovePoint"s)();
				ui.image("UsePoint"s)();

			ui.endBox();

			ui.box(UI::LayoutVertical);
			ui.border(1);
				ui.rect(150,18).text("punkt 1"s).color(colorWhite).font("ui_12"s)();
				ui.rect(150,18).text("punkt 2"s).color(colorWhite).font("ui_12"s)();
				ui.rect(150,18).text("punkt 3"s).color(colorWhite).font("ui_12"s)();
				ui.rect(150,18).text("punkt 4"s).color(colorWhite).font("ui_12"s)();
			ui.endBox();
			move();
		ui.endTable();
	}

}sideBarPoints;

class PlusMinusWidget {
public:
	Module *module;
	float incrementValue = 2.f*toRad;
	PlusMinusWidget(Module *module): module(module){}

	void operator()(){
		// ui.box(UI::LayoutHorizontal | UI::AlignTop | UI::AlignLeft);
		ui.box(UI::LayoutVertical | UI::AlignTop | UI::AlignLeft);
		ui.image("Minimize")
			.onHover([this]{ module->entity->isOutlined = true; })
			.onlClick([this]{ module->value -=	incrementValue; cout<<"AAAA\n";})
			.onRepeat([this]{ if(signal10ms)module->value -=	incrementValue; })();
		ui.image("Maximize")
			.onHover([this]{ module->entity->isOutlined = true; })
			.onlClick([this]{ module->value +=	incrementValue; cout<<"AAAA\n";})
			.onRepeat([this]{ if(signal10ms)module->value +=	incrementValue; })();
		ui.rect(20,20)
			.text(std::to_string((int)(module->value*toDeg)))();
		ui.endBox();
	}

};

class SteeringConsole : public MovableBar {
	bool minimized = true;
public:
	vector<PlusMinusWidget> buttons;

	SteeringConsole():MovableBar({940,0,456,196}){}
	~SteeringConsole(){
		// cerr<<"~SteeringConsole"<<endl;
	}

	void operator ()(){
		ui.image(minimized?window_width-10:position.x+440,minimized?12:position.y+198, minimized?"Maximize":"Minimize")()
		.onlClick([this]{
			minimized = !minimized;
		});
		if(minimized)
			return;
		ui.table(position, UI::LayoutHorizontal | UI::AlignBottom | UI::AlignLeft | UI::FixedSize | UI::FixedPos);
			ui.border(2);
			ui.box(UI::LayoutHorizontal).position(position + glm::vec4(20, 170, -456, -196));
			for(auto &it : buttons)
				it();
			ui.endBox();
			move();

		ui.endTable();

	}
}steeringConsole;

void recursiveTopMenu(Button button){


}


void MainMenu(){
	// top menu

	ui.table(UI::LayoutHorizontal | UI::AlignTop | UI::AlignLeft | UI::Draw);
		TopBar();
		// sam się zamyka

	ui.table(UI::LayoutHorizontal | UI::AlignTop | UI::AlignRight );
	ui.image("Exit").color(colorRed)()
		.onlClick([&]{
			quit = true;
			})
		.onHover([](glm::vec4 r){
			ui.rect(r.x-10, r.y-20, 150,20).text("exit")();
		});
	ui.endTable();

	// zrobic z tego scrollListę!

	alert.show();
	sideBarPoints();
	steeringConsole();

		if(ui.hasHover(glm::vec4(0,0,window_width, window_height)) &&	ui.lRepeat()){
				float dx = (mouse_x - cacheMx)/window_width*2;
				float dy = (cacheMy - mouse_y)/window_height*2;
				camera.setMouseMov(dx, dy);
				enableCameraZoom = true;
			}
	cacheMx = mouse_x;
	cacheMy = mouse_y;


}
