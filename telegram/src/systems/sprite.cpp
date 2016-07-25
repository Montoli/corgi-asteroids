#include "sprite.h"
#include "transform.h"
#include "common.h"
#include "GL/glew.h"

#include <stdio.h>
#include <stdlib.h>


CORGI_DEFINE_SYSTEM(SpriteSystem, SpriteData)

void SpriteSystem::AddPointToBuffer(vec4 p, vec2 uv, vec4 tint) {
	vertex_buffer_[buffer_length_++] = p.x();
	vertex_buffer_[buffer_length_++] = p.y();
	vertex_buffer_[buffer_length_++] = p.z();
	vertex_buffer_[buffer_length_++] = 0.0f;	// u
	vertex_buffer_[buffer_length_++] = 0.0f;	// v
	vertex_buffer_[buffer_length_++] = tint.x();	// r
	vertex_buffer_[buffer_length_++] = tint.y();	// g
	vertex_buffer_[buffer_length_++] = tint.z();	// b
	vertex_buffer_[buffer_length_++] = tint.w();	// a
	assert(buffer_length_ < kTotalBufferSize);
	buffer_count_++;
}

void SpriteSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
	// build our vertex buffer:
	buffer_length_ = 0;
	buffer_count_ = 0;
	for (auto itr = begin(); itr != end(); ++itr) {
		corgi::EntityRef entity = itr->entity;
		TransformData* transform_data = Data<TransformData>(entity);
		SpriteData* sprite_data = Data<SpriteData>(entity);
		
		float width = sprite_data->size.x();
		float height = sprite_data->size.y();
		
		vec4 origin_offset = vec4(transform_data->origin.x(),
				transform_data->origin.y(), 0.0f, 0.0f);
		vec4 p1 = vec4(0.0f,  0.0f,   0.0f, 1.0f) - origin_offset;
		vec4 p2 = vec4(width, 0.0f,   0.0f, 1.0f) - origin_offset;
		vec4 p3 = vec4(0.0f,  height, 0.0f, 1.0f) - origin_offset;
		vec4 p4 = vec4(width, height, 0.0f, 1.0f) - origin_offset;

		mat4 transform_matrix = transform_data->GetTransformMatrix();
		p1 = transform_matrix * p1;
		p2 = transform_matrix * p2;
		p3 = transform_matrix * p3;
		p4 = transform_matrix * p4;

		// todo - convert this to an index buffer, save some space
		AddPointToBuffer(p1, vec2(0, 0), sprite_data->tint);
		AddPointToBuffer(p2, vec2(0, 0), sprite_data->tint);
		AddPointToBuffer(p3, vec2(0, 0), sprite_data->tint);

		AddPointToBuffer(p2, vec2(0, 0), sprite_data->tint);
		AddPointToBuffer(p3, vec2(0, 0), sprite_data->tint);
		AddPointToBuffer(p4, vec2(0, 0), sprite_data->tint);

		if (buffer_length_ >= kTotalBufferSize - 10) break;
	}
}

const char vShaderStr[] =
"attribute vec4 a_vertex;          \n"
"uniform mat4 u_mvp;               \n"
"attribute vec2 a_tex_uv;          \n"
"attribute vec4 a_tint;            \n"
"varying vec4 v_tint;              \n"
"void main() {                     \n"
"  v_tint = a_tint;                \n"
"  gl_Position = u_mvp * a_vertex;   \n"
"}                                 \n";

const char fShaderStr[] =
"varying vec4 v_tint;                         \n"
"void main() {                                \n"
"  gl_FragColor = vec4(v_tint.xyz, 1);        \n"
"  //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); \n"
"}                                            \n";

void SpriteSystem::DeclareDependencies() {
  DependOn<TransformSystem>(corgi::kExecuteAfter, corgi::kReadAccess);
	DependOn<CommonSystem>(corgi::kNoOrderDependency, corgi::kReadAccess);
	SetIsThreadSafe(true);
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
	mat4 vp_matrix = mat4::Ortho(0.0f, 640.0f, 480.0f, 0.0f, -1.0f, 1.0f, 1.0f);


	
	GLfloat vVertices[] = { 
		0.0f,  0.0f, 0.0f,    0.0f, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
		100.0f, 100.0f, 0.0f,   0.0f, 0.0f,  1.0f, 1.0f, 0.0f, 1.0f,
		100.0f, 0.0f,  0.0f,   0.0f, 0.0f,  0.0f, 1.0f, 1.0f, 1.0f };

	// Set the viewport
	glViewport(0, 0, static_cast<GLsizei>(common->screen_size.x()),
		static_cast<GLsizei>(common->screen_size.y()));

	// Clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Use the program object
	glUseProgram(shader_program);

	GLint loc = glGetUniformLocation(shader_program, "u_mvp");
	if (loc != -1) {
		glUniformMatrix4fv(loc, 1, false, &vp_matrix[0]);
	}

	int stride = sizeof(GLfloat) * 9;

	glVertexAttribPointer(kVertexLoc, 3, GL_FLOAT, GL_FALSE, stride, vertex_buffer_ + 0);
	glVertexAttribPointer(kTextureUVLoc, 2, GL_FLOAT, GL_FALSE, stride, vertex_buffer_ + 3);
	glVertexAttribPointer(kTintLoc, 4, GL_FLOAT, GL_FALSE, stride, vertex_buffer_ + 5);


	glEnableVertexAttribArray(kVertexLoc);
	glEnableVertexAttribArray(kTextureUVLoc);
	glEnableVertexAttribArray(kTintLoc);


	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_TRIANGLES, 0, buffer_count_);
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