#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include "mathfu/glsl_mappings.h"
#include "GL/glew.h"
#include <map>
#include <string>

using mathfu::quat;
using mathfu::vec2;
using mathfu::vec3;
using mathfu::vec4;
using mathfu::mat3;
using mathfu::mat4;


// Basic texture manager class, so don't
// end up loading the same thing more than once.
class TextureManager {
public:
  TextureManager();

  GLuint GetTexture(const char* path);
  void ClearAllTextures();

private:
  std::map<std::string, GLuint> texture_directory;


};

#endif // TEXTURE_MANAGER_H