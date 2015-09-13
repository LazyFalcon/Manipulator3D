#include "Bitmap.h"

typedef unsigned char Bit;
extern std::unordered_map<string, GLuint>	textures;

bool inRange(int value, int min, int max){
	return value>=min && value<max;
}

class ConstantPlot {
public:

	int x,y,samples;
	glm::vec2 size;
	glm::vec2 position;
	int sample;
	float minval, maxval;
	bool draw { true };

	GLuint textureID;

	void create(glm::vec2 _position, int _x, int _y, int _samples, float _minval, float _maxval){
		position = _position;
		// x = _x;
		x = _samples;
		y = _y;
		samples = _samples;
		minval = _minval;
		maxval = _maxval;
		sample = 0;
		size = glm::vec2(x+3,y);

		Bit zeros[y*(x+3)] = {0};


		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(	GL_TEXTURE_2D, 0, GL_R8, x+3,	y, 0,	GL_RED,	GL_UNSIGNED_BYTE, (void*)zeros);
		glBindTexture(GL_TEXTURE_2D, 0);
		textures["graph"+to_string(textureID)] = textureID;
		firstColumn();
	}
	void firstColumn(){
		Bit column[y];
		for(auto &it : column)
			it = 255;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1,y, GL_RED, GL_UNSIGNED_BYTE, (void*)column);
		glBindTexture(GL_TEXTURE_2D, 0);



		for(auto &it : column)
			it = 0;

		int y0 = fta(0);
		column[y0] = 255;
		column[y-1] = 255;
		column[fta(maxval/2)] = 255;
		column[fta(minval/2)] = 255;
		column[0] = 255;
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 1, 0, 1,y, GL_RED, GL_UNSIGNED_BYTE, (void*)column);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 2, 0, 1,y, GL_RED, GL_UNSIGNED_BYTE, (void*)column);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	void clear(){
		glDeleteTextures(1, &textureID);
	}
	int fta(float value){
		// return glm::clamp((value-minval)/(maxval-minval), minval,maxval)*y;
		return glm::clamp((int)((value-minval)/(maxval-minval)*y), 0,y-1);
	}
	void push(float value){
		Bit column[y] = {0};

		column[fta(0)] = 128;
		column[fta(value)] = 255;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, sample+2, 0, 1,y, GL_RED, GL_UNSIGNED_BYTE, (void*)column);
		glBindTexture(GL_TEXTURE_2D, 0);

		sample = (sample + 1)%samples;
	}
	void push(float value, float value2){
		Bit column[y] = {0};

		// column[fta(0)] = 128;
		column[fta(value)] = 255;
		column[fta(value2)] = 250;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, sample+2, 0, 1,y, GL_RED, GL_UNSIGNED_BYTE, (void*)column);
		glBindTexture(GL_TEXTURE_2D, 0);

		sample = (sample + 1)%samples;
	}
	void push(float value, float value2, float value3){
		Bit column[y] = {0};

		// column[fta(0)] = 128;
		column[fta(value3)] = 100;
		column[fta(value2)] = 250;
		column[fta(value)] = 255;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, sample+2, 0, 1,y, GL_RED, GL_UNSIGNED_BYTE, (void*)column);
		glBindTexture(GL_TEXTURE_2D, 0);

		sample = (sample + 1)%samples;
	}
};

list<ConstantPlot*> g_plots;


class Graph {
	Bitmap<unsigned char> m_bitmap;
	glm::vec4 m_axis;
	glm::vec2 m_scalling;


public:
	GLuint textureID;


	Graph(){}
	void create(int x, int y, glm::vec4 axis){
		m_bitmap.resize(x,y);
		m_scalling.x = abs(axis.y-axis.x)/x;
		m_scalling.y = abs(axis.w-axis.z)/y;
		m_axis = axis;


		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(	GL_TEXTURE_2D, 0, GL_RED, x,	y, 0,	GL_RED,	GL_UNSIGNED_BYTE, (void*)m_bitmap.getData());
		glBindTexture(GL_TEXTURE_2D, 0);
		textures["graph"+to_string(textureID)] = textureID;
	}
	void create(int x, int y, float x_scale, float y_scale){
		m_bitmap.resize(x,y);
		m_scalling.x = x_scale;
		m_scalling.y = y_scale;
		m_axis = glm::vec4(0, x*x_scale, 0, y*y_scale);
	}
	float sizex(){
		return m_bitmap.m_size_x;
	}
	float sizey(){
		return m_bitmap.m_size_y;
	}
	float dx(){
		return m_scalling.x;
	}

	void drawAxis(){
		int x0 = round((0-m_axis.x)/m_scalling.x);
		int y0 = floor((0-m_axis.z)/m_scalling.y);
		for(int i=0; i<m_bitmap.m_size_x; i++)
			m_bitmap(i, y0) = 150;
		for(int i=0; i<m_bitmap.m_size_y; i++)
			m_bitmap(x0, i) = 150;
	}

	void drawGrid(){

	}

	// zak³adamy ¿e data jest ju¿ wyskalowana
	void update(vector<glm::vec2> &data){
		int x=1, y=1, prev_y = 0;
		unsigned char one = 255;
		m_bitmap.clear();
		drawAxis();
		bool first = true;
		for(auto &it : data){
			x = round((it.x-m_axis.x)/m_scalling.x);
			y = floor((it.y-m_axis.z)/m_scalling.y);
			// cout<<y<<endl;

			if(m_bitmap.inRange(x,y)){
				m_bitmap(x,y) = 255;
				for(int i=1; i<y-prev_y && !first; i++)
					m_bitmap(x,prev_y+i) = 255;
				for(int i=1; i<prev_y-y && !first; i++)
					m_bitmap(x,prev_y-i) = 255;

				prev_y = y;
			}
			first = false;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_bitmap.m_size_x, m_bitmap.m_size_y, GL_RED, GL_UNSIGNED_BYTE, (void*)m_bitmap.getData());
		glBindTexture(GL_TEXTURE_2D, 0);

		textures[to_string(textureID)] = textureID;
	}

};