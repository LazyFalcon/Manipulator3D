
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

void UIContainer::draw(UI::IMGUI &gui){
	auto shader = shaders[m_boxes.second];
	glUseProgram(shader);
	glUniform(shader, window_width,   "uWidth");
	glUniform(shader, window_height,  "uHeight");

	updateBuffer(Engine::b_guiRects, m_boxes.first.m_box);
	updateBuffer(Engine::b_color, m_boxes.first.m_color);
		setupBuffer(Engine::quadCorner,0,4,0);
		setupBuffer(Engine::b_guiRects,1,4,1);
		setupBuffer(Engine::b_color, 2, 4, 1, GL_UNSIGNED_BYTE, GL_TRUE);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_boxes.first.m_box.size());


	shader = shaders[m_images.second];
	glUseProgram(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gui.m_imageSet->ID);
	glUniform1i(glGetUniformLocation(shader,"mainTex"),0);
	setupBuffer(Engine::quadCorner,0,4,0);
	glUniform(shader, window_width,   "uWidth");
	glUniform(shader, window_height,  "uHeight");

	for(auto &it : m_images.first){
		glUniform(shader, colorHex(it.color),"uColor");
		glUniform(shader, it.rect, "rect");
		glUniform(shader, it.uvs, "uvs");
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	m_images.first.clear();
	m_boxes.first.m_box.clear();
	m_boxes.first.m_color.clear();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


bool ResourceLoader::loadImage(const string &name){
	cout<<"image: "+name<<endl;
	auto &images = resources->images;
	auto &textures = resources->textures;

	string onlyName(name, 0, name.size()-4);
	if(textures.find(onlyName) != textures.end())
		return true;
	if(images.find(onlyName) != images.end())
		return true;

	string fileName = imagePath+name;
	ILuint imageID;
	GLuint textureID;
	ILboolean success;
	ILenum error;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);
	success = ilLoadImage(fileName.c_str());
	if (success){
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		logger::log<<"name "<<fileName<<std::endl;
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT){
			iluFlipImage();
			logger::log<<"\tflipped"<<endl;
		}
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (!success){
			error = ilGetError();
			cout << "Image conversion failed  " << name << " - " << iluErrorString(error) << std::endl;

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
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,5);
	}
	else{
		error = ilGetError();
		cerr<<"error loading image( "<<error<<" )"<<name<<endl;

		unsigned char tmp[] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 2,
					 2,
					 0,
					 GL_RGBA,
					 GL_UNSIGNED_BYTE,
					 (void*)tmp);

		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);



		// dorzuæ ³adowanie pustej, ró¿owej tekstury
	}

	textures[onlyName] = textureID;
	images[onlyName] = Image { textureID, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT)};

	ilDeleteImages(1, &imageID);

	return success;
}
GLuint ResourceLoader::compileShader(string vertex, string fragment){
	GLuint f, v;
	GLuint shader;
	char *vs,*fs;

	v=glCreateShader(GL_VERTEX_SHADER);
	f=glCreateShader(GL_FRAGMENT_SHADER);

	// load shaders & get length of each
	GLint vlen;
	GLint flen;
	vs = loadFile(shaderPath+vertex,vlen);
	fs = loadFile(shaderPath+fragment,flen);

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
void ResourceLoader::printShaderInfoLog(GLint shader){
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
Resources::~Resources(){
	glDeleteBuffers(3, VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteVertexArrays(1, &VAO);
	for(auto &it : textures)
		glDeleteTextures(1, &it.second);
	for(auto &it : textures)
		glDeleteTextures(1, &it.second);

	cerr<<"~Resources"<<endl;
}
Scene::~Scene(){
	cerr<<"~Scene"<<endl;
	delete resources;
}

void Graph::draw(){
	// if(flags & LastX)
		// Engine::b_vec2_16k.update(data, data.size() - getCount(), data.size());
	// else
		Engine::b_vec2_16k.update(data);

	auto shader = shaders["lines2D"];
	glUseProgram(shader);

	glUniform(shader, getProjection(), "projection");
	glUniform(shader, colorHex(dataColor), "color");
	glLineWidth(1);
	glDisable(GL_LINE_SMOOTH);

	Engine::b_vec2_16k.bind(0,2);
	glDrawArrays(GL_LINE_STRIP, 0, getCount());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
}

void Plot::init(){
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, box.z, box.w, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);
	glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
		glDrawBuffers(1, Engine::DrawBuffers);
		glViewport(0, 0, box.z, box.w);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	logger::log<<"tex & FBO "<<texID<<" "<<fbo<<endl;
}
void Plot::resize(glm::vec2 newDimensions){
	box.z = newDimensions.x;
	box.w = newDimensions.y;
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA8 , box.z, box.w, 0,GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glBindTexture(GL_TEXTURE_2D, 0);
}
void Plot::setUpFBO(){
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
	glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, box.z, box.w);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void Plot::plot(){
	// if(not visible) return;
	// bool graphsVisibled = false;
	// for(auto graph : graphs){
		// graphsVisibled |= graph->getModified();
	// }
	// if(!graphsVisibled) return;
	setUpFBO();
	for(auto graph : graphs){
		graph->draw();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Engine::drawTexturedBox(texID, box);
}

