#pragma once
using namespace std;



template <typename T>
class Bezier {
public:
	string name;
	static map<string, Bezier<T>*> g_beziers;
	vector <T> m_points;
	vector <float> m_weights;
	Bezier(){}
	float sign(float x){
		if(x < 0.f)
			return -1.f;
		else if(x > 0.f)
			return 1.f;
		return 0.f;
	}
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
			default : return 0;
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
	void operator = (Bezier<T> bez){
		name = bez.name;
		m_points = bez.m_points;
		m_weights= bez.m_weights;
	}
};
template <typename T>
map<string, Bezier<T>*> Bezier<T>::g_beziers;
