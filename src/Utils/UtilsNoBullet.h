#pragma once
int clamp(int val, int a, int b){
	return max( min( val, b ), a );
}
default_random_engine randEngine(156);
normal_distribution<float> normalDistr(1.f, 0.2f);
float RNG(){
	return glm::clamp(normalDistr(randEngine),0.5f, 1.5f);
}
float RNG(float mod){
	return glm::clamp(normalDistr(randEngine),1.f-mod, 1.f+mod);
}
float sgnRNG(){
	return (1.f-RNG())>=0 ? 1.f : -1.f;
}
int iRNG(int val){
	return (int)(val*RNG());
}

void setColumn(glm::mat4 &mat, int num, glm::vec4 vec){
	mat[num][0] = vec[0];
	mat[num][1] = vec[1];
	mat[num][2] = vec[2];
	mat[num][3] = vec[3];
}


void genVoidBuffer(GLuint &buffer, int max_size){
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*max_size, NULL, GL_DYNAMIC_DRAW);
}
void genBuffer2(GLuint &buffer, vector<glm::vec2> &data){
	glDeleteBuffers(1, &buffer);
	glGenBuffers(1, &buffer);	
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 4*2*data.size(), glm::value_ptr(data[0]), GL_STATIC_DRAW);
}
void genBuffer4(GLuint &buffer, vector<glm::vec4> &data){
		glDeleteBuffers(1, &buffer);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 4*4*data.size(), glm::value_ptr(data[0]), GL_STATIC_DRAW);
}
void genBuffer4(GLuint &buffer, glm::vec4 *data, int size){
		glDeleteBuffers(1, &buffer);
		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, 4*4*size, data, GL_STATIC_DRAW);
}
void updateBuffer1(GLuint &buffer, float *data, int count){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*count, data);
}
void updateBuffer2(GLuint &buffer, vector<glm::vec2> &data){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2)*data.size(), data.data());
}
void updateBuffer4(GLuint &buffer, vector<glm::vec4> &data){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*data.size(), data.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}
void updateBuffer3(GLuint &buffer, vector<glm::vec3> &data){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3)*data.size(), data.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}
void updateBuffer4(GLuint &buffer, glm::vec4 *data, int size){
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec4)*size, data);
	glBindBuffer(GL_ARRAY_BUFFER, 0 );
}
void setupBuffer(GLuint buffer, int numBuffer=0, int dataSize=4, int divisor=0, GLenum type = GL_FLOAT){
	glEnableVertexAttribArray(numBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glVertexAttribPointer(numBuffer, dataSize, type, GL_FALSE, 0, (void*)0);
	glVertexAttribDivisor(numBuffer, divisor);
}

void glUniform(GLuint shader, const glm::mat4 &m, const char *name){
	glUniformMatrix4fv(glGetUniformLocation(shader,name), 1, GL_FALSE,glm::value_ptr(m)); 
}
void glUniform(GLuint shader, const glm::vec4 &v, const char *name){
	glUniform4fv(glGetUniformLocation(shader, name), 1, glm::value_ptr(v)); 
}
void glUniform(GLuint shader, const glm::vec3 &v, const char *name){
	glUniform3fv(glGetUniformLocation(shader, name), 1, glm::value_ptr(v)); 
}
void glUniform(GLuint shader, const glm::vec2 &v, const char *name){
	glUniform2fv(glGetUniformLocation(shader, name), 1, glm::value_ptr(v)); 
}
void glUniform(GLuint shader, float v, const char *name){
	glUniform1fv(glGetUniformLocation(shader, name), 1, &v); 
}


glm::vec4 cross(glm::vec4 const & x, glm::vec4 const & y){
	return glm::vec4(glm::cross(glm::vec3(x.xyz()), glm::vec3(y.xyz())),0);
}
void addLine(glm::vec3 p1, glm::vec3 p2){
	lines[lines_k++]=p1[0];
	lines[lines_k++]=p1[1];
	lines[lines_k++]=p1[2];
	lines[lines_k++]=p2[0];
	lines[lines_k++]=p2[1];
	lines[lines_k++]=p2[2];
	lines_count++;
}
void addLine(glm::vec4 v, glm::vec4 p){
	lines[lines_k++]=p[0];
	lines[lines_k++]=p[1];
	lines[lines_k++]=p[2];
		 p = p + v;
	lines[lines_k++]=p[0];
	lines[lines_k++]=p[1];
	lines[lines_k++]=p[2];
	lines_count++;
}
void addLine(glm::vec4 v, glm::vec3 p){
	lines[lines_k++]=p[0];
	lines[lines_k++]=p[1];
	lines[lines_k++]=p[2];
		 p = p + glm::vec3(v.xyz());
	lines[lines_k++]=p[0];
	lines[lines_k++]=p[1];
	lines[lines_k++]=p[2];
	lines_count++;
}
string string_formatted(double f, int precision=2){
	// std::ostringstream out;
	// out.precision(2);
	// out << std::setprecision(1) << f;
	// out<<f;
	char buffer [10];
	sprintf(buffer, "%.2f", f);
	
	return string(buffer);
}
string string_formatted(float f, int precision=2){
	// std::ostringstream out;
	// out.precision(2);
	// out << std::setprecision(1) << f;
	// out<<f;
	char buffer [10];
	sprintf(buffer, "%.2f", f);
	
	return string(buffer);
}
string to_string(const glm::vec4 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2])+" "+string_formatted(v[3]);
}
string to_string(const glm::vec3 &v){
	return string_formatted(v[0])+" "+string_formatted(v[1])+" "+string_formatted(v[2]);
}

float sign(float x){
	if(x < 0.f)
		return -1.f;
	else if(x > 0.f)
		return 1.f;
	return 0.f;
}

float period(float val, float a, float b){
	if(val >= b)
		val -= b-a;
	else if(val < a)
		val += b-a;
	return val;
}

/*Note:
	PID s³u¿y tylko i wy³¹cznie do sterowania
	wartoœæ ewaluowan¹ wyznaczamy tylko na podstwaie uchybu
	, tzn. podajemy uchyb PID wyznacza sobie resztê(prêdkoœci, przyspieszenia)
*/
#define NM 3
class PID{
protected:
	double delta_e[NM-1];
	double prev_velocity;
	double velocity, accelereation;
	double prev_val;
	void move_e(double e){
		for(int i=NM-1; i>0; i--)
			prev_e[i] = prev_e[i-1];
		prev_e[0] = e;
	}
	void cmpt_delta_e(){
		for(int i=0; i<NM-1; i++)
		delta_e[i] = prev_e[i++] - prev_e[i];
	}
	void cmpt_velocity(){
		prev_velocity = velocity;
		velocity = (prev_e[0] - prev_e[1]) /step;	
		accelereation = (prev_velocity - velocity) /step;
	}
	
	
public:
	double prev_e[NM];
	double K, Td, Ti;
	double current_val;
	double min, max;
	double step;
	double summa;
	//new PID
	
	
	PID(){
		K=0;
		Td=0;
		min=1000; 
		max=1000;
		step=0.001;
		prev_val = 0;
	};
	void init(){
		summa = 0;
		K=0;
		Td=0;
		min=1000; 
		max=1000;
		step=0.001;
		prev_val = 0;
		for(int i=0; i<NM-1; i++)
			delta_e[i] = 0;
		for(int i=NM-1; i>=0; i--)
			prev_e[i] = 0;
			velocity = 0;
	}
	PID(double x, double z){
		K=x;
		Td=z;
		min=1000; 
		max=1000;
		step=0.001;
	};
	void operator=(const PID &p){
		K=p.K;
		Td=p.Td;
		min=p.min;
		max=p.max;
		step=p.step;
		Ti=p.Ti;
	}
	void reload(double x, double z){
		K=x; 
		Td=z;
	};
	double evaluate(double to_reach, double pos){
		current_val=pos;
		double e=to_reach-current_val;
		double out=K*(e+Td*e/step);
		current_val=out;
		return out;
	};
	double compute(double e){
		double out;
		move_e(e);
		cmpt_velocity();
		
		out = K * (e - Td * velocity);
		prev_val = out;
		return out;
	}
	double evaluate(double to_reach){
		double e = to_reach - current_val;
		// move_e(e);
		// cmpt_velocity();
		
		double	out = prev_val + K * (e + Td * velocity*0);
		prev_e[0] = e;
		// if(out > max)
			// out = max;
		// else if(out < min)
			// out = min;
		
		prev_val = out;
		current_val = out;
		return out;
	}
	void reset(){
		prev_val = 0;
		for(int i=0; i<NM-1; i++)
			delta_e[i] = 0;
		for(int i=NM-1; i>=0; i--)
			prev_e[i] = 0;
	}
	
};

double clamp(double x, double minVal, double maxVal){
	return min(max(x, minVal), maxVal);
}

class Inertia2{
private:
	double t1, t2, t3, t4;
	double summa;
	double s_min, s_max, a_min, a_max;
	double step, accu;
	double v, a;
public:
	double pos;
	double direction;
	//t1:t4, s_max, a_max, step
	
	Inertia2(){}
	Inertia2(std::initializer_list <double> args){
		std::initializer_list<double>::iterator it;  // same as: const int* it
		it=args.begin();
		t1=*it++;
		t2=*it++;
		t3=*it++;
		t4=*it++;
		summa=0;
		s_min=-(*it);
		s_max=*it++;
		a_min=-(*it);
		a_max=*it++;
		step=*it;
		accu=0;
		pos=0;
		direction=1;
		a=0; 
		v=0;
	}
	double evaluate(double to_reach, double delta_t){
		accu+=delta_t;
		while(accu>=step){
			accu-=step;
			pos=pos+v*step;
			double e=to_reach-pos;
			summa+=a*step;
			summa=clamp(summa, s_min, s_max);
			v= t3*v + t4*a + summa;
			a= t1*a + t2*e;
			a=clamp(a, a_min, a_max);
		}
		return pos;
	}	
	double evaluate2(double force, double delta_t){
		accu+=delta_t;
		while(accu>=step){
			accu-=step;
			pos=pos+v*step;
			summa+=a*step;
			summa=clamp(summa, s_min, s_max);
			v= t3*v + t4*a + summa;
			a= t1*a + t2*force;
			a=clamp(a, a_min, a_max);
		}
		return pos;
	}
/*
t=0:0.001:4;

a=zeros(length(t),1);
v=zeros(length(t),1);
y=zeros(length(t),1);
summa=0;
to=0.5;
for i=2:1:length(t)
    e=to-y(i-1);
    summa=summa+a(i-1)*0.001;
    summa=max( min( summa,0.1 ), -0.1   );
    a(i)=0.7*a(i-1)+0.7*e;
    a(i)=min(a(i), 0.25);
    a(i)=max(a(i), -0.25);
    v(i)=0.9*v(i-1)+0.1*a(i-1)+summa;
    y(i)=y(i-1)+v(i-1)*0.001 ;
end


*/
};

/*Note:
	zadanime rotatora jest utrzymywanie zadanego k¹ta obrotu

	przy ewaluacji zwraca moment obrotowy(lub impuls)
*/
class Rotator: public PID{
private:
public:
	double 	power;
	double 	angle;
	Rotator(){
		power = 0;
		angle = 0;
	}
	~Rotator(){}
	void setAngle(double a){
		angle = a;
	}
	double evaluate(double current_angle){
		double e = current_angle - angle;
		double k = 1;
		

		if(e > PI)
			e = - 2*PI + e;
		else if(e < -PI)
			e = - e - 2*PI;
		move_e(e);
		cmpt_velocity();
		double	out = k * (K*e + Ti*summa +Td*velocity) *power;
		
		if(out > power)
			out = power;
		else if(out < -power)
			out = -power;
		else 
			summa += e;
			
		prev_e[0] = e;
		prev_val = out;
		current_val = out;
		return out;
	}
	double evaluate2(double current_angle){
		double e = current_angle - angle;
		double k = 1;
		
		
		if(abs(e) > PI){// to zmieniamy kierunek
			k *= -1.f;
		}
		move_e(e);
		cmpt_velocity();
		double	out = k * (K*e + Ti*summa +Td*velocity) *power;
		
		if(out > power)
			out = power;
		else if(out < -power)
			out = -power;
		else 
			summa += e;
			
		prev_e[0] = e;
		prev_val = out;
		current_val = out;
		return out;
	}
	double evaluate3(double e){
		double k = 1;
		
		
		// if(abs(e) > PI){// to zmieniamy kierunek
			// k *= -1.f;
		// }
		move_e(e);
		cmpt_velocity();
		double	out = k * (K*e + Ti*summa +Td*velocity) *power;
		
		if(out > power)
			out = power;
		else if(out < -power)
			out = -power;
		else 
			summa += e;
			
		prev_e[0] = e;
		prev_val = out;
		current_val = out;
		return out;
	}
};

class Motor: public PID{
public:
	double vel;
	double power; 
	
	void setVelocity(double v){
		vel = v;
	}
	double evaluate(double curr_v){
		double e = vel - curr_v;
		double k = 1;
		
		vel = glm::clamp(vel, -power, power);
		
		if(abs(e) < 0.05*power)
			e = 0.f;
		
		move_e(e);
		cmpt_velocity();
		double	out = k * (K*e + Ti*summa +Td*velocity) *power;
		
		// out = glm::clamp(out, -power, power);
		if(out > power)
			out = power;
		else if(out < -power)
			out = -power;
		else 
			summa += e;
			
		prev_e[0] = e;
		prev_val = out;
		current_val = out;
		return out;
	
	}
};

class Divider : public PID {
public:
	
};

template <typename T>
bool saveCache(string fileName, vector<T> &data){
	ofstream file;
	file.open("cache\\"+fileName+".cache", ios::binary | ios::out | ios::trunc);
		int size = data.size();
		file.write((char*)data.data(), data.size()*sizeof(T));
	file.close();
	return true;
}
template <typename T>
int loadCache(string fileName, vector<T> &data){
	ifstream file;
	int size=0;
	// try {
		file.open("../bin/cache/"+fileName+".cache", ios::binary | ios::in);
		if(file.is_open()){
			file.seekg(0, std::ios::end);
			size = file.tellg()/sizeof(T);
			file.seekg(0, std::ios::beg);
		
			// cerr<<"rozmiar danych: "<<size<<endl;
			data.resize(size);
			file.read((char*)data.data(), size*sizeof(T));
		}
		else
			cerr<<"\n\n\nfail..\n\n"<<endl;
		file.close();
	// }
	// catch (ifstream::failure e) {
    // cout << "Exception opening "+fileName+": "<<e.what()<<endl;;
  // }
	// cerr<<"loaded: "<<size<<endl;
	return size;
}

template <typename T>
class Bezier {
public:
	string name;
	static map<string, Bezier<T>*> g_beziers;
	vector <T> m_points;
	vector <float> m_weights;
	Bezier(){}
	void reg(string n){
		name = n;
		g_beziers[n] = this;
	}
	Bezier(vector<T> &points, vector<float> &weights){
		m_points = points;
		m_weights = weights;
	}
	void edit(vector<T> &points, vector<float> &weights){
		m_points = points;
		m_weights = weights;
	}
	float factorial(int k){
		switch (k){
			case 0 : return 1;
			case 1 : return 1;
			case 2 : return 2;
			case 3 : return 6;
			case 4 : return 24;
			case 5 : return 120;
			case 6 : return 720;
			case 7 : return 5040;
		}
	}
	float Berenstein(float t, int n, int i){
		// t - parametr
		// n - stopieñ krzywej
		// i - indeks punktu
		if(i<0 || i >n)
			return -1;
		
		return factorial(n)/factorial(n-i)/factorial(i)*pow(t, i)*pow(1.f-t, n-i);
	}
	T eval(float param){
		float t = glm::clamp(abs(param), 0.f, 1.f);
		int n = m_points.size()-1;
		T out(0);
		float sum = 0;
		for(int i = 0; i<=n; i++){
			float tmp = m_weights[i]*Berenstein(t, n, i);
			sum += tmp;
			out += tmp * m_points[i];
		}
		return out/sum*sign(param);
	}
	int size(){
		return m_points.size();
	}
};
template <typename T>
map<string, Bezier<T>*> Bezier<T>::g_beziers;



