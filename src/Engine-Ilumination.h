// https://msdn.microsoft.com/en-us/library/windows/desktop/ee416324%28v=vs.85%29.aspx
glm::mat4 calculateShadowProjection(){
  return glm::perspective(1.5f, 1.f, 1.f, 50.f);
}

/// R32F target
void renderShadowMap(GLuint target, Scene &scene, const glm::mat4 &projection){

}

/// R8 target, R32F source
void renderScreenSpaceShadowing(GLuint target, GLuint source){

}

void blurR8(GLuint target, GLuint source){
/// blur to 1/4 screen size
}

GLuint screen_full_R8;
GLuint screen_quarter_R8;

GLuint drawShadows(Scene &scene){
  auto projection = calculateShadowProjection();
  renderShadowMap(shadowMap, scene, projection);
  
  GLuint screenSpaceShadows = screen_full_R8;
  renderScreenSpaceShadowing(screenSpaceShadows, shadowMap);
  
  if(BLUR_SHADOWS){
    blur(screen_quarter_R8, screen_full_R8);
    screenSpaceShadows = screen_quarter_R8;
  }
  return screenSpaceShadows;
}

void bigLight(Scene &scene){
  if(SHADOWS_ENABLED){
    shader = shaders["GlobalLightWithShadowMap"];
    shadow = drawShadows(scene);
  }
  else {
    shader = shaders["GlobalLightWithoutShadowMap"];
    shadow = 0;
  }
  
  glUseProgram(shader);
  
  auto uPV = glGetUniformLocation(shader,"uPV");
	auto uView = glGetUniformLocation(shader,"uView");
	auto uNormalBuffer = glGetUniformLocation(shader,"uNormalBuffer");
	auto uDepthBuffer = glGetUniformLocation(shader,"uDepthBuffer");
	if(shadow)
	  auto uDepthBuffer = glGetUniformLocation(shader,"uShadowMap");
		
	auto uEyePos = glGetUniformLocation(shader,"uEyePos");
  
  auto uLightPos = glGetUniformLocation(shader,"uLightPos");
  auto uColor = glGetUniformLocation(shader,"uColor");
  auto uSize = glGetUniformLocation(shader,"uSize");
  auto uEnergy = glGetUniformLocation(shader,"uEnergy");
  
  glUniform(shader, camera.ProjectionMatrix, uPV);
	glUniform(shader, camera.ViewMatrix, uView);
	glActiveTexture(GL_TEXTURE0);
  	glUniform1i(uNormalBuffer,0);
  	glBindTexture(GL_TEXTURE_2D, normalBuffer);
	glActiveTexture(GL_TEXTURE1);
  	glUniform1i(uDepthBuffer,1);
  	glBindTexture(GL_TEXTURE_2D, depthBuffer2);

	glUniform(shader, camera.eyePosition, uEyePos);
	glUniform(shader, shadowProjection, u_shadowProjection);
	glUniform(shader, scene.pointLamps[0].position, uLightPos);
	glUniform(shader, scene.pointLamps[0].color, uColor);
	glUniform(shader, scene.pointLamps[0].falloffDistance, uSize);
	glUniform(shader, scene.pointLamps[0].energy, uEnergy);
  
  
  
}
