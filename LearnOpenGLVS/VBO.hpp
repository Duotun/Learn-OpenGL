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

	virtual void BindBufferData(const void* vertices, GLsizeiptr size, int drawMode = GL_STATIC_DRAW);

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

	EBO();
	EBO(GLuint* indices, GLsizeiptr size, int drawMode= GL_STATIC_DRAW);
	//Bind the EBO
	void Bind();

	//Unbinds the EBO
	void UnBind();

	//Bind the BufferData
	void BindBufferData(GLuint* indices, GLsizeiptr size, int drawMode = GL_STATIC_DRAW);

	//Delete the EBO
	void Delete();


};

#endif // !VBO_CLASS_H
