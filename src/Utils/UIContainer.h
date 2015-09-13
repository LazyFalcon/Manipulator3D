#pragma once
namespace UI {
	enum DrawFlags : int;
	class IMGUI;
	struct Style;
};
struct IUIContainer {
	virtual void push(const int drawFlags, const UI::Style &style, const glm::vec4 box, const int layer) = 0;
	virtual void push(const int drawFlags, const HexColor color, const glm::vec4 box, const int layer) = 0;
	virtual void push(const int drawFlags, const Icon icon) = 0;
	virtual void draw(UI::IMGUI &gui, u32 layer) = 0;
	virtual void change(const int drawFlags, const int idx, const HexColor color, const glm::vec4 box, u32 layer) = 0;
	virtual int size(const int drawFlags, u32 layer) = 0;
	virtual ~IUIContainer(){}
};

struct BoxColor {
	std::vector <HexColor>  m_color;
	std::vector <glm::vec4> m_box;
};
struct UIContainer : public IUIContainer {
	void push(const int drawFlags, const UI::Style &style, const glm::vec4 box, const int layer);
	void push(const int drawFlags, const HexColor color, const glm::vec4 box, const int layer);
	void push(const int drawFlags, const Icon icon);
	void draw(UI::IMGUI &gui, u32 layer);
	void change(const int drawFlags, const int idx, const HexColor color, const glm::vec4 box, u32 layer);
	int size(const int drawFlags, u32 layer);

	UIContainer(){
		m_images.second = "UIIcon2D";
		m_labels[0].second = "UIButton";
		m_labels[1].second = "UIButton";
		m_labels[2].second = "UIButton";
		m_hovers[0].second = "UIButton";
		m_hovers[1].second = "UIButton";
		m_hovers[2].second = "UIButton";
		m_editBoxes[0].second = "UIButton";
		m_editBoxes[1].second = "UIButton";
		m_editBoxes[2].second = "UIButton";
		m_bigBoxes[0].second = "UIButton";
		m_bigBoxes[1].second = "UIButton";
		m_bigBoxes[2].second = "UIButton";
	}

	std::pair<std::vector <Icon>, std::string> m_images;

	std::pair<BoxColor, std::string> m_labels[3];
	std::pair<BoxColor, std::string> m_hovers[3];
	std::pair<BoxColor, std::string> m_editBoxes[3];
	std::pair<BoxColor, std::string> m_bigBoxes[3];
};
struct GameUIContainer : public IUIContainer {
	void push(const int drawFlags, const UI::Style &style, const glm::vec4 box, const int layer);
	void push(const int drawFlags, const HexColor color, const glm::vec4 box, const int layer);
	void push(const int drawFlags, const Icon icon);
	void draw(UI::IMGUI &gui, u32 layer);
	void change(const int drawFlags, const int idx, const HexColor color, const glm::vec4 box, u32 layer);
	int size(const int drawFlags, u32 layer);

	GameUIContainer(){
		m_images.second = "shapeIcon";
		m_boxes.second = "shapeRectColoredI";
	}

	std::pair<std::vector <Icon>, std::string> m_images;
	std::pair<BoxColor, std::string> m_boxes;
};
struct GameHUDContainer : public IUIContainer {
	void push(const int drawFlags, const UI::Style &style, const glm::vec4 box, const int layer);
	void push(const int drawFlags, const Icon icon);
	void draw(UI::IMGUI &gui, u32 layer);
	void change(const int drawFlags, const int idx, const HexColor color, const glm::vec4 box, u32 layer);
	int size(const int drawFlags, u32 layer);

	GameHUDContainer(){
		m_images.second = "shapeIcon";
		m_boxes.second = "shapeRectColoredI";
	}

	std::pair<std::vector <Icon>, std::string> m_images;
	std::pair<BoxColor, std::string> m_boxes;
};


