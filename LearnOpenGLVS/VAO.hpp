#pragma once
#ifndef  VAO_CLASS_H
#define  VAO_CLASS_H

#include <glad/glad.h>
#include "vbo.hpp"

class VAO
{
public:
	//ID references for the Vertex Array Object
	GLuint ID;
	//Construct the VAO
	VAO();

	//link VBO with attributeArrays
	void linkVBO(VBO &inVBO, GLuint layout);
	void linkVBO(VBO &inVBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);

	//Binds the VAO
	void Bind();

	//Unbinds the VAO
	void Unbind();

	//Deletes the VAO
	void Delete();

	int linked = 0;   //indicating linked at least one VBO
};

#endif // ! VAO_CLASS_H
