#include "sprite.h"
#include "transform.h"
#include "common.h"
#include "GL/glew.h"

#include <stdio.h>
#include <stdlib.h>


CORGI_DEFINE_SYSTEM(SpriteSystem, SpriteData)

void SpriteSystem::AddPointToBuffer(BufferInfo& buffer, vec4 p, vec2 uv, vec4 tint) {
  int index = buffer.start_index + buffer.length;
	vertex_buffer_[index++] = p.x();
	vertex_buffer_[index++] = p.y();
	vertex_buffer_[index++] = p.z();
	vertex_buffer_[index++] = uv.x();	// u
	vertex_buffer_[index++] = uv.y();	// v
	vertex_buffer_[index++] = tint.x();	// r
	vertex_buffer_[index++] = tint.y();	// g
	vertex_buffer_[index++] = tint.z();	// b
	vertex_buffer_[index++] = tint.w();	// a
  buffer.length = index - buffer.start_index;
  buffer.count++;
}

void SpriteSystem::UpdateAllEntities(corgi::WorldTime delta_time) {
  // First pass:  Just count how many sprites there are of each
  // texture so we know how to map out vertex memory:
  tex_count.clear();
  for (auto itr = begin(); itr != end(); ++itr) {
    if (tex_count.find(itr->data.texture) == tex_count.end()) {
      tex_count[itr->data.texture] = 1;
    } else {
      tex_count[itr->data.texture]++;
    }
  }

  // Now figure out offsets into our buffer, for where each texture
  // batch of sprites should live.
  buffer.clear();
  int index = 0;
  for (auto itr = tex_count.begin(); itr != tex_count.end(); ++itr) {
    buffer[itr->first] = BufferInfo(index);
    index += (1 + itr->second) * kFloatsPerPoint * kPointsPerSprite;
  }

	for (auto itr = begin(); itr != end(); ++itr) {
		corgi::Entity entity = itr->entity;
		TransformData* transform_data = Data<TransformData>(entity);
		SpriteData* sprite_data = Data<SpriteData>(entity);
    BufferInfo b_info = buffer[sprite_data->texture];
		
		float width = sprite_data->size.x();
		float height = sprite_data->size.y();
		
		vec4 origin_offset = vec4(transform_data->origin.x(),
				transform_data->origin.y(), 0.0f, 0.0f);

    float depth = transform_data->position.z();

		vec4 p1 = vec4(0.0f,  0.0f,   depth, 1.0f) - origin_offset;
		vec4 p2 = vec4(width, 0.0f,   depth, 1.0f) - origin_offset;
		vec4 p3 = vec4(0.0f,  height, depth, 1.0f) - origin_offset;
		vec4 p4 = vec4(width, height, depth, 1.0f) - origin_offset;

		mat4 transform_matrix = transform_data->GetTransformMatrix();
		p1 = transform_matrix * p1;
		p2 = transform_matrix * p2;
		p3 = transform_matrix * p3;
		p4 = transform_matrix * p4;

		AddPointToBuffer(b_info, p1, vec2(0, 0), sprite_data->tint);
		AddPointToBuffer(b_info, p2, vec2(1, 0), sprite_data->tint);
		AddPointToBuffer(b_info, p3, vec2(0, 1), sprite_data->tint);

		AddPointToBuffer(b_info, p2, vec2(1, 0), sprite_data->tint);
		AddPointToBuffer(b_info, p3, vec2(0, 1), sprite_data->tint);
		AddPointToBuffer(b_info, p4, vec2(1, 1), sprite_data->tint);

    // shove it back into the map now that we've updated it.
    buffer[sprite_data->texture] = b_info;
	}
}

const char vShaderStr[] =
"attribute vec4 a_vertex;          \n"
"uniform mat4 u_mvp;               \n"
"attribute vec2 a_tex_uv;          \n"
"attribute vec4 a_tint;            \n"
"varying vec4 v_tint;              \n"
"varying vec2 v_tex_uv;            \n"
"void main() {                     \n"
"  v_tint = a_tint;                \n"
"  v_tex_uv = a_tex_uv;            \n"
"  gl_Position = u_mvp * a_vertex;   \n"
"}                                 \n";

const char fShaderStr[] =
"uniform sampler2D tex; //this is the texture                   \n"
"varying vec4 v_tint;                         \n"
"varying vec2 v_tex_uv; //this is the texture coord              \n"
"void main() {                                                  \n"
"  vec4 color = texture2D(tex, v_tex_uv);                     \n"
"  if (color.a == 0.0) discard;  \n"
"  gl_FragColor = color * v_tint; \n"
"}";

void SpriteSystem::DeclareDependencies() {
  DependOn<TransformSystem>(corgi::kExecuteAfter, corgi::kReadAccess);
	DependOn<CommonSystem>(corgi::kNoOrderDependency, corgi::kReadAccess);
  RequireComponent<TransformSystem>();

	SetIsThreadSafe(true);
}

void SpriteSystem::Cleanup() {
	//Deallocate surface
	SDL_FreeSurface(hello_world);
	hello_world = NULL;

}

void SpriteSystem::InitEntity(corgi::Entity entity) {

}

void SpriteSystem::RenderSprites() {
	CommonComponent* common = entity_manager_->GetSystem<CommonSystem>()->CommonData();
	mat4 vp_matrix = mat4::Ortho(0.0f, 640.0f, 480.0f, 0.0f, -1.0f, 1.0f, 1.0f);

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

	int stride = sizeof(GLfloat) * kFloatsPerPoint;

	glVertexAttribPointer(kVertexLoc, 3, GL_FLOAT, GL_FALSE, stride, vertex_buffer_ + 0);
	glVertexAttribPointer(kTextureUVLoc, 2, GL_FLOAT, GL_FALSE, stride, vertex_buffer_ + 3);
	glVertexAttribPointer(kTintLoc, 4, GL_FLOAT, GL_FALSE, stride, vertex_buffer_ + 5);

	glEnableVertexAttribArray(kVertexLoc);
	glEnableVertexAttribArray(kTextureUVLoc);
	glEnableVertexAttribArray(kTintLoc);

  for (auto itr = buffer.begin(); itr != buffer.end(); ++itr) {
    const char* texture_name = itr->first;
    BufferInfo b_info = itr->second;

    GLuint texture = common->texture_manager->GetTexture(texture_name);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, b_info.start_index / kFloatsPerPoint, b_info.count);
  }
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