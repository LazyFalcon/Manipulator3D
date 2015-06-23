#include "CFGparser.h"
#include <Utils/Includes.h>
#include <regex>
#include <boost/algorithm/string/trim.hpp>
using namespace std;

namespace CFG {
	string rFloat = "(-?[0-9]+\\.?[0-9]*)";
	bool	incomingSequence = false;
	int 	bufferPos = 0;
	bool	notEscape = false;
	bool	breakSeq = false;
	std::vector <std::string> fileBuff;

	glm::vec4 Node::asVec31(){
		return glm::vec4(cacheFloat[0],cacheFloat[1],cacheFloat[2],1);
	}
	glm::vec4 Node::asVec30(){
		return glm::vec4(cacheFloat[0],cacheFloat[1],cacheFloat[2],0);
	}
	glm::vec2 Node::asVec2(){
		return glm::vec2(cacheFloat[0],cacheFloat[1]);
	}
	glm::vec3 Node::asVec3(){
		return glm::vec3(cacheFloat[0],cacheFloat[1],cacheFloat[2]);
	}
	glm::vec4 Node::asVec4(){
		return glm::vec4(cacheFloat[0],cacheFloat[1],cacheFloat[2],cacheFloat[3]);
	}
	glm::quat Node::asQuat(){
		return glm::quat(cacheFloat[0],cacheFloat[1],cacheFloat[2],cacheFloat[3]);
	}
	btVector3 Node::asbtVec3(){
		return btVector3(cacheFloat[0],cacheFloat[1],cacheFloat[2]);
	}

	Bezier<float> Node::asBezier(){
		vector<float>p(cacheFloat.begin(),cacheFloat.begin()+cacheFloat.size()/2);
		vector<float>w(cacheFloat.begin()+cacheFloat.size()/2+1, cacheFloat.end());
		Bezier<float> bezier(p,w);
		return bezier;

	}


	int numOfSpaces(const string &s){
		int len = s.size();
		for(int i=0; i<len; i++){
			if(s[i]!=' ' && s[i]!= '-')
				return i;
			if(s[i] == '-')
				incomingSequence = true;
		}
		return 0;
	}

	void recursivelyOut(Node &node, const string &offset, bool seq = true){
		if(node.Map.size()>0){
			if(seq)
				cout<<endl;
			for(auto &it : node.Map){
				if(seq)
					cout<<offset;
				cout<<it.first+": ";
				recursivelyOut(it.second, offset+"  ");
				// cout<<endl;
				seq = true;
			}
		}
		else if(node.Vector.size() > 0){
			cout<<endl;
			for(auto &it : node.Vector){
				cout<<offset+"- ";
				recursivelyOut(it, offset+"  ", false);
				// cout<<endl;
			}
		}
		else if(node.value=="" && node.Vector.size()==0)
			cout<<endl;
		else
			cout<<node.value<<endl;

	}
	void recursivelySave(fstream &file, Node &node, const string &offset, bool seq = true){

		if(node.Map.size()>0){
			// if(node.value != "")
				// node.prop("@default", node.value);

			if(seq)
				file<<endl;
			for(auto &it : node.Map){
				if(seq)
					file<<offset;
				if(it.first == "")
					continue;
				file<<it.first+":";
				recursivelySave(file, it.second, offset+"  ");
				// file<<endl;
				seq = true;
			}
		}
		else if(node.Vector.size() > 0){
			file<<endl;
			for(auto &it : node.Vector){
				file<<offset+"- ";
				recursivelySave(file, it, offset+"  ", false);
				// file<<endl;
			}
		}
		else if(node.value=="")
			file<<endl;
		else
			if(node.value.size() > 0)
				file<<" "<<node.value<<endl;

	}

	void Node::split(){
		regex rf("(-?[0-9]+\\.?[0-9]*)");
		regex_iterator<string::iterator> rit ( value.begin(), value.end(), rf );
		regex_iterator<string::iterator> rend;
		cacheFloat.clear();
		while (rit!=rend) {
			cacheFloat.push_back(stof(rit->str()));
			++rit;
		}
		// if(cacheFloat.size() > 0){
			// cout<<value<<"  ";
			// for(auto it : cacheFloat)
				// cout<<it<<" ";
			// cout<<endl;
		// }

	}

	Node recursivelyIn(int spaces, bool seq = false){
		regex	param1("\\s*-?\\s*(@?#?\\w+):");
		regex	param2("\\s*-?\\s*(@?#?\\w+):\\s+([\\S+\\s]*)");
		regex	param3 ("\\s*-?\\s*([\\S+\\s*^:]+)");
		regex	sequenceRgx("\\s*-.*:");
		smatch 	s;
		Node node;
		node.userInfo = 0;
		int depth;
		string tmp = "";
		bool incSeq = false;
		while(u32(bufferPos) < fileBuff.size()){
			depth = numOfSpaces(fileBuff[bufferPos]);
			if(depth > spaces){
				if(incomingSequence){
					// cout<<tmp;
					// cout<<depth;
					incSeq = true;
					incomingSequence = false;
				}
				if(!incSeq){
					node.prop(tmp, recursivelyIn(depth));
				}
				else {
					// cout<<"\nLoading seq to: "+tmp+"\n";
					notEscape = true;
					Node in = recursivelyIn(depth, true);
					if(in.value != "" || in.isMap() || in.isSeq())
						node[tmp].push_back(in);
					if(breakSeq)
						breakSeq = incSeq = false;
				}
				continue;
			}
			else if(depth < spaces){
				breakSeq = true;
				if(node.Map.find("@default") != node.Map.end()){
					node.value = node["@default"].value;
					node.split();
				}
				return node;
			}
			else if(incomingSequence && !notEscape){
				incomingSequence = false;
				// cerr<<"\nseq ";
				return node;
			}
			// else
				// cerr<<depth<<"-";
			incomingSequence = false;
			notEscape = false;

			string buff = fileBuff[bufferPos];
			boost::trim(buff);
			if(regex_search (buff, s, param2)){
				node.prop(s[1], s[2]);
			}
			else if(regex_search (buff, s, param1)){
				tmp = s[1];
			}
			else if(regex_search (buff, s, param3)){
				node.prop(s[1]);
			}

			bufferPos++;
		}
		return node;
	}

	Node Load(const string &filename){
		Node node;
		fstream file;
		string buff;
		file.open(filename);
			if (file.is_open()){
				cout<<"reading: "+filename<<endl;
				while(!file.eof()){
					getline(file, buff);
					fileBuff.push_back(buff);
				}
			}
		else
			cout<<"file error"<<endl;
		file.close();

		node = recursivelyIn(numOfSpaces(fileBuff[0]));

		splitAll(node);

		return node;
	}

	void Save(const string &filename, Node &node){
		fstream file;
		file.open(filename, ios::out | ios::trunc);
			if (file.is_open()){
				recursivelySave(file, node, "");
			}
			else
				cout<<filename<<" error\n";
		file.close();
	}

	void splitAll(Node &node){
		node.split();
			for(auto &it : node.Vector)
				splitAll(it);

			for(auto &it : node.Map)
				splitAll(it.second);

	}

	void goThroughAllChildrenRecurs(Node &node, function<bool(Node&)> fun){
		// cout<<node.value<<endl;

		if(fun(node))
			return;
		if(node.Vector.size()>0)
			for(auto &it : node.Vector){
				goThroughAllChildrenRecurs(it, fun);
			}
		else if(node.Map.size()>0){
			for(auto &it : node.Map){
				// cout<<it.first<<endl;
				goThroughAllChildrenRecurs(it.second, fun);
			}
		}

	}
}
