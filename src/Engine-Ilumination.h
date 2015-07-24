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
