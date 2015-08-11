enum class SourceFormat : u32
{
    SingleChannel, RGBA
};

SourceFormat getChanels(GLint internalFormat){
    if(internalFormat == GL_R8 || internalFormat == GL_R16F || internalFormat == GL_R32F)
        return SourceFormat::SingleChannel;
    return SourceFormat::RGBA;
}

struct Texture
{
	GLint internalFormat; /// GL_R8, GL_RGBA8, GL_RGBA16F, GL_R32F, GL_DEPTH24_STENCIL8
	float width;        /// --
	float height;       /// --
	GLenum format;        /// GL_RGBA, GL_RED, GL_DEPTH_STENCIL
	GLenum type;          /// GL_UNSIGNED_BYTE, GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_INT_24_8
												///
	GLint filtering;      /// GL_NEAREST, GL_LINEAR
												///
	u32 mipmapLevel;       /// 0, 1
	bool mipmaps;         ///
	GLuint ID;            /// --
	bool isFullScreen;
	bool isHalfScreen;
	SourceFormat channels;

	u32 mipmapLevelToAttach;

	GLuint generate(){
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);

		mipmaps = mipmapLevel > 0;
		if(mipmaps){
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapLevel);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

		genMipmaps();

		glBindTexture(GL_TEXTURE_2D, 0);

		channels = getChanels(internalFormat);

		isFullScreen = (width==window_width) && (height==window_height);
		isHalfScreen = (width==window_width/2) && (height==window_height/2);

		return ID;
	}
	GLuint genMipmaps(){
			if(mipmaps){
					glGenerateMipmap(GL_TEXTURE_2D);
			}
	}
	void bind(u32 sampler = 0){
		glActiveTexture(GL_TEXTURE0+sampler);
		glBindTexture(GL_TEXTURE_2D, ID);
	}
	void bind(GLenum sampler, u32 uniform){
		glActiveTexture(sampler);
		glUniform1i(uniform, sampler-GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ID);
	}
	GLuint get(){
		return ID;
	}
	void setViewport(){
		glViewport(0, 0, width, height);
	}
};

/**
Blur process:

	target -> vertical -> buffer
	buffer -> horizontal -> target

	jak trzeba będzie to downsample zrobi się osobno i uj


Potrzebne bufory:
	R8, full x1, half x2
	RGBA8, full x1, half x2


*/

void blurVertical(Texture &source, Texture &target, float scale = 1.f, float kernelSize = 1.f){
	GLuint shader;
	if(source.internalFormat == GL_R8){
			shader = shaders["BlurVertical_SingleChannel"];
			glUseProgram(shader);
	}
	else {
			shader = shaders["BlurVertical"];
			glUseProgram(shader);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.ID, 0);

	setupBuffer(screenQuad);
	source.bind(0);
	glUniform(shader, kernelSize, "uBlurSize");
	glUniform(shader, screenSize/scale, "uScreenSize");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void blurHorizontal(Texture &source, Texture &target, float scale = 1.f, float kernelSize = 1.f){
	GLuint shader;
	if(source.internalFormat == GL_R8){
			shader = shaders["BlurHorizontal_SingleChannel"];
			glUseProgram(shader);
	}
	else {
			shader = shaders["BlurHorizontal"];
			glUseProgram(shader);
	}
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target.ID, 0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
	setupBuffer(screenQuad);
	source.bind(0);
	glUniform(shader, kernelSize, "uBlurSize");
	glUniform(shader, screenSize/scale, "uScreenSize");
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void blend(Texture &source){
	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_SRC_COLOR);
auto shader = shaders["ApplyFBO"];
	glUseProgram(shader);
	source.bind();

	setupBuffer(screenQuad);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisable(GL_BLEND);
}






















