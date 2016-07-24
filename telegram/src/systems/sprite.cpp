#include "sprite.h"
#include "transform.h"
#include "common.h"
#include "GL/glew.h"

#include <stdio.h>
#include <stdlib.h>


CORGI_DEFINE_SYSTEM(SpriteSystem, SpriteComponent)
/*
void SpriteSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	//printf("entity updated!");
  for (auto itr = begin(); itr != end(); ++itr) {
    corgi::EntityRef entity = itr->entity;
		// do stuff
  }
}
*/
void SpriteSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	printf("SpriteSystem - starting update!\n");

	for (auto itr = begin(); itr != end(); ++itr) {
		corgi::EntityRef entity = itr->entity;

	}

	printf("SpriteSystem - ending update!\n");


}

const char vShaderStr[] =
"attribute vec4 a_vertex;  \n"
"attribute vec2 a_tex_uv;    \n"
"attribute vec4 a_tint;      \n"
"varying vec4 v_tint;        \n"
"void main() {                \n"
"  v_tint = a_tint;          \n"
"   //v_tint = abs(a_vertex); \n"
"  //v_tint = vec4(1.0, 1.0, 0.0, 1.0); \n"
"  gl_Position = a_vertex;// + a_tint; \n"
"}                           \n";

const char fShaderStr[] =
"varying vec4 v_tint;      \n"
"void main() {                                 \n"
"  gl_FragColor = vec4(v_tint.xyz, 1);                     \n"
"  //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
"}                                            \n";


void SpriteSystem::DeclareDependencies() {
  DependOn<TransformSystem>(corgi::kExecuteAfter, corgi::kReadAccess);
	DependOn<CommonSystem>(corgi::kNoOrderDependency, corgi::kReadAccess);
}


void SpriteSystem::Cleanup() {
	//Deallocate surface
	SDL_FreeSurface(hello_world);
	hello_world = NULL;

}

void SpriteSystem::InitEntity(corgi::EntityRef& entity) {

}

void SpriteSystem::RenderSprites() {
	//	SDL_BlitSurface(hello_world, NULL, screen_surface, NULL);

		//glClearColor(0, 0, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT);
		//SDL_GL_SwapWindow(window);

		//UserData *userData = esContext->userData;

	CommonComponent* common = entity_manager_->GetSystem<CommonSystem>()->CommonData();

	GLfloat vVertices[] =
	{ 0.0f,  0.5f, 0.0f,    0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		0.5f, -0.5f,  0.0f,   0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f };

	// Set the viewport
	glViewport(0, 0, static_cast<GLsizei>(common->screen_size.x()),
		static_cast<GLsizei>(common->screen_size.y()));

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(shader_program);


	int stride = sizeof(GLfloat) * 9;

	glVertexAttribPointer(kVertexLoc, 3, GL_FLOAT, GL_FALSE, stride, vVertices + 0);
	glVertexAttribPointer(kTextureUVLoc, 2, GL_FLOAT, GL_FALSE, stride, vVertices + 3);
	glVertexAttribPointer(kTintLoc, 4, GL_FLOAT, GL_FALSE, stride, vVertices + 5);

	glEnableVertexAttribArray(kVertexLoc);
	glEnableVertexAttribArray(kTextureUVLoc);
	glEnableVertexAttribArray(kTintLoc);


	glDrawArrays(GL_TRIANGLES, 0, 3);
}

SDL_Surface* SpriteSystem::LoadPNG(std::string path) {
	//The final optimized image
	SDL_Surface* optimizedSurface = NULL;
	CommonComponent* common = entity_manager_->GetSystem<CommonSystem>()->CommonData();
	/*
	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		//Convert surface to screen format
		optimizedSurface = SDL_ConvertSurface(loadedSurface, common->screen_surface->format, NULL);
		if (optimizedSurface == NULL) {
			printf("Unable to optimize image %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	*/
	return optimizedSurface;
}


GLuint LoadShader(const char *shaderSrc, GLenum type) {
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
		return 0;

	// Load the shader source
	glShaderSource(shader, 1, &shaderSrc, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		GLint infoLen = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

		if (infoLen > 1) {
			char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));

			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			printf("Error compiling shader:\n%s\n", infoLog);
			free(infoLog);
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;

}


void SpriteSystem::Init() {

	/*
	//The window we'll be rendering to
	SDL_Window* gWindow = NULL;

	//The surface contained by the window
	SDL_Surface* gScreenSurface = NULL;

	//The image we will load and show on the screen
	*/
	/*
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		assert(false);
	}
	*/
	/*

	//hello_world = SDL_LoadBMP("rsc\\hello_world.bmp");
	hello_world = LoadPNG("rsc/welshcorgi.png");

	if (hello_world == NULL) {
		printf("Unable to load image! SDL Error: %s\n",  SDL_GetError());
		assert(false);
	}
	*/

	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint programObject;

	// Load the vertex/fragment shaders
	vertexShader = LoadShader(vShaderStr, GL_VERTEX_SHADER);
	fragmentShader = LoadShader(fShaderStr, GL_FRAGMENT_SHADER);

	// Create the program object
	programObject = glCreateProgram();

	if (programObject == 0) {
		assert(false);
		return;
	}

	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);



	// Bind vPosition to attribute 0
	glBindAttribLocation(programObject, kVertexLoc, "a_vertex");
	glBindAttribLocation(programObject, kTextureUVLoc, "a_tex_uv");
	glBindAttribLocation(programObject, kTintLoc, "a_tint");
	

	// Link the program
	glLinkProgram(programObject);

	//assert(kVertexLoc == glGetAttribLocation(programObject, "a_vertex"));
	//assert(kTextureUVLoc == glGetAttribLocation(programObject, "a_tex_uv"));
	//assert(kTintLoc == glGetAttribLocation(programObject, "a_tint"));


	GLint infoLen = 0;
	glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);

	if(infoLen > 1) {
		char* infoLog = static_cast<char*>(malloc(sizeof(char) * infoLen));

		glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
		printf("Error linking program:\n%s\n", infoLog);
		free(infoLog);
		glDeleteProgram(programObject);
		assert(false);
	}


	shader_program = programObject;
}