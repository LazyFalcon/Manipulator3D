void UIContainer::push(const int drawFlags, const UI::Style &style, const glm::vec4 box, const int layer){
    if(drawFlags & UI::Hover){
        m_labels[layer].first.m_color.push_back(style.hover);
        m_labels[layer].first.m_box.push_back(box);
    }
    else if(drawFlags & UI::Editable){
        m_editBoxes[layer].first.m_color.push_back(style.editBox);
        m_editBoxes[layer].first.m_box.push_back(box);
    }
    else if(drawFlags & UI::Label){
        m_labels[layer].first.m_color.push_back(style.button);
        m_labels[layer].first.m_box.push_back(box);
    }
    else if(drawFlags & UI::BigBox){
        m_bigBoxes[layer].first.m_color.push_back(style.background);
        m_bigBoxes[layer].first.m_box.push_back(box);
    }
}
void UIContainer::push(const int drawFlags, const HexColor color, const glm::vec4 box, const int layer){
    if(drawFlags & UI::Hover){
        m_hovers[layer].first.m_color.push_back(color);
        m_hovers[layer].first.m_box.push_back(box);
    }
    else if(drawFlags & UI::Editable){
        m_editBoxes[layer].first.m_color.push_back(color);
        m_editBoxes[layer].first.m_box.push_back(box);
    }
    else if(drawFlags & UI::Label){
        m_labels[layer].first.m_color.push_back(color);
        m_labels[layer].first.m_box.push_back(box);
    }
    else if(drawFlags & UI::BigBox){
        m_bigBoxes[layer].first.m_color.push_back(color);
        m_bigBoxes[layer].first.m_box.push_back(box);
    }
}
void UIContainer::push(const int drawFlags, const Icon icon){
	m_images.first.push_back(icon);
}
int UIContainer::size(const int drawFlags, u32 layer){
	return m_bigBoxes[layer].first.m_box.size();
}

void UIContainer::change(const int drawFlags, const int idx, const HexColor color, const glm::vec4 box, u32 layer){
		m_bigBoxes[layer].first.m_box[idx] = box;
		m_bigBoxes[layer].first.m_color[idx] = color;
}
