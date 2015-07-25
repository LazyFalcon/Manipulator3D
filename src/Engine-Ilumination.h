/// https://msdn.microsoft.com/en-us/library/windows/desktop/ee416324%28v=vs.85%29.aspx
/// http://encelo.netsons.org/tag/hdr/

/// /////////////////////
/// /////////////////////
/// /////////////////////
/// /////////////////////
/// /////////////////////
/// /////////////////////

void fullscreenFBOWithoutDepth(GLuint target){
		glBindFramebuffer(GL_FRAMEBUFFER, fullFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target, 0);
		glDrawBuffers(1,&DrawBuffers[0]);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDepthMask(GL_FALSE);
		setupBuffer(screenQuad);
}
void bindTexture(GLuint id, u32 num){
	glActiveTexture(GL_TEXTURE0 + num);
	glBindTexture(GL_TEXTURE_2D, id);
}
void bindTexture(GLuint shader, GLuint id, const std::string name, u32 num){
	glActiveTexture(GL_TEXTURE0 + num);
	glUniform1i(glGetUniformLocation(shader, name.c_str()),num);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);

}

/// RGBA16F, full size
void renderGlobalLight(GLuint target16, GLuint diffuse8, GLuint normals, GLuint depth){
	auto shader = shaders["GlobalLightWithoutShadowMap"];
	fullscreenFBOWithoutDepth(target16);
	bindTexture(shader, diffuse8, "uDiffuseTex", 0);
	bindTexture(shader, normals, "uNormalTex", 1);
	bindTexture(shader, depth, "uDepthTex", 2);

	auto uEyePos = glGetUniformLocation(shader,"uEyePos");
	auto uInvPV = glGetUniformLocation(shader,"uInvPV");

	auto uLightDir = glGetUniformLocation(shader,"uLightDir");
	auto uColor = glGetUniformLocation(shader,"uColor");
	auto uEnergy = glGetUniformLocation(shader,"uEnergy");
	glUniform(shader, glm::normalize(glm::vec4(10,10,-15,0)), uLightDir);
	glUniform(shader, glm::vec4(1,0.97,0.9,1), uColor);
	glUniform(shader, 15.f, uEnergy);
	glUniform(shader, glm::inverse(camera.ProjectionMatrix*camera.ViewMatrix), uInvPV);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
}

/// blur to target
void renderLights(GLuint target, Scene &scene){

}

void brightPass(GLuint target, GLuint source){

}
void bloom(GLuint target, GLuint source){}

/// RGB16F -> RGB8
void toneMapping(GLuint target8, GLuint source16){
	auto shader = shaders["ToneMapping"];
	fullscreenFBOWithoutDepth(target8);
	bindTexture(source16, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindTexture(GL_TEXTURE_2D, 0);
}

/// -> RGB8, trget is 16f,
void HDR(GLuint buffer16f, GLuint diffuse, GLuint normals, GLuint depth, Scene &scene){
	renderGlobalLight(buffer16f, diffuse, normals, depth);
	// renderLights(buffer16f, scene);
	// brightPass(buffer16f, scene);
	// bloom(buffer16f, scene);
	toneMapping(diffuse, buffer16f);


}



