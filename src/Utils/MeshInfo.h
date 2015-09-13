#pragma once
class MeshInfo {
public:
	MeshInfo(){
		start_index = 0;
		end_index = 0;
	}
	int start_index, end_index;
	int count;
	vector <glm::mat4> matrices;
	// static GLuint VAO;
	// static GLuint IBO;
	uint32_t VAO;
	uint32_t IBO;
	uint32_t VBO[5];
	//Id textury
	//Id shadera
	void operator = (const MeshInfo &info){
		start_index = info.start_index;
		end_index = info.end_index;
		VAO = info.VAO;
		IBO = info.IBO;
		VBO[0] = info.VBO[0];
		VBO[1] = info.VBO[1];
		VBO[2] = info.VBO[2];
		VBO[3] = info.VBO[3];
		VBO[4] = info.VBO[4];
	}


};

// GLuint MeshInfo::VAO;
// GLuint MeshInfo::IBO;