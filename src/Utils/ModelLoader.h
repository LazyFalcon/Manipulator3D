typedef vector<float> floats;
typedef vector<int> ints;

struct Bone {
	glm::mat4 bind_pose;
	glm::vec4 axis;
	
	Bone(){
		bind_pose = identity;
		axis = glm::vec4(0,0,1,0);
	}
	
	void operator = (const Bone &in){
		bind_pose = in.bind_pose;
		axis = in.axis;
	}
	
};


class SubMesh {
	Bone bone;
	floats normals;
	floats texcoords;
	floats positions;
	ints indices;
	void addBoneIndex(int index){
		for(int i=0; i<positions.size(); i+=4)
			positions[i] = ind;
	}
	void addOffset(int offset){
		for(auto &it : indices)
			it+=offset;
	}
	void clear(){
		normals.clear();
		texcoords.clear();
		positions.clear();
		indices.clear();
	}
};

SubMesh* loadSubMesh(aiMesh *mesh){
	SubMesh *out = new SubMesh;
	
	int mNumVertices = MeshClass->mNumVertices;
	int mPrimitiveTypes = MeshClass->mPrimitiveTypes;

	for(int i=0, j=0, k=0; i<mNumVertices; i++){
		out->texcoords [k++] = MeshClass->mTextureCoords[i][0][0];
		out->texcoords [k++] = MeshClass->mTextureCoords[i][1][0];
		out->normals [j] = MeshClass->mNormals[i][0];
		out->positions [j++] = MeshClass->mVertices[i][0];
		out->normals [j] = MeshClass->mNormals[i][1];
		out->positions [j++] = MeshClass->mVertices[i][1];
		out->normals [j] = MeshClass->mNormals[i][2];
		out->positions [j++] = MeshClass->mVertices[i][2];
		out->normals [j] = 1;
		out->positions [j++] = 1;
	}
	
	for(int i=0; i < MeshClass->mNumFaces; ++i){
			out->indices.insert(out->indices.end()-1, MeshClass->mFaces[i].mIndices, MeshClass->mFaces[i].mIndices+MeshClass->mFaces[i].mNumIndices);
	}

	if(MeshClass->HasBones()){
		// out->bone = new Bone;
		out->bone.axis = glm::vec4(0,0,1,0);
		out->bone.bind_pose = identity;
		for(int k=0; k<4; k++)
		for(int l=0; l<4; l++)
			out->bone.bind_pose[k][l]=MeshClass->mBones[0]->mOffsetMatrix[l][k];
	}

	return out;
}

void loadSubMeshes(vector<SubMesh*> out, string file_name){
	Assimp::Importer importer;
	const aiScene* scene=importer.ReadFile(file_name,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
		);

	if(!scene){
		cout<<"assimp siem wywali³ na: "<<file_name<<".\nwhy?\n"<< importer.GetErrorString()<<endl;
		quit=true;
		return -1;
	}

	int mesh_num=scene->mNumMeshes;
	out.reserve(mesh_num);

	for(int i=0; i<mesh_num; i++){
		out.push_back(loadSubMesh());
	}
}

SubMesh* loadCnvMesh(aiMesh *mesh){}

void loadConvexMeshes()vector<SubMesh*> out, string file_name){
	Assimp::Importer importer;
	const aiScene* scene=importer.ReadFile(file_name,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
		);

	if(!scene){
		cout<<"assimp siem wywali³ na: "<<file_name<<".\nwhy?\n"<< importer.GetErrorString()<<endl;
		quit=true;
		return -1;
	}

	int mesh_num=scene->mNumMeshes;
	out.reserve(mesh_num);

	for(int i=0; i<mesh_num; i++){
		out.push_back(loadCnvMesh());
	}
}
