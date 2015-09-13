#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#define GLM_SWIZZLE
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SSE2
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <functional>
#include <algorithm>
#include <random>
#include <btBulletDynamicsCommon.h>
#include "Utils/Bezier.h"

namespace CFG {
	extern string 	rFloat;
	extern bool 		incomingSequence;
	extern std::vector <std::string> fileBuff;
	extern int 			bufferPos;
	extern bool			notEscape;
	extern bool 		breakSeq;

	struct Node {
		std::string value;
		std::map <std::string, Node> Map;
		std::vector <Node> Vector;
		int userInfo;
		vector<float>cacheFloat;
		Node& operator [] (Node &node){
			return Map[node.value];
		}
		Node& operator [] (const std::string &s){
			return Map[s];
		}
		Node& operator [] (int i){
			return Vector[i];
		}
		int size(){
			return max(Map.size(), Vector.size());
		}
		bool has(const string &prop){
			auto it = Map.find(prop);
			return it != Map.end();
		}
		void prop(const std::string &p, const std::string &v){
			Map.emplace(p, Node {v, std::map <std::string, Node>{}, std::vector <Node>{}, 0});
		}
		void prop(const std::string &p){
			value = p;
		}
		void prop(const std::string &p, const Node &n){
			// Map.emplace(p, n);
			Map[p] = n;
		}
		void set(string s){
			value = s;
			split();
			if(Map.size() > 0){
				Map["@default"] = s;
				Map["@default"].split();
			}
		}

		bool find(string val){
			if(Vector.size() > 0){
				for(auto &it : Vector){
					if(it.value == val)
						return true;
				}
				return false;
			}
			else if(Map.size() > 0)
				return Map.find(val) != Map.end();
			else
				return value == val;
		}

		std::map<std::string, Node>::iterator begin(){
			return Map.begin();
		}
		std::map<std::string, Node>::iterator end(){
			return Map.end();
		}
		void push_back(const Node n){
			Vector.push_back(n);
		}
		void push_back(string s){
			Vector.push_back(Node {s});
		}

		void split();

		void operator = (const std::string &s){
			value = s;
		}
		void operator = (int i){
			value = to_string(i);
		}
		void operator = (float f){
			value = to_string(f);
		}
		void operator = (glm::vec2 v){
			cacheFloat.clear();
			cacheFloat.push_back(v[0]);
			cacheFloat.push_back(v[1]);
			value = "["+to_string(v[0])+", "+to_string(v[1])+"]";
		}
		void operator = (glm::vec3 v){
			cacheFloat.clear();
			cacheFloat.push_back(v[0]);
			cacheFloat.push_back(v[1]);
			cacheFloat.push_back(v[2]);
			value = "["+to_string(v[0])+", "+to_string(v[1])+", "+to_string(v[2])+"]";
		}
		void operator = (glm::vec4 v){
			cacheFloat.clear();
			cacheFloat.push_back(v[0]);
			cacheFloat.push_back(v[1]);
			cacheFloat.push_back(v[2]);
			cacheFloat.push_back(v[3]);
			value = "["+to_string(v[0])+", "+to_string(v[1])+", "+to_string(v[2])+", "+to_string(v[3])+"]";
		}

		bool operator ~ (){
			return ~userInfo;
		}
		bool operator | (int val){
			return userInfo | val;
		}
		void operator |= (int val){ // set bit
			 userInfo |= val;
		}
		void operator ^= (int val){ // toggle bit
			 userInfo ^= val;
		}
		bool operator & (int val){
			return userInfo & val;
		}
		void operator &= (int val){ // reset bit userinfo &= ~bit
			 userInfo &= val;
		}

		bool operator == (const string &s){
			return value == s;
		}
		bool operator == (int s){
			return asInt() == s;
		}
		bool operator == (float s){
			return asFloat() == s;
		}
		bool operator != (const string &s){
			return value == s;
		}
		bool operator != (int s){
			return asInt() == s;
		}
		bool operator != (float s){
			return asFloat() == s;
		}

		bool isSeq(){
			return Vector.size() > 0;
		}
		bool isMap(){
			return Map.size() > 0;
		}
		// by default returns true :P
		bool asBool(){
			return  value != "0" && value != "false" && value != "no" && value != "null";

		}
		int32_t asInt(){
			try {
				return std::stoi(value);
			}
			catch(const std::invalid_argument& ia){
				cerr<<ia.what()<<" when parsing: "<< value <<" as int.\n";
				return 0;
			}
		}
		uint32_t asColor(){
			try {
				return std::stoul(value, nullptr, 16);
			}
			catch(const std::invalid_argument& ia){
				cerr<<ia.what()<<" when parsing: "<< value <<" as uint.\n";
				return 0;
			}
		}
		uint32_t asUInt(){
			try {
				return std::stoul(value);
			}
			catch(const std::invalid_argument& ia){
				cerr<<ia.what()<<" when parsing: "<< value <<" as int.\n";
				return 0;
			}
		}
		uint64_t asuInt64(int base=10){
			try {
				return std::stoul (value, nullptr, base);
			}
			catch(const std::invalid_argument& ia){
				cerr<<ia.what()<<" when parsing: "<< value <<" as int.\n";
				return 0;
			}
		}
		float asFloat(){
			try {
				return std::stof(value);
			}
			catch(const std::invalid_argument& ia){
				cerr<<ia.what()<<" when parsing: "<< value <<"  float.\n";
				return 0.f;
			}
		}
		glm::vec2 asVec2();
		glm::vec4 asVec31();
		glm::vec4 asVec30();
		glm::vec3 asVec3();
		glm::vec4 asVec4();
		glm::quat asQuat();
		btVector3 asbtVec3();
		Bezier<float> asBezier();
		vector<float> asVector(){
			return cacheFloat;
		}
		operator std::string(){
				return value;
		}
		operator glm::vec4(){
				return asVec4();
		}
		// operator glm::quat(){
				// return asQuat();
		// }
		// operator glm::vec3(){
				// return asVec3();
		// }
		// operator glm::vec2(){
				// return asVec2();
		// }
		// operator float(){
				// return asFloat();
		// }
		// operator int(){
				// return asInt();
		// }
		// operator bool(){
				// return  value != "0" && value != "false" && value != "no" && value != "null";
		// }

		};

	void splitAll(Node &node);
	int numOfSpaces(const string &s);
	void recursivelyOut(Node &node, const string &offset, bool seq);

	void recursivelySave(fstream &file, Node &node, const string &offset, bool seq);
	Node recursivelyIn(int spaces, bool seq);
	Node Load(const string &filename);
	void Save(const string &filename, Node &node);

	typedef map <string, Node> Map;
	typedef vector <Node> Seq;

	void goThroughAllChildrenRecurs(Node &node, function<bool(Node&)> fun);
}


