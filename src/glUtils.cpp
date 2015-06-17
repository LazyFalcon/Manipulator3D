#include <Utils/glUtils.h>
void genVoidBuffer(uint32_t &buffer, int max_size){
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*max_size, NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void genBuffer2(uint32_t &buffer, std::vector<glm::vec2> &data){
	glDeleteBuffers(1, &buffer);
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 4*2*data.size(), glm::value_ptr(data[0]), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void genBuffer4(uint32_t &buffer, std::vector<glm::vec4> &data){
		glDeleteBuffers(1, &buffer);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 4*4*data.size(), glm::value_ptr(data[0]), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void genBuffer4(uint32_t &buffer, glm::vec4 *data, int size){
		glDeleteBuffers(1, &buffer);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 4*4*size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void updateBuffer1(uint32_t &buffer, float *data, int count){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*count, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void updateBuffer2(uint32_t &buffer, std::vector<glm::vec2> &data){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2)*data.size(), glm::value_ptr(data[0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void updateBuffer(uint32_t &buffer, std::vector<glm::vec4> &data){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*data.size(), glm::value_ptr(data[0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}
void updateBuffer3(uint32_t &buffer, std::vector<glm::vec3> &data){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*data.size(), glm::value_ptr(data[0]));
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}
void updateBuffer(uint32_t &buffer, glm::vec4 *data, int size){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*size, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

template<typename T>
void updateBuffer(uint32_t &buffer, std::vector<T> &data){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T)*data.size(), data.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

template<typename T>
void updateBuffer(uint32_t &buffer, std::vector<T> &data, uint32_t from, uint32_t to){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T)*(to - from), data.data() + from*sizeof(T));
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

template<typename T>
void updateSubBuffer(uint32_t &buffer, std::vector<T> &data, int offset){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, offset*sizeof(T), sizeof(T)*data.size(), data.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

template<typename T>
void updateBuffer(uint32_t &buffer, T *data, int size){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(T)*size, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}

void setupBuffer(uint32_t buffer, int numBuffer, int dataSize, int divisor, GLenum type, GLboolean normalize){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glEnableVertexAttribArray(numBuffer);
	glVertexAttribPointer(numBuffer, dataSize, type, normalize, 0, (void*)0);
	glVertexAttribDivisor(numBuffer, divisor);
}

void glUniform(uint32_t shader, const glm::mat4 &m, const char *name){
	glUniformMatrix4fv(glGetUniformLocation(shader,name), 1, GL_FALSE,glm::value_ptr(m));
}
void glUniform(uint32_t shader, const glm::vec4 &v, const char *name){
	glUniform4fv(glGetUniformLocation(shader, name), 1, glm::value_ptr(v));
}
void glUniform(uint32_t shader, const glm::vec3 &v, const char *name){
	glUniform3fv(glGetUniformLocation(shader, name), 1, glm::value_ptr(v));
}
void glUniform(uint32_t shader, const glm::vec2 &v, const char *name){
	glUniform2fv(glGetUniformLocation(shader, name), 1, glm::value_ptr(v));
}
void glUniform(uint32_t shader, float v, const char *name){
	glUniform1f(glGetUniformLocation(shader, name), v);
}
void glUniform(uint32_t shader, int v, const char *name){
	glUniform1i(glGetUniformLocation(shader, name), v);
}
void glUniform(uint32_t shader, uint32_t v, const char *name){
	glUniform1i(glGetUniformLocation(shader, name), v);
}

void prepareVAO(uint32_t &VAO){
	glGenVertexArrays(1, &VAO);
}
struct VAOdata {
	uint32_t VAO;
	uint32_t IBO;
	uint32_t VBO[3];
};

uint32_t genVAO(){
	uint32_t vao;
	glGenVertexArrays(1, &(vao));
	return vao;
}
void fillVAO(std::vector<float>&vertices, std::vector<float>&uvs, std::vector<float>&normals, std::vector<int32_t>&indices, VAOdata vaodata){

	// glGenVertexArrays(1, &(vaodata.VAO));
	glBindVertexArray(vaodata.VAO);
	GLuint *modelVBO = vaodata.VBO;
	glGenBuffers(4, modelVBO);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), uvs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer((GLuint)2, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);
	glGenBuffers(1, &(vaodata.IBO));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vaodata.IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(),indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}
void fillVAO(std::vector<float>&vertices, std::vector<float>&uvs, std::vector<float>&normals, std::vector<float>&tangents, std::vector<float>&bitnagents, std::vector<uint32_t>&indices, uint32_t VAO){

	// glGenVertexArrays(1, &(vaodata.VAO));
	glBindVertexArray(VAO);
	GLuint modelVBO[5];
	GLuint IBO;
	glGenBuffers(3, modelVBO);

	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(float), vertices.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer((GLuint)0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, uvs.size()*sizeof(float), uvs.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, modelVBO[2]);
	glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(float), normals.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer((GLuint)2, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribDivisor(0, 0);

	// glBindBuffer(GL_ARRAY_BUFFER, modelVBO[3]);
	// glBufferData(GL_ARRAY_BUFFER, tangents.size()*sizeof(float), tangents.data(), GL_STATIC_DRAW);
		// glEnableVertexAttribArray(3);
		// glVertexAttribPointer((GLuint)3, 4, GL_FLOAT, GL_FALSE, 0, 0);
		// glVertexAttribDivisor(0, 0);

		// glBindBuffer(GL_ARRAY_BUFFER, modelVBO[4]);
	// glBufferData(GL_ARRAY_BUFFER, bitnagents.size()*sizeof(float), bitnagents.data(), GL_STATIC_DRAW);
		// glEnableVertexAttribArray(4);
		// glVertexAttribPointer((GLuint)4, 4, GL_FLOAT, GL_FALSE, 0, 0);
		// glVertexAttribDivisor(0, 0);

	glGenBuffers(1, &(IBO));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t)*indices.size(),indices.data(), GL_STATIC_DRAW);
	glBindVertexArray(0);
}


void deleteBuffers(int a, uint32_t *b){
	glDeleteBuffers(a,b);
}
void deleteVAO(int a, uint32_t *b){
	glDeleteVertexArrays(a,b);
}


