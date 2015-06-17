
typedef std::function<float(float, float, float)> BuffOp;
struct Buff {
	BuffOp op;
	float mod {0};
	int priority;
	uint64_t timeout {0xffffffffffffff}; // in calls
	std::string name;
	float operator()(float curr, float val){
		--timeout;
		if(timeout == 0)
			cout<<name<<" timeout"<<endl;
		// cout<<timeout<<" timeout"<<endl;
		return op(curr, val, mod);
	}
};


typedef std::multimap<int, Buff> BuffList;

BuffOp add = [](float curr, float val, float mod)->float{
	return curr = curr + mod;
};
BuffOp mul = [](float curr, float val, float mod)->float{
	return curr = curr * mod;
};
BuffOp addMul = [](float curr, float val, float mod)->float{
	return curr + mod*val;
};
BuffOp assign = [](float curr, float val, float mod)->float{
	return mod;
};

std::pair<int, Buff> parse(BuffOp op, int priority = 0 ,  const std::string& timeout = "permament"){
	Buff buff;
	if(timeout != "permament"){ // timeout
		std::smatch match;
		std::regex rgxNum 				( R"((-?[0-9]+\.?[0-9]*))");
		std::regex_search(timeout, match, rgxNum);
		float val = std::stof(match[1]);
		if(match.suffix().str() == "calls" || match.suffix().str().size() == 0)
			buff.timeout = (uint64_t)val;
		if(match.suffix().str() == "ms")
			buff.timeout = (uint64_t)val/g_step;
		if(match.suffix().str() == "s")
			buff.timeout = (uint64_t)(val*1000)/g_step;
		if(match.suffix().str() == "min")
			buff.timeout = (uint64_t)(val*1000*60)/g_step;
		cout<<"\tduration: "<<buff.timeout<<endl;
	}
	buff.priority = priority ? priority : 6;
	buff.op = op;
	return std::pair<int, Buff>(buff.priority, buff);
}
std::pair<int, Buff> parse(const std::string& word, const std::string& timeout = "permament"){
	Buff buff;
	std::smatch match;
	std::regex rgxNum 				( R"((-?[0-9]+\.?[0-9]*))");
	if(timeout != "permament"){ // timeout
		std::regex_search(timeout, match, rgxNum);
		float val = std::stof(match[1]);
		if(match.suffix().str() == "calls")
			buff.timeout = (uint64_t)val;
		if(match.suffix().str() == "ms")
			buff.timeout = (uint64_t)val/g_step;
		if(match.suffix().str() == "s")
			buff.timeout = (uint64_t)(val*1000)/g_step;
		if(match.suffix().str() == "min")
			buff.timeout = (uint64_t)(val*1000*60)/g_step;
		cout<<"\tduration: "<<buff.timeout<<endl;
	}
	{ // value & op
		std::regex rgxPlus 				( R"((\+))");
		std::regex rgxPlusMull 		( R"((\+.+%))");
		std::regex rgxMull 				( R"((\*))");
		std::regex rgxOvervrite 	( R"((@))");
		std::regex rgxPercent 		( R"((%))");
		std::regex_search(word, match, rgxNum);
		buff.mod = std::stof(match[1]);

		if(std::regex_search(word, match, rgxPercent)){
			buff.mod *= 0.01f;
		}
		cout<<"\n-- \t"+word<<endl;
		cout<<"\t"<<buff.mod<<endl;
		if(std::regex_search(word, match, rgxPlusMull)){
			buff.op = addMul;
			buff.priority = 4;
			cout<<"\t+x%"<<endl;
		}
		else if(std::regex_search(word, match, rgxPlus)){
			buff.op = add;
			buff.priority = 2;
			cout<<"\t+x"<<endl;
		}
		else if(std::regex_search(word, match, rgxMull)){
			buff.op = mul;
			buff.priority = 3;
			cout<<"\t*x"<<endl;
		}
		else if(std::regex_search(word, match, rgxOvervrite)){
			buff.op = assign;
			buff.priority = 5;
			cout<<"\t@x"<<endl;
		}
		else {
			buff.op = assign;
			buff.priority = 1;
			cout<<"\tx"<<endl;
		}
	}
	buff.name = word;
	return std::pair<int, Buff>(buff.priority, buff);
}

float apply(BuffList &buffs, float value){
	float tmpVal = value;
	// for(auto &it : buffs)
	for(auto it=buffs.begin(); it != buffs.end(); it++){
		tmpVal = it->second(tmpVal, value);
		if(it->second.timeout == 0){
			it = buffs.erase(it);
		}
	}
	return tmpVal;
}

struct Bufferable;
std::list<Bufferable*> BufferableList;
struct Bufferable {
	float 		val;
	float 		current;
	BuffList 	buffs;
	Bufferable(){}
	Bufferable(float v) : val(v), current(v){
		BufferableList.push_back(this);
	}
	void up(){
		current = val;
		for(auto it=buffs.begin(); it != buffs.end(); it++){
			current = it->second(current, val);
			if(it->second.timeout == 0)
				it = buffs.erase(it);
		}
	}
	float& operator()(){
		return current;
	}
	Bufferable& buff(BuffOp op, int priority = 0 ,  const std::string& timeout = "permament"){
		buffs.insert(parse(op, priority, timeout));
		return *this;
	}
	Bufferable& buff(const std::string& word, const std::string& timeout = "permament"){
		buffs.insert(parse(word, timeout));
		return *this;
	}
};

