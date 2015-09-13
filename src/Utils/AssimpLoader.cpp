#include "Includes.h"
#include "Utils.h"
#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/scene.h> // Output data structure
#include <assimp/postprocess.h> // Post processing flags
#include "MeshInfo.h"
#include "AssimpLoader.h"
typedef std::vector<unsigned int> ints;
typedef std::vector<float> floats;
extern glm::mat4 identity;
// bool AssimpCacheData = true;
// bool AssimpCacheData = false;
bool AssimpCacheData = false;

bool sortAlongY(const aiBone *a, const aiBone *b){
	// cout<<a->mOffsetMatrix[1][3]<<" "<<b->mOffsetMatrix[1][3]<<endl;
	return a->mOffsetMatrix[1][3] < b->mOffsetMatrix[1][3];
}
bool sortAlongX(const aiBone *a, const aiBone *b){
	// cout<<a->mOffsetMatrix[1][3]<<" "<<b->mOffsetMatrix[1][3]<<endl;
	return a->mOffsetMatrix[0][3] > b->mOffsetMatrix[0][3];
}


Bone::Bone(){
	bind_pose = identity;
	axis = glm::vec4(0,0,1,0);
}

void Bone::operator = (const Bone &in){
	bind_pose = in.bind_pose;
	fixed_bind_pose = in.fixed_bind_pose;
	axis = in.axis;
	name = in.name;
}

void Bone::fix(){
	btVector3 a(bind_pose[3][0],bind_pose[3][2],bind_pose[3][1]);
	fixed_bind_pose = bind_pose;
	fixed_bind_pose[3][0] = a[0];
	fixed_bind_pose[3][1] = a[1];
	fixed_bind_pose[3][2] = a[2];
	bind_pose = fixed_bind_pose;
	bind_pose = glm::translate(identity, glm::vec3(a[0], -a[1], a[2]));
	fixed_bind_pose = glm::translate(identity, glm::vec3(a[0], -a[1], a[2]));
	// bind_pose[3][2] = fixed_bind_pose[3][1];
	// bind_pose[3][1] = fixed_bind_pose[3][2];
}
btVector3 Bone::getPosition(){
	return btVector3(fixed_bind_pose[3][0],fixed_bind_pose[3][1],fixed_bind_pose[3][2]);
	// return btVector3(bind_pose[3][2],bind_pose[3][0],bind_pose[3][1]);
}
glm::mat4 Bone::inverseBindPose(){
	glm::vec3 a(fixed_bind_pose[3][0],fixed_bind_pose[3][1],fixed_bind_pose[3][2]);
	// cout<<"AAA: "<<a[0]<<" "<<a[1]<<" "<<a[2]<<endl;
	// cout<<"-AA: "<<-a[0]<<" "<<-a[1]<<" "<<-a[2]<<endl;
	// fixed_bind_pose[3][0] = -a[0];
	// fixed_bind_pose[3][1] = -a[1];
	// fixed_bind_pose[3][2] = -a[2];
	return glm::inverse(bind_pose);
}

void SubMesh::clear(){
	tangents.clear();
	normals.clear();
	bitangents.clear();
	texcoords.clear();
	positions.clear();
	indices.clear();
}


void AssimpLoader::init(){
	Normals.clear();
	Bitangents.clear();
	Tangents.clear();
	Texcoords.clear();
	Positions.clear();
	Indices.clear();

	Normals.reserve(800000);
	Tangents.reserve(800000);
	Bitangents.reserve(800000);
	Texcoords.reserve(400000);
	Positions.reserve(800000);
	Indices.reserve(800000);
}
void AssimpLoader::openFile(string file_name){
	if(AssimpCacheData)
		return;
	//cerr<<file_name<<"\n";
	scene = importer.ReadFile(file_name.c_str(),
		aiProcess_CalcTangentSpace
		// |aiProcess_Triangulate
		// |aiProcess_ImproveCacheLocality
		// |aiProcess_PreTransformVertices
		// |aiProcess_SortByPType
		|aiProcess_JoinIdenticalVertices
		// |aiProcess_FlipWindingOrder
		// |aiProcess_MakeLeftHanded

		);

	if(!scene){
		//cerr<<"assimp siem wywali³ na: "<<file_name<<".\nwhy?\n"<< importer.GetErrorString()<<endl;
		return;
	}

}
void AssimpLoader::close(){
	if(AssimpCacheData)
		return;
	importer.FreeScene();
}
SubMesh* AssimpLoader::loadRigged(aiString name){
	cerr<<"\tLoading rigged subMesh: "<<name.C_Str()<<endl;


	SubMesh *out = new SubMesh;
	if(AssimpCacheData){
		loadCache<float>((string)name.C_Str()+"t", out->texcoords);
		loadCache<float>((string)name.C_Str()+"n", out->normals);
		loadCache<float>((string)name.C_Str()+"ta", out->tangents);
		loadCache<float>((string)name.C_Str()+"b", out->bitangents);
		loadCache<float>((string)name.C_Str()+"p", out->positions);
		loadCache<unsigned int>((string)name.C_Str()+"i", out->indices);
		return out;
	}

	int count = scene->mNumMeshes;
	aiMesh *mesh;
	mesh = NULL;
	// wyszukiwanie w³aœciwego mesha
	for(int i=0; i<count; i++){
		if(scene->mMeshes[i]->mName == name){
			mesh = scene->mMeshes[i];
			break;
		}
	}

	if(mesh == NULL){
		//cerr<<"Error: no mesh";
		return NULL;
	}


	int mNumVertices = mesh->mNumVertices;
	int mPrimitiveTypes = mesh->mPrimitiveTypes;

	out->texcoords.resize(mNumVertices*2);
	out->normals.resize(mNumVertices*4);
	out->tangents.resize(mNumVertices*4);
	out->bitangents.resize(mNumVertices*4);
	out->positions.resize(mNumVertices*4);

	// ³adujemy wierzcho³ki
	for(int i=0, j=0, k=0, m = 0; i<mNumVertices; i++){
		out->texcoords [k++] = mesh->mTextureCoords[0][i].x;
		out->texcoords [k++] = mesh->mTextureCoords[0][i].y;

		out->normals [j] = mesh->mNormals[i].x;
		out->tangents [j] = mesh->mTangents[i].x;
		out->bitangents [j] = mesh->mBitangents[i].x;
		out->positions [j++] = mesh->mVertices[i].x;

		out->normals [j] = mesh->mNormals[i].y;
		out->tangents [j] = mesh->mTangents[i].y;
		out->bitangents [j] = mesh->mBitangents[i].y;
		out->positions [j++] = mesh->mVertices[i].y;

		out->normals [j] = mesh->mNormals[i].z;
		out->tangents [j] = mesh->mTangents[i].z;
		out->bitangents [j] = mesh->mBitangents[i].z;
		out->positions [j++] = mesh->mVertices[i].z;

		out->tangents [j] = 0;
		out->bitangents [j] = 0;
		out->normals [j] = 0;
		out->positions [j++] = 0;
	}

	// wrzucamy indeksy trójk¹tów
	out->indices.reserve(10);
	for(int i=0; i < mesh->mNumFaces; i++){
		for(int w=0; w<mesh->mFaces[i].mNumIndices; w++){
			out->indices.push_back(mesh->mFaces[i].mIndices[w]);
		}
	}

	// wierzcho³kom przypisujemy numer koœci, te którym nic nie bêdzie przypisane maj¹ 0, czyli przypisane s¹ do rodzica
	// cout<<"Bones: "<<mesh->mNumBones<<endl;
	for(int i= 0; i<mesh->mNumBones; i++){
		//cerr<<"\t\t\t"<<mesh->mBones[i]->mName.C_Str()<<endl;
		for(int j=0; j<mesh->mBones[i]->mNumWeights; j++){
			auto tmp = mesh->mBones[i]->mWeights[j];
			out->positions[(tmp.mVertexId)*4+3] = i+1;
			// cout<<"\t"<<i+1<<endl;
		}
	}
	saveCache<float>((string)name.C_Str()+"t", out->texcoords);
	saveCache<float>((string)name.C_Str()+"n", out->normals);
	saveCache<float>((string)name.C_Str()+"ta", out->tangents);
	saveCache<float>((string)name.C_Str()+"b", out->bitangents);
	saveCache<float>((string)name.C_Str()+"p", out->positions);
	saveCache<unsigned int>((string)name.C_Str()+"i", out->indices);
	return out;
}
SubMesh* AssimpLoader::load(aiString name){
	cerr<<"\tLoading subMesh: "<<name.C_Str()<<endl;

	SubMesh *out = new SubMesh;
	if(AssimpCacheData){
		loadCache<float>((string)name.C_Str()+"t", out->texcoords);
		loadCache<float>((string)name.C_Str()+"n", out->normals);
		loadCache<float>((string)name.C_Str()+"ta", out->tangents);
		loadCache<float>((string)name.C_Str()+"b", out->bitangents);
		loadCache<float>((string)name.C_Str()+"p", out->positions);
		loadCache<unsigned int>((string)name.C_Str()+"i", out->indices);
		return out;
	}

	int count = scene->mNumMeshes;
	aiMesh *mesh;
	mesh = NULL;
	for(int i=0; i<count; i++){
		if(scene->mMeshes[i]->mName == name){
			mesh = scene->mMeshes[i];
			break;
		}
	}

	if(mesh == NULL){
		//cerr<<"Error: no mesh";
		return NULL;
	}



	int mNumVertices = mesh->mNumVertices;
	int mPrimitiveTypes = mesh->mPrimitiveTypes;

	out->texcoords.resize(mNumVertices*2);
	out->normals.resize(mNumVertices*4);
	out->tangents.resize(mNumVertices*4);
	out->bitangents.resize(mNumVertices*4);
	out->positions.resize(mNumVertices*4);
//cerr<<"assimp: "<<mNumVertices<<endl;
	for(int i=0, j=0, k=0, m = 0; i<mNumVertices; i++){
		out->texcoords [k++] = mesh->mTextureCoords[0][i].x;
		out->texcoords [k++] = mesh->mTextureCoords[0][i].y;

		out->normals [j] = mesh->mNormals[i].x;
		out->tangents [j] = 1;//mesh->mTangents[i].x;
		out->bitangents [j] = 1;//mesh->mBitangents[i].x;
		out->positions [j++] = mesh->mVertices[i].x;

		out->normals [j] = mesh->mNormals[i].y;
		out->tangents [j] = 0;//mesh->mTangents[i].y;
		out->bitangents [j] = 0;// mesh->mBitangents[i].y;
		out->positions [j++] = mesh->mVertices[i].y;

		out->normals [j] = mesh->mNormals[i].z;
		out->tangents [j] = 0;//mesh->mTangents[i].z;
		out->bitangents [j] = 0;//mesh->mBitangents[i].z;
		out->positions [j++] = mesh->mVertices[i].z;

		out->tangents [j] = 0;
		out->bitangents [j] = 0;
		out->normals [j] = 0;
		out->positions [j++] = 0;
	}
	out->indices.reserve(10);
	for(int i=0; i < mesh->mNumFaces; i++){
		for(int w=0; w<mesh->mFaces[i].mNumIndices; w++){
			out->indices.push_back(mesh->mFaces[i].mIndices[w]);
		}
	}

		saveCache<float>((string)name.C_Str()+"t", out->texcoords);
		saveCache<float>((string)name.C_Str()+"n", out->normals);
		saveCache<float>((string)name.C_Str()+"ta", out->tangents);
		saveCache<float>((string)name.C_Str()+"b", out->bitangents);
		saveCache<float>((string)name.C_Str()+"p", out->positions);
		saveCache<unsigned int>((string)name.C_Str()+"i", out->indices);

	return out;

}
SubMesh* AssimpLoader::loadConvex(aiString name){
	//cerr<<"\tLoading cnvMesh: "<<name.C_Str()<<endl;

	SubMesh *out = new SubMesh;
	if(AssimpCacheData){
		loadCache<float>((string)name.C_Str()+"p", out->positions);
		return out;
	}
	int count = scene->mNumMeshes;
	aiMesh *mesh;
	mesh = NULL;
	for(int i=0; i<count; i++){
		mesh = scene->mMeshes[i];
		if(mesh->mName == name)
			break;
	}

	int mNumVertices = mesh->mNumVertices;
	out->positions.resize(mNumVertices*3);
	for(int i=0, j=0, k=0; i<mNumVertices; i++){
		out->positions [j++] = mesh->mVertices[i].x;
		out->positions [j++] = mesh->mVertices[i].y;
		out->positions [j++] = mesh->mVertices[i].z;
	}

	// cout<<"\t\tcnv_mesh: "<<mesh->mName.C_Str()<<" "<<mesh->mNumVertices<<endl;
	saveCache<float>((string)name.C_Str()+"p", out->positions);
	return out;

}
void AssimpLoader::insert(SubMesh *mesh){
	// zwraca iloœæ do³o¿onych indeksów
	int offset = Positions.size()/4;
	for(auto i=0; i<mesh->indices.size(); i++)
		mesh->indices[i] += offset;

	Normals.insert(Normals.end(), mesh->normals.begin(), mesh->normals.end());
	Texcoords.insert(Texcoords.end(), mesh->texcoords.begin(), mesh->texcoords.end());
	Positions.insert(Positions.end(), mesh->positions.begin(), mesh->positions.end());
	Tangents.insert(Tangents.end(), mesh->tangents.begin(), mesh->tangents.end());
	Bitangents.insert(Bitangents.end(), mesh->bitangents.begin(), mesh->bitangents.end());
	Indices.insert(Indices.end(), mesh->indices.begin(), mesh->indices.end());
	mesh->clear();
	delete mesh;
}
void AssimpLoader::addIndex(SubMesh *mesh, int idx){
		for(int i=3; i<mesh->positions.size(); i+=4)
			mesh->positions[i]+=idx;
}
void AssimpLoader::makeVAO(MeshInfo *info){

	info->VAO = genVAO();
	fillVAO(Positions, Texcoords, Normals, Tangents, Bitangents, Indices, info->VAO);

	Tangents.clear();
	Bitangents.clear();
	Normals.clear();
	Texcoords.clear();
	Positions.clear();
	Indices.clear();
	//cerr<<"VAO DONE\n";
}


