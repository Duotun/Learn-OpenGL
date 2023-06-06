#include "VAO.hpp"

//Constructor
VAO::VAO()
{
	glGenVertexArrays(1, &ID);
}

//Link a VBO
void VAO::linkVBO(VBO& inVBO, GLuint layout)
{
	inVBO.Bind();
	glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(layout);
	inVBO.UnBind();

	linked = 1;
}

void VAO::linkVBO(VBO& inVBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	inVBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	inVBO.UnBind();

	linked = 1;
}

//Bind a VAO
void VAO::Bind()
{
	glBindVertexArray(ID);
}

//Unbind a VAO
void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}