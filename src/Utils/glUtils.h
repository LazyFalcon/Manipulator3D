#pragma once
#include <GL/gl.h>
struct VAOdata;

void genVoidBuffer(uint32_t &buffer, int max_size);
void genBuffer2(uint32_t &buffer, std::vector<glm::vec2> &data);
void genBuffer4(uint32_t &buffer, std::vector<glm::vec4> &data);
void genBuffer4(uint32_t &buffer, glm::vec4 *data, int size);
void updateBuffer1(uint32_t &buffer, float *data, int count);
void updateBuffer2(uint32_t &buffer, std::vector<glm::vec2> &data);
// void updateBuffer4(uint32_t &buffer, std::vector<glm::vec4> &data);
void updateBuffer(uint32_t &buffer, std::vector<glm::vec4> &data);
void updateBuffer3(uint32_t &buffer, std::vector<glm::vec3> &data);

template<typename T>
void updateBuffer(uint32_t &buffer, std::vector<T> &data);

template<typename T>
void updateBuffer(uint32_t &buffer, std::vector<T> &data, uint32_t from, uint32_t to);

template<typename T>
void updateSubBuffer(uint32_t &buffer, std::vector<T> &data, int offset);

template<typename T>
void updateBuffer(uint32_t &buffer, T *data, int size);




void setupBuffer(uint32_t buffer, int numBuffer=0, int dataSize=4, int divisor=0, GLenum type=GL_FLOAT, GLboolean normalize = 0); // 5126 : GL_FLOAT
void glUniform(uint32_t shader, const glm::mat4 &m, const GLint name);
void glUniform(uint32_t shader, const glm::vec4 &v, const GLint name);
void glUniform(uint32_t shader, const glm::vec3 &v, const GLint name);
void glUniform(uint32_t shader, const glm::vec2 &v, const GLint name);
void glUniform(uint32_t shader, const glm::mat4 &m, const char *name);
void glUniform(uint32_t shader, const glm::vec4 &v, const char *name);
void glUniform(uint32_t shader, const glm::vec3 &v, const char *name);
void glUniform(uint32_t shader, const glm::vec2 &v, const char *name);
void glUniform(uint32_t shader, float v, const GLint name);
void glUniform(uint32_t shader, uint32_t v, const GLint name);
void glUniform(uint32_t shader, int v, const GLint name);
void glUniform(uint32_t shader, float v, const char *name);
void glUniform(uint32_t shader, int v, const char *name);
void glUniform(uint32_t shader, uint32_t v, const char *name);

void prepareVAO(uint32_t &VAO);

uint32_t genVAO();


void fillVAO(std::vector<float>&vertices, std::vector<float>&uvs, std::vector<float>&normals, std::vector<int32_t>&indices, VAOdata vaodata);
void fillVAO(std::vector<float>&vertices, std::vector<float>&uvs, std::vector<float>&normals, std::vector<float>&tangents, std::vector<float>&bitnagents, std::vector<uint32_t>&indices, uint32_t VAO);

void deleteBuffers(int, uint32_t*);
void deleteVAO(int, uint32_t*);
