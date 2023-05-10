#include "VBO.hpp"

//VBO Part
VBO::VBO(GLfloat* vertices, GLsizeiptr size, int drawMode)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ARRAY_BUFFER, ID);
	glBufferData(GL_ARRAY_BUFFER, size, vertices, drawMode);
}

//Binds the VBO
void VBO::Bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, ID);
}

//Unbind the VBO
void VBO::UnBind()
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//Delete the VBO
void VBO::Delete()
{
	glDeleteBuffers(1, &ID);
}

//EBO Part, Bind with Creation
EBO::EBO(GLuint* indices, GLsizeiptr size, int drawMode)
{
	glGenBuffers(1, &ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, drawMode);
}

//Binds the VBO
void EBO::Bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
}

//Unbind the VBO
void EBO::UnBind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//Delete the VBO
void EBO::Delete()
{
	glDeleteBuffers(1, &ID);
}