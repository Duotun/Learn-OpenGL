#pragma once
#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include <glad/glad.h>

class VBO
{
public:

	//Reference ID of the Vertex Buffer Object
	GLuint ID;

	//Constructot that GENERATES A VBO and LINKS it to vertices
	VBO(GLfloat* vertices, GLsizeiptr size, int drawMode = GL_STATIC_DRAW);

	VBO();   // set glBindBuffer later

	virtual void BindBufferData(GLfloat* vertices, GLsizeiptr size, int drawMode = GL_STATIC_DRAW);

	//Bind the VBO
	virtual void Bind();

	//Unbinds the VBO
	virtual void UnBind();

	//Delete the VBO
	virtual void Delete();

};

//Add EBO Definition here as well
class EBO
{

public: 

	//Reference ID of the Vertex Buffer Object
	GLuint ID;

	EBO(GLuint* indices, GLsizeiptr size, int drawMode= GL_STATIC_DRAW);
	//Bind the VBO
	void Bind();

	//Unbinds the VBO
	void UnBind();

	//Delete the VBO
	void Delete();

};

#endif // !VBO_CLASS_H
