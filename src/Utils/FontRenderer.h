#pragma once
#include "Includes.h"
#include <iterator>
namespace UI {
	extern float LastTextLength;
	extern float LastTextHeight;

	struct LetterInfo {
		glm::vec4 uv; 		// [0-1]
		glm::vec2 size; 	// px
		glm::vec2 offset;	// px
		float 		advance;// px
		std::unordered_map <char, float> kerning;
	};
	struct UnicodeSymbolInfo {
		glm::vec4 uv; 		// [0-1]
		glm::vec2 size; 	// px
		glm::vec2 offset;	// px
		float 		advance;// px
	};

	struct RenderData {
		int ID;
		int m_size;
		glm::vec2 size;
		std::vector <glm::vec2> positions; // dolny lewy róg znaku
		std::vector <glm::vec2> sizes; // dolny lewy róg znaku
		std::vector <glm::vec4> uvs; // pozycja uv + d³ugoœæ w uv
		std::vector <HexColor> colors; // kolor
		std::vector <float> total; // wszystko *reinterpret_cast<float*>(&v)
	};

	struct TextInfo {
		std::string m_string;
		glm::vec2 m_position;
		int m_caretPosition;
		HexColor m_color;
		TextInfo();
		TextInfo(glm::vec2 p, const std::string &s, HexColor c, int k = -1);
		void operator =(const TextInfo &t);
	};

	class FontRenderer {
	public:
		std::string m_name;
		// std::string m_associatedSymbols;
		FontRenderer *m_associatedSymbols;
		RenderData 	m_renderData[5];
		int 				m_height;
		int 				m_lastTextSize{ 0 };
		LetterInfo 	m_letters[256];
		std::unordered_map<char16_t, UnicodeSymbolInfo>m_symbols;
		bool 				m_empty[5] = { true, true, true, true, true };

		~FontRenderer();
		float getLen(const std::string &text);

		void clear(int layer);
		void loadFontInfoXML(std::string name, int _width, int _height);
		void loadFontInfoFNT(std::string name);
		void loadFontInfoSYM(std::string name);
		float generateText(const std::string &text, glm::vec2 position, HexColor color, int caretPosition = -1);
		float generateText(const std::u16string &text, glm::vec2 position, HexColor color, int caretPosition = -1);
		float genSingleSymbol(char16_t symbol, glm::vec2 position, HexColor color);
		void move(int x, int y);
		void insertFont(const std::string &word, float U, float V);
		void insertSymbol(const std::string &word, float U, float V);
		void loadKerning(const std::string &word);
		void placeCaret(const std::string &text, glm::vec2 position, HexColor color, int caretPosition);
	};

	glm::vec2 splitVec2(std::string toSplit);
	glm::vec4 splitVec4(std::string toSplit);
	struct Font {
		uint32_t texID;
		FontRenderer *generator;
		float push(const std::string &text, const glm::vec2 &pos, HexColor color, int key = -1);
		float push(const std::u16string &text, const glm::vec2 &pos, HexColor color, int key = -1);
	};
	extern std::unordered_map<std::string, Font>	fonts;

}