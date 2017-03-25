#include "texture_manager.h"
#include <SDL.h>
#include <SDL_image.h>


TextureManager::TextureManager() {
  texture_directory.clear();
}

GLuint TextureManager::GetTexture(const char* path) {
  if (texture_directory.find(path) != texture_directory.end()) {
    return texture_directory[path];
  } else {
    GLuint new_texture_id = 0;
    SDL_Surface* surface = IMG_Load(path);

    // TODO - error checking?
    if (surface == nullptr) {
      printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
      return 0;
    }

    glGenTextures(1, &new_texture_id);
    glBindTexture(GL_TEXTURE_2D, new_texture_id);

    int texture_format = (surface->format->BytesPerPixel == 4) ? GL_RGBA : GL_RGB;

    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, surface->w, surface->h,
        0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    texture_directory[path] = new_texture_id;
    return new_texture_id;
  }
}


void TextureManager::ClearAllTextures() {

  for (std::map<std::string, GLuint>::iterator itr = texture_directory.begin();
      itr != texture_directory.end(); ++itr) {
    glDeleteTextures(1, &itr->second);
  }
  texture_directory.clear();
}