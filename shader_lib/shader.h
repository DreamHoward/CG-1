

#ifndef SHADER_H
#define SHADER_H
//my define
#include "../GL/glew.h"
//---
#include "../GL/glut.h"
//---
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

GLuint createShader(const char *filename, const char *type);
GLuint createProgram(GLuint vert, GLuint frag);

#include <stdio.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h"

#include "shader.h"

GLuint createShader(const char *filename, const char *type)
{
	FILE *fp = fopen(filename, "rb");
	fseek(fp, 0, SEEK_END);
	long fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);  //same as rewind(fp);

	char *source = (char *)malloc(sizeof(char) * (fsize + 1));
	fread(source, fsize, 1, fp);
	fclose(fp);

	source[fsize] = 0;

	GLuint shader;
	if (0 == strcmp(type, "vertex"))
	{
		// GLuint glCreateShader( GLenum shaderType ); //
		shader = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (0 == strcmp(type, "fragment"))
	{
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	}
	else
	{
		puts("Error: Unknown type of shader.");
		free(source);
		return -1;
	}

	/*
	void glShaderSource(GLuint shader,
	GLsizei count,
	const GLchar **string,
	const GLint *length);
	*/
	glShaderSource(shader, 1, (const GLchar **)&source, 0);

	glCompileShader(shader);
	GLint isCompiled = 0;

	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		char *infoLog = (char *)malloc(sizeof(char) * (maxLength));
		glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);

		//We don't need the shader anymore.
		glDeleteShader(shader);

		//Use the infoLog as you see fit.
		puts(infoLog);
		free(infoLog);

		//In this simple program, we'll just leave
		return -1;
	}

	free(source);
	return shader;
}

GLuint createProgram(GLuint vert, GLuint frag)
{
	GLuint program = glCreateProgram();

	//Attach our shaders to our program
	glAttachShader(program, vert);
	glAttachShader(program, frag);

	//Link our program
	glLinkProgram(program);

	//Note the different functions here: glGetProgram* instead of glGetShader*.
	GLint isLinked = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &isLinked);

	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		char *infoLog = (char *)malloc(sizeof(char) * (maxLength));
		glGetProgramInfoLog(program, maxLength, &maxLength, infoLog);

		//We don't need the program anymore.
		glDeleteProgram(program);
		//Don't leak shaders either.
		glDeleteShader(vert);
		glDeleteShader(frag);

		//Use the infoLog as you see fit.
		puts(infoLog);
		free(infoLog);

		//In this simple program, we'll just leave
		return -1;
	}

	//Always detach shaders after a successful link.
	glDetachShader(program, vert);
	glDetachShader(program, frag);

	return program;
}
//my class
class Shader
{
public:
	Shader(const char *vertex_path, const char *fragment_path);
	void UseProgram() { glUseProgram(program); }
	GLuint GetProgram() { return program; }
private:
	std::string readFile(const char *filePath);
	GLuint loadShader(const char *vertex_path, const char *fragment_path);
	GLuint createBasicShader(int shaderCmd, const char * path);
	GLuint compileShader(int shaderCmd, GLuint shader, const char* Source);
	void debugShader(GLuint shader);
	GLuint program;
};


#endif