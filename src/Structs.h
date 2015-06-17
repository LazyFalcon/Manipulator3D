class IControllable {
public:
	glm::vec4 position;
	glm::quat orientation;
	
	void edit(); // rysuje strza³ki b¹dŸ kó³ka(jak w blenderze) do zmieniania pozycji/orientacji obiektu
	
};

void IControllable::edit(){
	if(Scene::EditPosition){
		Engine::drawArrow(AxisX, Engine::colorRed, [this](float delta){
			position.x += delta;
		});
		Engine::drawArrow(AxisY, Engine::colorRed, [this](float delta){
			position.y += delta;
		});
		Engine::drawArrow(AxisZ, Engine::colorRed, [this](float delta){
			position.z += delta;
		});
		
	}
	else if(Scene::EditOrientation){
		
		
	}
	
}





class Light : public IControllable {
public:
	glm::vec4 color;
	
};
class SpotLight : public Light {
public: 
	float size;
};
class PointLight : public Light {
public:
	glm::vec3 size;
};

class Enviro : public IControllable {
public:
	btRigidBody *rgBody;
	Engine::Mesh mesh;
	GLuint texture;
	
	void update(float dt);
	~Enviro(); // trzeba wyrejestrowaæ teksturê, mesha i rgBody
};

class Joint {
public:
	glm::vec4 position;
	glm::vec4 axis;
	float jointVariable;
	
};
class Module {
public:
	Joint jointA;
	Joint jointB;
	
	
};
class Robot {
public:
	vector <Module> chain;
	
	void forwardKinematics(vector<float>jointVariables);
	void inverseKinematics();
	
};



