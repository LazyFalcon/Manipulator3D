#pragma once
#include "Robot.h"

// szerokość jest drugim parametrem
class Matrix {
public:
	std::vector<double> array;
	int width;
	int height;
	Matrix(int h, int w);
	Matrix& operator =(const Matrix &matrix);
	Matrix(const Matrix &matrix);
	~Matrix(){
		// delete [] array;
	}
	double& operator()(int h, int w=0);
	float operator()(int h, int w=0) const ;
	
	Matrix operator+(Matrix &matrix);
	Matrix operator*(Matrix &matrix);
	Matrix operator*(float val);
	float mulRowColumn(int row, int column, Matrix &second);
	
	void insertRow(int row, std::vector<float> &&vec);
	void insertColumn(int column, std::vector<float> &&vec);
	friend std::ostream & operator <<(std::ostream& output, const Matrix &mat){
		
		for(int h=0; h<mat.height; h++){
			for(int w=0; w<mat.width; w++)
				output<<std::to_string(mat(h,w))+"\t";
			output<<"\n";
		}
	}
	Matrix& transpose();
	Matrix transposed();
	std::vector<double>& getVector();
};

float dot(const Matrix &a, const Matrix &b);
Matrix mul(const Matrix &a, const Matrix &b);


Matrix buildJacobian(Robot &robot);
Matrix buildJacobian(Robot &robot, std::vector<double> &variables);

void clamp(std::vector<double> &vec, double min, double max);
void clamp(Matrix &mat, double min, double max);


  
void BADBADBADRobotIKRealtimeTest(Robot &robot);

void test(Robot &robot);
