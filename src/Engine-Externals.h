#include <FreeImage.h>
#define _DebugLine_  std::cerr<<"line: "<<__LINE__<<" : "<<__FILE__<<" : "<<__FUNCTION__<<"()\n";

#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
/// dodać tu jakis cykliczny buffer
void drawSingleButtonList(BoxColor &boxColor, std::string &shaderName){
	if(boxColor.m_box.size() == 0) return;
	
	auto shader = shaders[shaderName];
	glUseProgram(shader);
	glUniform(shader, window_width,   "uWidth");
	glUniform(shader, window_height,  "uHeight");

	updateBuffer(Engine::b_guiRects, boxColor.m_box);
	updateBuffer(Engine::b_color, boxColor.m_color);
		setupBuffer(Engine::quadCorner,0,4,0);
		setupBuffer(Engine::b_guiRects,1,4,1);
		setupBuffer(Engine::b_color, 2, 4, 1, GL_UNSIGNED_BYTE, GL_TRUE);

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, boxColor.m_box.size());

	shader = shaders[m_images.second];
	glUseProgram(shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gui.m_imageSet->ID);
	glUniform1i(glGetUniformLocation(shader,"uTexture"),0);
	setupBuffer(Engine::quadCorner,0,4,0);
	glUniform(shader, window_width,   "uWidth");
	glUniform(shader, window_height,  "uHeight");
}

void UIContainer::draw(UI::IMGUI &gui, int layer){
	drawSingleButtonList(m_backgroundBox.first[layer], m_backgroundBox.second);
	drawSingleButtonList(m_label.first[layer], m_label.second);
	drawSingleButtonList(m_editBox.first[layer], m_editBox.second);
	drawSingleButtonList(m_boxes.first[layer], m_boxes.second);

	for(auto &it : m_images.first[layer]){
		glUniform(shader, colorHex(it.color),"uColor");
		glUniform(shader, it.rect, "uRect");
		glUniform(shader, it.uvs, "uUVs");
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	m_images.first.clear();
	m_boxes.first.m_box.clear();
	m_boxes.first.m_color.clear();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


bool ResourceLoader::loadImage(const string &name){
	auto &images = resources->images;
	auto &textures = resources->textures;

	string onlyName(name, 0, name.size()-4);
	if(textures.find(onlyName) != textures.end())
		return true;
	if(images.find(onlyName) != images.end())
		return true;

	string fileName = imagePath+name;
	GLuint textureID;

	//image format
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	//check the file signature and deduce its format
	fif = FreeImage_GetFileType(fileName.c_str(), 0);
	if(fif == FIF_UNKNOWN){
		std::cout<<"duupa\n";
		return false;
	}

	FIBITMAP *image = FreeImage_Load(fif, fileName.c_str());

	BYTE *imageData = FreeImage_GetBits(image);
	//get the image width and height
	auto width = FreeImage_GetWidth(image);
	auto height = FreeImage_GetHeight(image);

	// auto imageType = FreeImage_GetImageType(image);

	// How many bits-per-pixel is the source image?
	int bitsPerPixel =  FreeImage_GetBPP(image);
	// int colorsUsed =  FreeImage_GetColorsUsed(image);

	std::cout<<"name "<<fileName<<std::endl;

	GLuint internalFormat;
	GLuint imageFormat;
	GLuint imageMagFilter;
	GLuint imageMinFilter;

	if(bitsPerPixel == 8){
		imageMagFilter = GL_NEAREST;
		imageMinFilter = GL_NEAREST;
		internalFormat = GL_RED;
		imageFormat = GL_RED;
	}
	else if(bitsPerPixel == 24){
		imageMagFilter = GL_LINEAR;
		imageMinFilter = GL_LINEAR;
		imageFormat = GL_BGR;
		internalFormat = GL_RGBA8;
	}
	else if(bitsPerPixel == 32){
		imageMagFilter = GL_LINEAR;
		imageMinFilter = GL_LINEAR;
		imageFormat = GL_BGRA;
		internalFormat = GL_RGBA8;
	}

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 internalFormat,
				 width,
				 height,
				 0,
				 imageFormat,
				 GL_UNSIGNED_BYTE,
				 imageData);

	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, imageMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, imageMinFilter);


	FreeImage_Unload(image);

	textures[onlyName] = textureID;
	images[onlyName] = Image { textureID, int(width), int(height)};


	return true;
}
void ResourceLoader::reloadShader(const string &shaderName){
	glDeleteProgram(shaders[shaderName]);
	shaders[shaderName] = compileShader(shaderName+".glsl");
	auto err = glGetError();
	if(err)std::cout<<"\t>> "<<err<<endl;
}
GLuint ResourceLoader::compileShader(const string &shaderName){
	GLuint fragmentS, vertexS;
	GLuint shader;

	string shaderSource = loadFile(shaderPath+shaderName);

	string vertexSource = "#version 330\n"s + "#define COMPILING_VERTEX_SHADER\n"s + shaderSource;
	string fragmentSource = "#version 330\n"s + "#define COMPILING_FRAGMENT_SHADER\n"s + shaderSource;

	vertexS = glCreateShader(GL_VERTEX_SHADER);
	fragmentS = glCreateShader(GL_FRAGMENT_SHADER);

	const char *vs_str = vertexSource.c_str();
	const char *fs_str = fragmentSource.c_str();
	glShaderSource(vertexS, 1, &vs_str, NULL);
	glShaderSource(fragmentS, 1, &fs_str, NULL);


	glCompileShader(vertexS);

	GLint compiled;
	glGetShaderiv(vertexS, GL_COMPILE_STATUS, &compiled);

	if (!compiled){
		cout << "Vertex shader in " <<shaderName<<" not compiled." << endl;
		printShaderInfoLog(vertexS);
		int x;
		cin>>x;

		return compileShader(shaderName);
	}

	glCompileShader(fragmentS);
	glGetShaderiv(fragmentS, GL_COMPILE_STATUS, &compiled);

	if (!compiled){
		cout << "Fragment shader in "<<shaderName<<" not compiled." << endl;
		printShaderInfoLog(fragmentS);
		int x;
		cin>>x;

		return compileShader(shaderName);
	}

	shader = glCreateProgram();

	glAttachShader(shader,vertexS);
	glAttachShader(shader,fragmentS);

	glLinkProgram(shader);

	return shader;
}
std::string ResourceLoader::loadFile(string fname){
	std::ifstream in(fname, std::ios::in | std::ios::binary);
  if(in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return(contents);
  }
	else {
		cout << "Unable to open file " << fname << endl;
		exit(1);
	}
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
	// glDeleteBuffers(3, VBO);
	// glDeleteBuffers(1, &IBO);
	// glDeleteVertexArrays(1, &VAO);
	// for(auto &it : textures)
		// glDeleteTextures(1, &it.second);
	// for(auto &it : textures)
		// glDeleteTextures(1, &it.second);

	cerr<<"~Resources"<<endl;
}
Scene::~Scene(){
		std::cerr<<"delete Scene"<<std::endl;
}

void Graph::draw(){
	// if(flags & LastX)
		// Engine::b_vec2_16k.update(data, data.size() - getCount(), data.size());
	// else
		Engine::b_vec2_16k.update(data);

	auto shader = shaders["Lines2D"];
	glUseProgram(shader);

	glUniform(shader, getProjection(), "uPV");
	glUniform(shader, colorHex(dataColor), "uColor");
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

	std::cout<<"tex & FBO "<<texID<<" "<<fbo<<endl;
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

