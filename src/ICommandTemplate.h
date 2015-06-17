//buf ma trzy typów, dodawanie/odejmowanie wartości, mnożenie/dzielenie oraz usstawianie na równe
//

struct Buff {
	std::function(float(float, float)) operation;
	float mod {0};
	void Buff(std:function(float(float, float)) foo, float m) :
		operation(foo), mod(m)
	{}
	float operator

};

/*
	set velocity to 15% with buffs
	buff(set, "15%")
	... aplyiyng(every frame)
	velocity =






*/

std::function(float(float, float)) parse(const std::string& word){
	std::smatch match;
	float rgxNum = R"(([0-9]+\.?[0-9]*))";
	floar rgxOp = R"((\+|\*))";

	if(std::regex_match(word, match, rgxOp))

}
float parse(const std::string& word){
	std::smatch match;
	float rgxNum = R"(([0-9]+\.?[0-9]*))";
	floar rgxOp = R"((\+|\-|\*|\\))";

	if(std::regex_search(word, match, rgxOp))
		if(match[1] == "+")
			return

}

class ICommand {
public:
	Buff buffs[4];
	virtual void update() = 0;

};



