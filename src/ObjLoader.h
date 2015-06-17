class ObjLoader {
public:

	vector<float> model_vertices;
	vector<float> model_coords;
	vector<float> model_normals;
	vector<int> model_indices;
	GLuint VAO;
	GLuint IBO;
	GLuint VBO[3];
	
	void operataor ()(){
		
		
	}
	
	bool ResourceLoader::loadObj(string filename){

	fstream file;
	file.open(filename.c_str(), ios::in);
		string tmp, mpt;
		char ctmp;
		char nop;
		// pozbywamy siê poczatkowych œmieci
		while(tmp != "v" && !file.eof())
			file>>tmp;

	vector<float> vertex;
	vector<float> coord;
	vector<float> normal;
	vertex.reserve(600000);
	coord.reserve(400000);
	normal.reserve(600000);
	int u(0), k(0);
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
		if(model_indices.size()>0)
			i = model_indices[model_indices.size()-1]+1;
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

	file.close();

}
	
};