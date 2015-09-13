/*Note:
	s³uzy do przechowywania statycznej geometrii, bez convex meshy, znaczy niekolidujacej
	w przysz³oœci rozszerzyæ o mozliwoœæ wczytywana tekstur, convexów i tekstur

	generuje jedno VAO na plik
*/

class ObjManager{
public:
	map<string, MeshInfo> m_collection;
	
	vector<float> model_vertices;
	vector<float> model_coords;
	vector<float> model_normals;
	vector<int> model_indices;
	GLuint VAO;
	GLuint IBO;
	GLuint VBO[3];
	
	ObjManager(){
		model_vertices.reserve(600000);
		model_coords.reserve(400000);
		model_normals.reserve(600000);
		model_indices.reserve(600000);
	}
	
	MeshInfo push(string name, MeshInfo mesh){
		m_collection[name] = mesh;
	}
	MeshInfo get(string name){
		return m_collection[name];
	}
	void generateVAO();
	void load(string path);
};

void ObjManager::generateVAO(){

		glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		glGenBuffers(3, VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
		glBufferData(GL_ARRAY_BUFFER, model_vertices.size()*sizeof(float), &model_vertices[0], GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
		glBufferData(GL_ARRAY_BUFFER, model_coords.size()*sizeof(float), &model_coords[0], GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
		glBufferData(GL_ARRAY_BUFFER, model_normals.size()*sizeof(float), &model_normals[0], GL_STATIC_DRAW);
			glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(2);
	glBindVertexArray(0);
	
	glGenBuffers(1, &IBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*model_indices.size(),&model_indices[0], GL_STATIC_DRAW);
		
}

void ObjManager::load(string name){
	// cerr<<"model: "<<name<<endl;
	static string prefix("../res/models/");
	static string postfix(".obj");
	string tmp("#");
	
	//kontenerty tymczasowe
	vector<float> vertex;
	vector<float> coord;
	vector<float> normal;
	vertex.reserve(600000);
	coord.reserve(400000);
	normal.reserve(600000);
	
	name = prefix + name + postfix;
	fstream file;
	file.open(name.c_str(), ios::in);
	
	file>>tmp;
		
	//per model
	while(!file.eof()){
		MeshInfo info;
		info.start_index = model_indices.size();
		
		while(tmp != "o" && !file.eof())
			file>>tmp;
			
		string mesh_name(tmp);
		// cerr<<"\t"<<tmp<<endl;
		int u(0), k(0);
		{
			char nop;
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
				if(model_indices.size()>0)
					i = model_indices[model_indices.size()-1]+1;

				
				u=0;
				k=0;
				file>>tmp;

				while(tmp == "f" && !file.eof()){
					for(int t=0; t<3; t++){
						file>>a>>nop;
						file>>b>>nop;
						file>>c;
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
				}
		}
		info.end_index = model_indices.size();
		// info.offset = (void*)(sizeof(int)*info.start_index);
		info.count = info.end_index - info.start_index;
		push(mesh_name, info);
	}
	
	generateVAO();
	for(auto &it : m_collection){
		it.second.VAO = VAO;
		it.second.IBO = IBO;
		it.second.VBO[0] = VBO[0];
		it.second.VBO[1] = VBO[1];
		it.second.VBO[2] = VBO[2];
		
	}
	
	file.close();
}

