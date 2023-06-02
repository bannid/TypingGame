/* date = May 26th 2023 1:24 pm */

#ifndef GL_SHADER_H
#define GL_SHADER_H

#define T_GL_SHADERS_MAX_UNIFORMS 20
#define T_GL_SHADERS_UNIFORM_NAME_MAX_LENGTH 16

struct gl_uniform
{
	char Name[T_GL_SHADERS_MAX_UNIFORMS];
	GLint Location;
	GLenum Type;
};

struct gl_shader 
{
	char Name[20];
	GLint ProgramID;
	i32 NumberOfUniforms;
	gl_uniform Uniforms[T_GL_SHADERS_UNIFORM_NAME_MAX_LENGTH];
};

#endif //GL_SHADER_H
