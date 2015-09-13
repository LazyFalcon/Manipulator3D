class ResourceManager{
public:
	ResourceManager(){
		loadShaders("../shader.list");
		loadTextures("../res/texture.list");
	}
	static void loadShaders(string file_name);
	static GLuint loadTextureImage(string theFileName);//mipmapped
	static GLuint loadImage(string theFileName, int a);
	static void loadTextures(string file_name);
	static void printShaderInfoLog(GLint shader);
	static char* loadFile(string fname, GLint &fSize);
	static GLint initShader(string vertex, string fragment);
	static void addVAO(){}
	static void addIBO(){}
};

void ResourceManager::loadTextures(string file_name){
		fstream file;
		file.open(file_name.c_str(), ios::in);
		string prefix="../res/textures\\";
		
		string buff;
		GLint texture;
		int x;
		file>>x;
		while(x--){
			file>>buff;
			texture=loadTextureImage(prefix+buff);
			string blah(buff, 0, buff.size()-4);
			textures[blah]=texture;
		}
		file>>x;
		int a;
		while(x--){
			file>>buff>>a;
			texture=loadImage(prefix+buff, a);
			string blah(buff, 0, buff.size()-4);
			textures[blah]=texture;
		}
		file.close();
	}
void ResourceManager::loadShaders(string file_name){
		fstream file;
		file.open(file_name.c_str(), ios::in);
		string prefix = "../shaders\\";
		string vert = ".vert";
		string frag = ".frag";
		string name = "";
		string vs, fs;
		GLint shader;
		while(file>> name >> vs >> fs){
			shader = initShader(prefix+vs+vert, prefix+fs+frag);
			shaders[name]=shader;
			cout<<" "<<shaders[name]<<"\n";
		}
		file.close();
	}
GLint ResourceManager::initShader(string vertex, string fragment){
	GLuint f, v;
	GLuint shader;
	char *vs,*fs;
	
	v=glCreateShader(GL_VERTEX_SHADER);
	f=glCreateShader(GL_FRAGMENT_SHADER);	
	
	// load shaders & get length of each
	GLint vlen;
	GLint flen;
	vs = loadFile(vertex,vlen);
	fs = loadFile(fragment,flen);
	
	const char * vv = vs;
	const char * ff = fs;
	
	glShaderSource(v, 1, &vv,&vlen);
	glShaderSource(f, 1, &ff,&flen);
	
	GLint compiled;
	
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compiled);
	if (!compiled){
		cout << "Vertex shader " <<vertex<<" not compiled." << endl;
		printShaderInfoLog(v);
	}
	
	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compiled);
	if (!compiled){
		cout << "Fragment shader "<<fragment<<" not compiled." << endl;
		printShaderInfoLog(f);
	}
	
	shader=glCreateProgram();
	glBindAttribLocation(shader,0, "in_Position");
	glBindAttribLocation(shader,1, "in_Color");
	
	glAttachShader(shader,v);
	glAttachShader(shader,f);
	
	glLinkProgram(shader);

	delete [] vs; // dont forget to free allocated memory
	delete [] fs; // we allocated this in the loadFile function...
	return shader;
}
GLuint ResourceManager::loadTextureImage(string theFileName){
	ILuint imageID;
	GLuint textureID;
	ILboolean success;
	ILenum error;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	success = ilLoadImage(theFileName.c_str());
	if (success){
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT){
			iluFlipImage();
		// cerr<<"flip: "<<theFileName<<endl;
		}
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success){
			error = ilGetError();
			cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
			exit(-1);
		}
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 ilGetInteger(IL_IMAGE_BPP),
					 ilGetInteger(IL_IMAGE_WIDTH),
					 ilGetInteger(IL_IMAGE_HEIGHT),
					 0,
					 ilGetInteger(IL_IMAGE_FORMAT),
					 GL_UNSIGNED_BYTE,
					 ilGetData());
		
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,5);
	}
	else{ 
		error = ilGetError();
		cerr<<"error loading image( "<<error<<" )"<<theFileName<<endl;
		exit(-1);
	}
	ilDeleteImages(1, &imageID);
	return textureID;
}
GLuint ResourceManager::loadImage(string theFileName, int a){
	ILuint imageID;
	GLuint textureID;
	ILboolean success;
	ILenum error;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	success = ilLoadImage(theFileName.c_str());
	if (success){
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT){
			iluFlipImage();
		}
		if(a==1){
			// success = ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_BYTE);
			success = ilConvertImage(IL_LUMINANCE, IL_UNSIGNED_SHORT);
			if (!success){
				error = ilGetError();
				cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
				exit(-1);
			}
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 ilGetInteger(IL_IMAGE_BPP),
						 ilGetInteger(IL_IMAGE_WIDTH),
						 ilGetInteger(IL_IMAGE_HEIGHT),
						 0,
						 ilGetInteger(IL_IMAGE_FORMAT),
						 // GL_UNSIGNED_BYTE,
						 GL_UNSIGNED_SHORT,
						 ilGetData());
		}
		if(a==2){
			success = ilConvertImage(IL_RGBA, GL_UNSIGNED_BYTE);
			if (!success){
				error = ilGetError();
				cout << "Image conversion failed - IL reports error: " << error << " - " << iluErrorString(error) << std::endl;
				exit(-1);
			}
			glGenTextures(1, &textureID);
			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D,
						 0,
						 ilGetInteger(IL_IMAGE_BPP),
						 ilGetInteger(IL_IMAGE_WIDTH),
						 ilGetInteger(IL_IMAGE_HEIGHT),
						 0,
						 ilGetInteger(IL_IMAGE_FORMAT),
						 GL_UNSIGNED_BYTE,
						 ilGetData());
	}
	}
	else{ 
		error = ilGetError();
		cerr<<"error loading image( "<<error<<" )"<<theFileName<<endl;
		exit(-1);
	}
	ilDeleteImages(1, &imageID);
	return textureID;
}
void ResourceManager::printShaderInfoLog(GLint shader){
	int infoLogLen = 0;
	int charsWritten = 0;
	GLchar *infoLog;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);

// should additionally check for OpenGL errors here

	if (infoLogLen > 0){
		infoLog = new GLchar[infoLogLen];
		// error check for fail to allocate memory omitted
		glGetShaderInfoLog(shader,infoLogLen, &charsWritten, infoLog);
		cout << "InfoLog:" << endl << infoLog << endl;
		delete [] infoLog;
	}

// should additionally check for OpenGL errors here
}
char* ResourceManager::loadFile(string fname, GLint &fSize){
	ifstream::pos_type size;
	char * memblock;
	string text;

	// file read based on example in cplusplus.com tutorial
	ifstream file (fname.c_str(), ios::in|ios::binary|ios::ate);
	if (file.is_open()){
		size = file.tellg();
		fSize = (GLuint) size;
		memblock = new char [size];
		file.seekg (0, ios::beg);
		file.read (memblock, size);
		file.close();
		cout << "file " << fname << " loaded" << endl;
		text.assign(memblock);
	}
	else{
		cout << "Unable to open file " << fname << endl;
		exit(1);
	}
	return memblock;
}


