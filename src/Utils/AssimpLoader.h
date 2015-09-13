#pragma once
#include "Includes.h"
#include "Utils.h"
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
typedef std::vector<unsigned int> ints;
typedef std::vector<float> floats;
extern glm::mat4 identity;
extern bool AssimpCacheData;

bool sortAlongY(const aiBone *a, const aiBone *b);
bool sortAlongX(const aiBone *a, const aiBone *b);

struct Bone {
public:
	glm::mat4 bind_pose;
	glm::mat4 fixed_bind_pose;
	glm::vec4 axis;
	string name;
	Bone();
	void operator = (const Bone &in);
	void fix();
	btVector3 getPosition();
	glm::mat4 inverseBindPose();
};
class SubMesh {
public:
	vector <Bone> bones;
	Bone bone;
	floats normals;
	floats texcoords;
	floats positions;
	floats tangents;
	floats bitangents;
	ints indices;

	~SubMesh(){
	}
	void clear();
};
class AssimpLoader {
public:
		floats Normals;
		floats Texcoords;
		floats Positions;
		floats Tangents;
		floats Bitangents;
		ints Indices;
		Assimp::Importer importer;
		const aiScene* scene;

	void loadSubMeshes(){}
	void loadCnvMeshes(){}

	vector<SubMesh*> sub_meshes;

	void init();
	void openFile(string file_name);
	void close();
	SubMesh* loadRigged(aiString name);
	SubMesh* load(aiString name);
	SubMesh* loadConvex(aiString name);
	void insert(SubMesh *mesh);
	void addIndex(SubMesh *mesh, int idx);
	void makeVAO(MeshInfo *info);
};

