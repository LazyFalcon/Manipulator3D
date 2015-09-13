#pragma once
#include "Utils.h"
template <typename T>
class Bitmap {
public:
	vector <T> m_bitmap;
	int m_size_x;
	int m_size_y;
	int m_size2;
	GLuint m_texID;
	GLenum m_format;
	GLenum m_type;
	int m_scale;
	Bitmap(){}
	void resize(int size_x, int size_y, int _scale = 1){
		m_size_x = size_x;
		m_size_y = size_y;
		m_size2 = size_x*size_y;
		m_scale = _scale;
		m_bitmap = vector<T>(m_size2*_scale, 0);
	}
	void clear(){
		m_bitmap = vector<T>(m_size2, 0);
	}
	T* getData(){
		return m_bitmap.data();
	}
	bool inRange(int x, int y){
		return (m_size_x - x > 0 && m_size_y-y > 0 && x>=0 && y>=0);
	}
	T &operator()(int x, int y){
		return m_bitmap[x+m_size_x*y];
		// return m_bitmap[x*m_size_y+y];
	}
	T &operator()(int x, int y, int z){
		return m_bitmap[x*m_scale+m_size_x*y*m_scale + z];
	}
	
	void save(string name){
		saveCache<T>(name, m_bitmap);
	}
	void load(string name){
		// loadCache<T>(name, m_bitmap);
	}
	
	void genVoidTexture(GLint _internalFormat=GL_R32F, GLenum _format=GL_RED, GLenum _type=GL_FLOAT ){
		m_format = _format;
		m_type = _type;
		glGenTextures(1, &m_texID);
		glBindTexture(GL_TEXTURE_2D, m_texID);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED , m_x, m_y, 0,GL_RED, GL_FLOAT, NULL);
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat , m_size_x, m_size_y, 0,_format, _type, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void updateTexture(){
		glBindTexture(GL_TEXTURE_2D, m_texID);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_size_x, m_size_y, m_format, m_type, (void*)m_bitmap.data());
			glBindTexture(GL_TEXTURE_2D, 0);
		
	}
};
