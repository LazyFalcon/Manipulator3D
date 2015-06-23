
extern UI::IMGUI ui;
extern Camera camera;
extern float	 mouse_x, mouse_y;

extern int robotPositionsCounter;
extern int robotPositionsMax;
extern vector<glm::vec4> robotPositions;
extern PositionSaver g_mousePositions;
extern PositionSaver g_robotPositions;
extern PositionSaver g_robotDebugPositions;
Matrix::Matrix(u32 h, u32 w): width(w), height(h), array(w*h, 0){
	// array = new float[w*h];
}
Matrix& Matrix::operator =(const Matrix &matrix){
	// array = new float[matrix.width*matrix.height];
	array = matrix.array;
	width = matrix.width;
	height= matrix.height;
	// std::copy(matrix.array, matrix.array+width*height, array);
	return *this;
}
Matrix::Matrix(const Matrix &matrix){
	// array = new float[matrix.width*matrix.height];
	array = matrix.array;
	width = matrix.width;
	height= matrix.height;
	// std::copy(matrix.array, matrix.array+width*height, array);
}
double& Matrix::operator()(u32 h, u32 w){
	return array[h*width + w];
}
float Matrix::operator()(u32 h, u32 w) const {
	return array[h*width + w];
}

Matrix Matrix::operator+(Matrix &matrix){
	Matrix out(height, width);
	out = *this;
	for(u32 h=0; h<height; h++)
		for(u32 w=0; w<width; w++)
			out(h,w) += matrix(h,w);
	return out;
}
Matrix Matrix::operator*(Matrix &matrix){
	if(width != matrix.height)
		std::cout<<"matrix erorr"<<std::endl;

	Matrix out(height, matrix.width);

	// iterujemy po out
	for(u32 w=0; w<matrix.width; w++) // kolumny z m2
	for(u32 h=0; h<height; h++){ // wiersze z m1
		out(h,w) = mulRowColumn(h,w,matrix);
	}
	return out;
}
Matrix Matrix::operator*(float val){
	Matrix out(height, width);
	for(u32 w=0; w<width; w++)
	for(u32 h=0; h<height; h++)
		out(h,w) = (*this)(h,w)*val;

	return out;
}
float Matrix::mulRowColumn(u32 row, u32 column, Matrix &second){
	float out(0);
	for(u32 w=0; w<width; w++)
	// for(u32 h=0; h<second.height; h++)
		out += array[row*width + w] * second(w, column);
	return out;
}
void Matrix::insertRow(u32 row, std::vector<float> &&vec){
	for(u32 i=0; i<width && i<vec.size(); i++)
		array[row*width + i] = vec[i];
}
void Matrix::insertColumn(u32 column, std::vector<float> &&vec){
	for(u32 i=0; i<height && i<vec.size(); i++)
		array[width*i + column] = vec[i];
}

Matrix& Matrix::transpose(){
	std::vector<double>newArray(width*height);
	for(u32 h=0; h<height; h++)
		for(u32 w=0; w<width; w++)
			newArray[h + w*height] = array[h*width + w];
	std::swap(width, height);
	array.swap(newArray);
	// std::swap(array, newArray);
	// delete [] newArray;
	return *this;
}
Matrix Matrix::transposed(){
	Matrix out(width, height);
	for(u32 h=0; h<height; h++)
		for(u32 w=0; w<width; w++)
			out(w,h) = (*this)(h,w);
	return out;
}
std::vector<double>& Matrix::getVector(){
	return array;
}


float dot(const Matrix &a, const Matrix &b){
	float out(0);
	for(u32 i=0; i<a.height; i++)
		out += a(i)*b(i);
	return out;
}
Matrix mul(const Matrix &a, const Matrix &b){
	Matrix out(a.height, a.width);
	for(u32 h=0; h<a.height; h++)
	for(u32 w=0; w<a.width; w++)
		out(h,w) = a(h,w)*b(h,w);
	return out;
}

Matrix buildJacobian(Robot &robot){
	Matrix jacobian(robot.chain.size(), 6);
	glm::vec4 position = glm::vec4(0,0,0,1);
	glm::vec3 axis = robot.chain[0]->axis.xyz();
	glm::quat transform(1,0,0,0);
	glm::vec3 currentEndPosition = robot.endEffector.position.xyz();


	for(u32 i=0; i<robot.chain.size(); i++){
		auto &module = robot.chain[i];
		{ // forward kinematics
			if(module->type == HINGE){
				transform = transform*glm::angleAxis(module->value, module->axis.xyz());
				position += transform*module->vecToA;
			}
			else if(module->type == PRISMATIC){
				position += transform*(module->vecToA + module->axis*module->value);
			}
			axis = module->entity->quat*axis;
			position += transform*module->vecToB;
		}

		if(module->type == HINGE){
			glm::vec3 c = glm::cross(axis.xyz(), currentEndPosition-position.xyz());
			jacobian.insertRow(i, {c.x, c.y, c.z, axis.x, axis.y, axis.z});
		}
		if(module->type == PRISMATIC)
			jacobian.insertRow(i, {axis.x, axis.y, axis.z,0,0,0});

	}
	return jacobian;

}
Matrix buildJacobian(Robot &robot, std::vector<double> &variables, Point endPoint){
	Matrix jacobian(robot.chain.size(), 6);
	glm::vec4 endPosition = glm::vec4(0,0,0,1);
	glm::vec3 axis = robot.chain[0]->axis.xyz();
	glm::quat transform{0,0,0,1};
	glm::vec3 currentEndPosition = endPoint.position.xyz();


	for(u32 i=0; i<robot.chain.size(); i++){
		auto &module = robot.chain[i];
		{ // forward kinematics
			if(module->type == HINGE){
				transform = transform*glm::angleAxis((float)variables[i], glm::normalize(module->axis.xyz()));
				endPosition += transform*module->vecToA;
			}
			else if(module->type == PRISMATIC){
				endPosition += transform*(module->vecToA + module->axis*(float)variables[i]);
			}
			axis = transform*module->axis.xyz();
			endPosition += transform*module->vecToB;
		}

		if(module->type == HINGE){
			glm::vec3 c = glm::cross(axis.xyz(), currentEndPosition - endPosition.xyz());
			jacobian.insertRow(i, {c.x, c.y, c.z, axis.x, axis.y, axis.z});
		}
		if(module->type == PRISMATIC)
			jacobian.insertRow(i, {axis.x, axis.y, axis.z,0,0,0});


			// axis = module->entity->quat*axis;
	}
	return jacobian;

}
Matrix buildJacobianReversed(Robot &robot, std::vector<double> &variables){
	Matrix jacobian(robot.chain.size(), 6);
	glm::vec4 position = glm::vec4(0,0,0,1);
	glm::vec3 axis = robot.chain[0]->axis.xyz();
	glm::quat transform(1,0,0,0);
	glm::vec3 currentEndPosition = robot.endEffector.position.xyz();


	for(u32 i=0; i<robot.chain.size(); i++){
		auto &module = robot.chain[i];
		{ // forward kinematics
			if(module->type == HINGE){
				transform = transform*glm::angleAxis((float)variables[i], module->axis.xyz());
				position += transform*module->vecToA;
			}
			else if(module->type == PRISMATIC){
				position += transform*(module->vecToA + module->axis*(float)variables[i]);
			}
			axis = module->entity->quat*axis;
			position += transform*module->vecToB;
		}

		if(module->type == HINGE){
			glm::vec3 c = glm::cross(axis.xyz(), currentEndPosition-position.xyz());
			jacobian.insertRow(i, {c.x, c.y, c.z, axis.x, axis.y, axis.z});
		}
		if(module->type == PRISMATIC)
			jacobian.insertRow(i, {axis.x, axis.y, axis.z,0,0,0});

	}
	return jacobian;

}

void clamp(std::vector<double> &vec, double min, double max){
	for(auto &it : vec)
		it = std::min(std::max(it, min), max);
}
void clamp(Matrix &mat, double min, double max){
	for(auto &it : mat.getVector())
		it = std::min(std::max(it, min), max);
}
