#include "FontRenderer.h"
#include <regex>

#define _DebugLine_  std::cerr<<"line: "<<__LINE__<<"\n";
namespace UI {
	extern float LastTextLength;
	extern float LastTextHeight;
	extern int g_UILayer;
	extern int g_UILayer;

	TextInfo::TextInfo(){}
	TextInfo::TextInfo(glm::vec2 p, const std::string &s, HexColor c, int k){
		m_string = s;
		m_position = p;
		m_caretPosition = k;
		m_color = c;
	}
	void TextInfo::operator =(const TextInfo &t){
		m_string = t.m_string;
		m_position = t.m_position;
		m_caretPosition = t.m_caretPosition;
	}

	FontRenderer::~FontRenderer(){
		std::cerr << "~FontRenderer: " + m_name << std::endl;
	}
	float FontRenderer::getLen(const std::string &text){
		float len = 0.f;
		u8 letter;
		for (u32 i = 0; i < text.size(); i++){
			letter = text[i];
			len += m_letters[letter].size.x;
		}
		return len;
	}

	void FontRenderer::clear(int layer){
		m_empty[layer] = true;
		LastTextHeight = 0;
		m_renderData[layer].positions.clear();
		m_renderData[layer].uvs.clear();
		m_renderData[layer].sizes.clear();
		m_renderData[layer].colors.clear();

		m_renderData[layer].m_size = 0;
		m_renderData[layer].positions.reserve(1000);
		m_renderData[layer].uvs.reserve(1000);
		m_renderData[layer].sizes.reserve(1000);
		m_renderData[layer].colors.reserve(1000);
	}


	void FontRenderer::move(int x, int y){
		u32 len = m_renderData[g_UILayer].positions.size();
		glm::vec2 offset(x, y);
		for (u32 i = len - m_lastTextSize; i < len; i++){
			m_renderData[g_UILayer].positions[i] += offset;
		}
	}


	void FontRenderer::placeCaret(const std::string &text, glm::vec2 position, HexColor color, int caretPosition){
		if(caretPosition >= 0){ // key
			u8 letter = '|';
			if(caretPosition >= text.size() && text.size() > 0)
				m_renderData[g_UILayer].positions.push_back(m_renderData[g_UILayer].positions.back() + m_renderData[g_UILayer].sizes.back()*glm::vec2(1, 0));
			else if(caretPosition == 0)
				m_renderData[g_UILayer].positions.push_back(position - glm::vec2(0, m_letters[letter].size.y - m_height));
			else {
                u32 pos = m_renderData[g_UILayer].positions.size() - text.size() + caretPosition;
				m_renderData[g_UILayer].positions.push_back(m_renderData[g_UILayer].positions[pos]);
            }
			m_renderData[g_UILayer].uvs.push_back(m_letters[letter].uv);
			m_renderData[g_UILayer].sizes.push_back(m_letters[letter].size);
			m_renderData[g_UILayer].colors.push_back(color);
		}
	}

	float FontRenderer::generateText(const std::string &text, glm::vec2 position, HexColor color, int caretPosition){
		m_empty[g_UILayer] = false;
		LastTextHeight = 0;
		LastTextLength = 0;
		m_lastTextSize = text.size();
		glm::vec2 currentPosition = position;
		u8 letter;

		for(u32 i = 0; i < text.size(); i++){
			letter = text[i];
			auto &letterInfo = m_letters[letter];
			if(letter == '\n'){ // new line
				LastTextLength = std::max(LastTextLength, currentPosition[0] - position.x);
				LastTextHeight += m_height;
				currentPosition = position - glm::vec2(0, LastTextHeight);
				continue;
			}

			if (i > 0){ // kerning
				currentPosition[0] += letterInfo.kerning[text[i - 1]];
			}

			m_renderData[g_UILayer].positions.push_back(currentPosition + letterInfo.offset);
			m_renderData[g_UILayer].sizes.push_back(letterInfo.size);
			// m_renderData[g_UILayer].uvs.push_back(letterInfo.uv);
			m_renderData[g_UILayer].uvs.push_back(letterInfo.uv);
			m_renderData[g_UILayer].colors.push_back(color);
			currentPosition.x += letterInfo.advance;
		}

		if (m_lastTextSize > 0) // compute len
			LastTextLength = currentPosition[0] - position.x;

		placeCaret(text, position, color, caretPosition);
		m_renderData[g_UILayer].m_size = m_renderData[g_UILayer].uvs.size();
		return LastTextLength + 1.f;
	}
	float FontRenderer::generateText(const std::u16string &text, glm::vec2 position, HexColor color, int caretPosition){
		m_empty[g_UILayer] = false;
		LastTextHeight = 0;
		LastTextLength = 0;
		m_lastTextSize = text.size();
		glm::vec2 currentPosition = position;
		char16_t letter;

		for (u32 i = 0; i < text.size(); i++){
			letter = text[i];
			auto &letterInfo = m_letters[letter];
			if (letter == '\n'){ // new line
				LastTextLength = std::max(LastTextLength, currentPosition[0] - position.x);
				LastTextHeight += m_height;
				currentPosition = position - glm::vec2(0, LastTextHeight);
				continue;
			}
			if (letter > L'\u00ff'){
				currentPosition.x += m_associatedSymbols->genSingleSymbol(letter, currentPosition, color);
				continue;
			}
			else if (i > 0){ // kerning
				currentPosition[0] += letterInfo.kerning[text[i - 1]];
			}
			m_renderData[g_UILayer].m_size++;

			m_renderData[g_UILayer].positions.push_back(currentPosition + letterInfo.offset);
			m_renderData[g_UILayer].sizes.push_back(letterInfo.size);
			m_renderData[g_UILayer].uvs.push_back(letterInfo.uv);
			m_renderData[g_UILayer].colors.push_back(color);
			currentPosition.x += letterInfo.advance;
		}

		if (m_lastTextSize > 0) // compute len
			LastTextLength = currentPosition[0] - position.x;

		// placeCaret(text, position, color, caretPosition);
		m_renderData[g_UILayer].m_size = m_renderData[g_UILayer].uvs.size();
		return LastTextLength + 1.f;
	}
	float FontRenderer::genSingleSymbol(char16_t symbol, glm::vec2 position, HexColor color){
		m_empty[g_UILayer] = false;
		auto &symbolInfo = m_symbols[symbol];
		m_renderData[g_UILayer].positions.push_back(position + symbolInfo.offset);
		m_renderData[g_UILayer].sizes.push_back(symbolInfo.size);
		m_renderData[g_UILayer].uvs.push_back(symbolInfo.uv);
		m_renderData[g_UILayer].colors.push_back(color);
		m_renderData[g_UILayer].m_size = m_renderData[g_UILayer].uvs.size();
		return symbolInfo.advance;
	}

	glm::vec2 splitVec2(std::string toSplit){
		glm::vec2 out;
		std::istringstream buf(toSplit);
		std::istream_iterator<std::string> beg(buf), end;
		int i = 0;
		for (; beg != end; beg++){
			out[i++] = (float)std::stoi(*beg);
		}
		return out;
	}
	glm::vec4 splitVec4(std::string toSplit){
		glm::vec4 out;
		std::istringstream buf(toSplit);
		std::istream_iterator<std::string> beg(buf), end;
		int i = 0;
		for (; beg != end; beg++){
			out[i++] = (float)std::stoi(*beg);
		}
		return out;
	}


	float Font::push(const std::string &text, const glm::vec2 &pos, HexColor color, int key){
		return generator->generateText(text, pos, color, key);
	}

	float Font::push(const std::u16string &text, const glm::vec2 &pos, HexColor color, int key){
		return generator->generateText(text, pos, color, key);
	}


	void FontRenderer::loadFontInfoXML(std::string name, int _width, int _height){
	/* 	m_name = name;
		std::string path = "../res/fonts\\" + name + ".xml";
		rapidxml::file<> xmlFile(path.c_str()); // Default template is char
		rapidxml::xml_document<> doc;
		doc.parse<0>(xmlFile.data());

		rapidxml::xml_node<> *pRoot = doc.first_node();

		m_height = std::stoi(pRoot->first_attribute("height")->value()) / 2;

		for (rapidxml::xml_node<> *charNode = pRoot->first_node("Char"); charNode; charNode = charNode->next_sibling()){ // pêtla
			glm::vec4 offset = glm::vec4(splitVec2(charNode->first_attribute("offset")->value()), 0, 0);
			glm::vec4 rect = splitVec4(charNode->first_attribute("rect")->value());
			char code = *charNode->first_attribute("code")->value();

			// m_letters[code].uv = glm::vec4(rect.x/width, 1-rect.y/height-rect.w/height, rect.z/width, rect.w/height);
			m_letters[code].uv = glm::vec4(rect.x / _width, 1 - rect.y / _height - rect.w / _height, rect.z / _width, rect.w / _height);
			m_letters[code].size = glm::vec2(rect.zw());
			m_letters[code].offset = offset.xy();

			for (rapidxml::xml_node<> *kerningNode = charNode->first_node(); kerningNode; kerningNode = kerningNode->next_sibling()){
				// m_letters[code].kerning[*kerningNode->first_attribute("id")->value()] = std::stoi(kerningNode->first_attribute("advance")->value());
				m_letters[*kerningNode->first_attribute("id")->value()].kerning[code] = std::stoi(kerningNode->first_attribute("advance")->value());
			}


		}
		m_letters[' '].size = m_letters['_'].size; */
	}

	int rgxIntSearch(const std::string &word, const std::string &rgx){
		std::smatch match;
		std::regex regex(rgx);
		std::regex_search(word, match, regex);
		return std::stoi(match[1]);
	}

	std::vector<float> rgxVecFloatSearch(const std::string &_word){
		std::vector<float> out;
		std::smatch match;
		std::regex regex(R"(([\-]?[0-9]+))");
		std::string word = _word;
		while (std::regex_search(word, match, regex)){
			out.push_back(std::stof(match[1]));
			word = match.suffix().str();
		}
		return out;
	}

	std::vector<int> rgxVecIntSearch(const std::string &_word){
		std::vector<int> out;
		std::smatch match;
		std::regex regex(R"(([\-]?[0-9]+))");
		std::string word = _word;
		while (std::regex_search(word, match, regex)){\
			out.push_back(std::stoi(match[1]));
			word = match.suffix().str();
		}
		return out;
	}

	void FontRenderer::insertFont(const std::string &word, float U, float V){
		auto &&vec = rgxVecFloatSearch(word);
		int16_t id = vec[0];
		float x = vec[1]; // ~
		float y = vec[2]; // distance from top to top of letter
		float z = vec[3]; // width
		float w = vec[4]; // height
		float offset_x = vec[5];
		float offset_y = vec[6];
		float advance = vec[7];
		m_letters[id].uv = glm::vec4(x / U, 1.f - y / V - w / V, z / U, w / V);
		m_letters[id].size = glm::vec2(z, w);
		m_letters[id].offset = glm::vec2(offset_x, -offset_y);
		m_letters[id].advance = advance;
	}
	void FontRenderer::insertSymbol(const std::string &word, float U, float V){
		auto &&vec = rgxVecFloatSearch(word);
		int16_t id = vec[0];
		float x = vec[1]; // ~
		float y = vec[2]; // distance from top to top of letter
		float z = vec[3]; // width
		float w = vec[4]; // height
		float offset_x = vec[5];
		float offset_y = vec[6];
		float advance = vec[7];
		m_symbols[id];
		m_symbols[id].uv = glm::vec4(x / U, 1.f - y / V - w / V, z / U, w / V);
		m_symbols[id].size = glm::vec2(z, w);
		m_symbols[id].offset = glm::vec2(offset_x, -offset_y);
		m_symbols[id].advance = advance;

	}
	void FontRenderer::loadKerning(const std::string &word){
		auto &&vec = rgxVecIntSearch(word);
		m_letters[vec[1]].kerning[vec[0]] = vec[2];
	}
	void FontRenderer::loadFontInfoFNT(std::string name){
		std::string path = "../res/fonts/";
		std::ifstream file;
		file.open(path + name + ".fnt", std::ios::in);
		std::string infoFaceLine, commonLine, countLine, buff;
		getline(file, infoFaceLine);
		getline(file, commonLine);
		getline(file, buff); // non used
		getline(file, countLine);
		//
		float sizeU = (float)rgxIntSearch(commonLine, R"(scaleW=([\-]?[0-9]+))");
		float sizeV = (float)rgxIntSearch(commonLine, R"(scaleH=([\-]?[0-9]+))");
		int charCount = rgxIntSearch(countLine, R"(count=([\-]?[0-9]+))");

		std::vector<std::string> lines;
		lines.resize(charCount);

		// this->height = rgxIntSearch(commonLine, R"(base=([\-]?[0-9]+))");;
	this->m_height = rgxIntSearch(commonLine, R"(lineHeight=([\-]?[0-9]+))");

		for (int i = 0; i < charCount; i++)
			getline(file, lines[i]);
		for(auto &it : lines)
			insertFont(it, sizeU, sizeV);
		getline(file, buff);
		// int	kerningCount = rgxIntSearch(commonLine, R"(count=([\-]?[0-9]+))");
		// lines.resize(kerningCount);

		// for (int i = 0; i < kerningCount; i++)
			// getline(file, lines[i]);
		// for(auto &it : lines)
			// loadKerning(it);


		file.close();
	}

	void FontRenderer::loadFontInfoSYM(std::string name){
		std::string path = "../res/fonts/";
		std::ifstream file;
		file.open(path + name + ".fnt", std::ios::in);
		std::string infoFaceLine, commonLine, countLine, buff;
		getline(file, infoFaceLine);
		getline(file, commonLine);
		getline(file, buff); // non used
		getline(file, countLine);
		//
		float sizeU = (float)rgxIntSearch(commonLine, R"(scaleW=([\-]?[0-9]+))");
		float sizeV = (float)rgxIntSearch(commonLine, R"(scaleH=([\-]?[0-9]+))");
		int charCount = rgxIntSearch(countLine, R"(count=([\-]?[0-9]+))");
		std::vector<std::string> lines;
		lines.resize(charCount);
		// this->height = rgxIntSearch(commonLine, R"(base=([\-]?[0-9]+))");;
		this->m_height = rgxIntSearch(commonLine, R"(lineHeight=([\-]?[0-9]+))");;

		for (int i = 0; i < charCount; i++)
			getline(file, lines[i]);
		for(auto &it : lines)
			insertSymbol(it, sizeU, sizeV);
	}
}