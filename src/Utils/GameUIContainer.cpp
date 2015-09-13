

void GameUIContainer::push(const UI::DrawFlags drawFlags, const HexColor color, const glm::vec4 box, const int layer){
	m_boxes.first.m_color.push_back(color);
	m_boxes.first.m_box.push_back(box);

}
void GameUIContainer::push(const UI::DrawFlags drawFlags, const Icon icon){
	m_images.first.push_back(icon);
}
int GameUIContainer::size(const UI::DrawFlags drawFlags){
	return m_boxes.first.m_box.size();
}

void GameHUDContainer::push(const UI::DrawFlags drawFlags, const HexColor color, const glm::vec4 box, const int layer){
	m_boxes.first.m_color.push_back(color);
	m_boxes.first.m_box.push_back(box);

}
void GameHUDContainer::push(const UI::DrawFlags drawFlags, const Icon icon){
	m_images.first.push_back(icon);
}

int GameHUDContainer::size(const UI::DrawFlags drawFlags){
	return m_boxes.first.m_box.size();
}
void GameUIContainer::change(const UI::DrawFlags drawFlags, const int idx, const HexColor color, const glm::vec4 box){
		m_boxes.first.m_box[idx] = box;
		m_boxes.first.m_color[idx] = color;
}
void GameHUDContainer::change(const UI::DrawFlags drawFlags, const int idx, const HexColor color, const glm::vec4 box){
		m_boxes.first.m_box[idx] = box;
		m_boxes.first.m_color[idx] = color;
}