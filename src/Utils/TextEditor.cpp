extern bool GetInput;
extern const float pi;
extern int g_UILayer;

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
		// *(float*)valuePointer = std::stof(currentString);
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
void TextEditor::input(int key, int action, int mod){
	char input = 0;

	if(key == GLFW_KEY_ENTER && action == GLFW_PRESS && !GetInput)
		GetInput = true;
	else if(GetInput && (action == GLFW_PRESS || action == GLFW_REPEAT)){

		// if(key == GLFW_KEY_ENTER)
			// finishEdition();
		if(key == GLFW_KEY_ESCAPE)
			breakEdition();

		if(mod == GLFW_MOD_CONTROL ){
			if(key == GLFW_KEY_DELETE  && m_caretPosition<currentString.size()){
				currentString.erase(m_caretPosition);
				m_caretPosition = glm::clamp(m_caretPosition, 0, (int)currentString.size());
			}
			if(key == GLFW_KEY_BACKSPACE && m_caretPosition>0){
				currentString.erase(0, m_caretPosition);
				m_caretPosition = 0;
				m_caretPosition = glm::clamp(m_caretPosition, 0, (int)currentString.size());
			}
			// unsigned found = str.rfind(key);
			// if (found!=std::string::npos)
				// str.replace (found,key.length(),"seventh");
			if(key == GLFW_KEY_RIGHT)
				m_caretPosition = currentString.size();
			if(key == GLFW_KEY_LEFT)
				m_caretPosition = 0;
		}
		else {
			if(key == GLFW_KEY_DELETE  && m_caretPosition<currentString.size()){
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


template std::string prettyPrint<int32_t>	(int32_t &value, int base);
template std::string prettyPrint<uint32_t>(uint32_t &value, int base);
template std::string prettyPrint<int64_t>	(int64_t &value, int base);
template std::string prettyPrint<uint64_t>(uint64_t &value, int base);
template IMGUI& IMGUI::edit<int32_t>	(int32_t &value, int base);
template IMGUI& IMGUI::edit<uint32_t>	(uint32_t &value, int base);
template IMGUI& IMGUI::edit<int64_t>	(int64_t &value, int base);
template IMGUI& IMGUI::edit<uint64_t>	(uint64_t &value, int base);
template void IMGUI::processTE<int32_t>	(int32_t &value);
template void IMGUI::processTE<uint32_t>(uint32_t &value);
template void IMGUI::processTE<int64_t>	(int64_t &value);
template void IMGUI::processTE<uint64_t>(uint64_t &value);
template void IMGUI::processTE<int32_t>	(int32_t &value, int base);
template void IMGUI::processTE<uint32_t>(uint32_t &value, int base);
template void IMGUI::processTE<int64_t>	(int64_t &value, int base);
template void IMGUI::processTE<uint64_t>(uint64_t &value, int base);

template void TextEditor::setValueToEdit<int32_t>	(int32_t &value, int base);
template void TextEditor::setValueToEdit<uint32_t>(uint32_t &value, int base);
template void TextEditor::setValueToEdit<int64_t>	(int64_t &value, int base);
template void TextEditor::setValueToEdit<uint64_t>(uint64_t &value, int base);

template void TextEditor::setValueToEdit<int32_t>	(int32_t &value);
template void TextEditor::setValueToEdit<uint32_t>(uint32_t &value);
template void TextEditor::setValueToEdit<int64_t>	(int64_t &value);
template void TextEditor::setValueToEdit<uint64_t>(uint64_t &value);
template void TextEditor::setValueToEdit<float>(float &value);
template void TextEditor::setValueToEdit<std::string>(std::string &value);

template bool TextEditor::compare<float>(float &value);
template bool TextEditor::compare<int32_t>(int32_t &value);
template bool TextEditor::compare<uint32_t>(uint32_t &value);
template bool TextEditor::compare<int64_t>(int64_t &value);
template bool TextEditor::compare<uint64_t>(uint64_t &value);
template bool TextEditor::compare<std::string>(std::string &value);








