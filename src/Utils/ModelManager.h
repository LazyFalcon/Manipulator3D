// enum mesh_type{BODY, BARREL, TURRET, WHEEL};
struct Mesh{
public:
	int begin;
	int end;
	int count;
	int additional_info;
	string name;
	int ID;
	void *offset;
	glm::mat4 constTranslation;
	glm::vec3 rotAxis;
	glm::vec3 transAxis;
	
	void setRotAxis(float x, float y, float z){
		rotAxis=glm::vec3(x,y,z);
	}
	void setTransRotAxis(float x, float y, float z){
		transAxis=glm::vec3(x,y,z);
	}
	void setTranslation(float x, float y, float z){
		constTranslation=glm::translate(glm::mat4(1.f), glm::vec3(x,y,z));
	}
};

vector <Mesh*> meshes;
class MeshContainer{
public:
	void addMesh(Mesh *mesh){
		meshes.push_back(mesh);
	}
};

class ModelManager{
public:
		static vector<float> model_vertices;
		static vector<float> model_coords;
		static vector<float> model_normals;
		static vector<int> model_indices;
	ModelManager(){
		model_vertices.clear();
		 model_coords.clear();
		model_normals.clear();
		 model_indices.clear();
		
		model_vertices.reserve(650000);
		model_coords.reserve(650000);
		model_normals.reserve(650000);
		 model_indices.reserve(650000); 
		int a,b;
		loadMeshes("../res/meshList.list",a,b);
		
	}
	static void loadMeshes(string name, int &first_num, int &count);
	static void meshLoader(string name);
	static void generateModelBuffers();
	static void drawModels();
};
vector<float>ModelManager::model_vertices;
vector<float>ModelManager::model_coords;
vector<float>ModelManager::model_normals;
vector<int>  ModelManager::model_indices;

void ModelManager::meshLoader(string name){
	cerr<<"model: "<<name<<endl;
	
	// qubeTexture=loadTexture("qube.png");
	// string name="qube.obj";
	vector<float> vertex;
	vector<float> coord;
	vector<float> normal;
	vertex.reserve(600000);
	coord.reserve(400000);
	normal.reserve(600000);
	int u(0), k(0);
	// cerr<<"substr: "<<name.substr(name.size()-5, 4)<<endl;
	if(name.substr(name.size()-4, 4) == "mesh"){// load .mesh
	fstream file;
	file.open(name.c_str(), ios::in);
		string tmp, mpt;
		char ctmp;
	file>>tmp;
	float x,y,z;
	int a,b,c;
		while(tmp=="v"){
			file>>x>>y>>z;
			vertex[u++]=x;
			vertex[u++]=y;
			vertex[u++]=z;
			file>>tmp;
		}
		u=0;
		while(tmp=="vt"){
			file>>x>>y;
			coord[u++]=x;
			coord[u++]=y;
			file>>tmp;
		}
		u=0;
		while(tmp=="vn"){
			file>>x>>y>>z;
			normal[u++]=x;
			normal[u++]=y;
			normal[u++]=z;
			file>>tmp;
		}
		getline(file, tmp);
		getline(file, tmp);
		int i(0);
		if(model_indices.size()>0)i=model_indices[model_indices.size()-1]+1;
		// i+=przebieg;
		
		u=0;//model_vertices.size();
		k=0;//model_coords.size();
		file>>tmp;
		
		/*
		1_______2
		|            / |
		|         /    |
		|      /       |
		|   /          |
		|/             |
		0             3
		012023
		*/
		while(tmp=="f"){
		for(int t=0; t<3; t++){
		file>>a>>b>>c;
			a--;b--;c--;
				model_vertices.push_back(vertex[a*3]);
				model_vertices.push_back(vertex[a*3+1]);
				model_vertices.push_back(vertex[a*3+2]);
				model_coords.push_back(coord[b*2]);
				model_coords.push_back(coord[b*2+1]);
				model_normals.push_back(normal[c*3]);
				model_normals.push_back(normal[c*3+1]);
				model_normals.push_back(normal[c*3+2]);
				u+=3;k+=2;
				model_indices.push_back(i++);
			}
				file>>tmp;
				if(tmp=="f" || tmp=="#");
				else{
					model_indices.push_back(i-3);
					model_indices.push_back(i-1);
					file>>b>>c;
					a=atoi(tmp.c_str());a--;b--;c--;
				model_vertices.push_back(vertex[a*3]);
				model_vertices.push_back(vertex[a*3+1]);
				model_vertices.push_back(vertex[a*3+2]);
				model_coords.push_back(coord[b*2]);
				model_coords.push_back(coord[b*2+1]);
				model_normals.push_back(normal[c*3]);
				model_normals.push_back(normal[c*3+1]);
				model_normals.push_back(normal[c*3+2]);
				u+=3;k+=2;
						model_indices.push_back(i++);
						file>>tmp;
				}
				if(tmp=="#")break;
		}
		
		// LOG<<"vertices: "<<u/3<<endl;
		// LOG<<"indices: "<<model_indices.size()<<endl;
	file.close();
	}
	else{// load obj
	fstream file;
	file.open(name.c_str(), ios::in);
		string tmp, mpt;
		char ctmp;
		char nop;
		// pozbywamy siê poczatkowych œmieci
		while(tmp != "v" && !file.eof())
			file>>tmp;
		
	// file>>tmp;
	float x,y,z;
	int a,b,c;
		while(tmp=="v"){
			file>>x>>y>>z;
			vertex[u++]=x;
			vertex[u++]=y;
			vertex[u++]=z;
			file>>tmp;
		}
		u=0;
		while(tmp=="vt"){
			file>>x>>y;
			coord[u++]=x;
			coord[u++]=y;
			file>>tmp;
		}
		u=0;
		while(tmp=="vn"){
			file>>x>>y>>z;
			normal[u++]=x;
			normal[u++]=y;
			normal[u++]=z;
			file>>tmp;
		}
		while(tmp != "f" && !file.eof())
			file>>tmp;
		int i(0);
		if(model_indices.size()>0)i=model_indices[model_indices.size()-1]+1;
		// i+=przebieg;
		
		u=0;//model_vertices.size();
		k=0;//model_coords.size();
		
		
		/*
		1_______2
		|            / |
		|         /    |
		|      /       |
		|   /          |
		|/             |
		0             3
		012023
		*/
		while(tmp=="f"&& !file.eof()){
		for(int t=0; t<3; t++){
		file>>a>>nop>>b>>nop>>c;
		// cerr<<a<<" "<<b<<" "<<c<<endl;
			a--;b--;c--;
				model_vertices.push_back(vertex[a*3]);
				model_vertices.push_back(vertex[a*3+1]);
				model_vertices.push_back(vertex[a*3+2]);
				model_coords.push_back(coord[b*2]);
				model_coords.push_back(coord[b*2+1]);
				model_normals.push_back(normal[c*3]);
				model_normals.push_back(normal[c*3+1]);
				model_normals.push_back(normal[c*3+2]);
				u+=3;k+=2;
				model_indices.push_back(i++);
			}
				file>>tmp;
				if(tmp=="f");
				else{
					model_indices.push_back(i-3);
					model_indices.push_back(i-1);
					file>>nop>>b>>nop>>c;
					a=atoi(tmp.c_str());a--;b--;c--;
				model_vertices.push_back(vertex[a*3]);
				model_vertices.push_back(vertex[a*3+1]);
				model_vertices.push_back(vertex[a*3+2]);
				model_coords.push_back(coord[b*2]);
				model_coords.push_back(coord[b*2+1]);
				model_normals.push_back(normal[c*3]);
				model_normals.push_back(normal[c*3+1]);
				model_normals.push_back(normal[c*3+2]);
				u+=3;k+=2;
						model_indices.push_back(i++);
						file>>tmp;
				}
		}
		
		// LOG<<"vertices: "<<u/3<<endl;
		// LOG<<"indices: "<<model_indices.size()<<endl;
	file.close();
	}
	
	vertex.clear();
	coord.clear();
	normal.clear();
}

void ModelManager::loadMeshes(string name, int &first_num, int &count){
	fstream file;
	string buff;
	string prefix="../models\\";
	float tmp,x,y,z;
	int number;
	
	first_num=meshes.size();
	file.open(name.c_str(), ios::in);
		file>>number;

		for(int i=number; i>0; --i){//iloœæ meshy
			Mesh *mesh=new Mesh;
			file>>mesh->ID;//wczytaj ID
				mesh->begin=model_indices.size();
			file>>buff;//wczytaj nazwê pliku z meshem
			// LOG<<buff<<endl;
			meshLoader(prefix+buff);
				mesh->end=model_indices.size();
			// LOG<<"\nloaded: "<<buff<<endl;
			file>>x>>y>>z;//wczytaj oœ rotacji
				mesh->rotAxis=glm::vec3(x,y,z);
			file>>number;//wczytaj iloœæ spec punktów
			for(int j=number; j>0; --j){;}
			file>>x>>y>>z;
				mesh->constTranslation=glm::translate(glm::mat4(1.f), glm::vec3(x,y,z));
				mesh->offset=(void*)(sizeof(int)*mesh->begin);
				mesh->count=mesh->end-mesh->begin;
			meshes.push_back(mesh);
		}
		count=meshes.size()-first_num;
	file.close();
}

void ModelManager::generateModelBuffers(){
		VAO["model"]=0;
		IBO["model"]=0;
		GLuint modelVBO[3];
		glGenVertexArrays(1, &VAO["model"]);
	glBindVertexArray(VAO["model"]);
		glGenBuffers(3, modelVBO);
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO[0]);
		glBufferData(GL_ARRAY_BUFFER, model_vertices.size()*sizeof(float), &model_vertices[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glVertexAttribDivisor(0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO[1]);
		glBufferData(GL_ARRAY_BUFFER, model_coords.size()*sizeof(float), &model_coords[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glVertexAttribDivisor(0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, modelVBO[2]);
		glBufferData(GL_ARRAY_BUFFER, model_normals.size()*sizeof(float), &model_normals[0], GL_STATIC_DRAW);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glVertexAttribDivisor(0, 0);
		glGenBuffers(1, &IBO["model"]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO["model"]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*model_indices.size(),&model_indices[0], GL_STATIC_DRAW);
	glBindVertexArray(0);
		model_coords.clear();
		model_vertices.clear();
		model_normals.clear();
}

