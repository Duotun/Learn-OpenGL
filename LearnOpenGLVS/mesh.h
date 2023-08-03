#pragma once
#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include "shader.hpp"
#include "VAO.hpp"


struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	//glm::vec3 Tangent;
	//glm::vec2 Bitangent;

};

struct MeshTexture
{
	unsigned int id = 0;
	std::string type;
	std::string path;  //store the image path of the texture to compare with others
};

class Mesh
{
public:
	//mesh data
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<MeshTexture> textures;

	Mesh(std::vector<Vertex> inVertices, std::vector<unsigned int>inIndices, std::vector<MeshTexture> inTextures)
	{
		this->vertices = inVertices;
		this->indices = inIndices;
		this->textures = inTextures;

		setupMesh();
	}

	void Draw(Shader& shader)
	{
		//bind appropriate textures
		unsigned int diffuseNr = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr = 1;
		unsigned int heightNr = 1;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0+i);  //activate proper texgture before binding

			//retrieve texture number (the N in diffuseN)
			std::string number;
			std::string name = textures[i].type;
			if(name == "texture_diffuse")
				number = std::to_string(diffuseNr++);
			else if(name == "texture_specular")
				number = std::to_string(specularNr++);
			else if(name == "texture_normal")
				number = std::to_string(normalNr++);
			else if(name == "texture_height")
				number = std::to_string(heightNr++);

		    //set the sampler to the correct tecture unit
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
			// and finally bind the texture with ID
			glBindTexture(GL_TEXTURE_2D, textures[i].id);

		}
		
		//draw mesh
		glBindVertexArray(_vao.ID);
	    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		//alwasy good to set everythign back to defaults once configured
		glActiveTexture(GL_TEXTURE0);

	}

	VAO _vao;
	VBO _vbo;
	EBO _ebo;

private:

	//render data (VBO and VAO, EBO)

	//set up for drawings by setting up VAO and VBO, EBO
	void setupMesh()
	{
		_vao.Bind();

		_vbo.BindBufferData(&vertices[0], vertices.size() * sizeof(Vertex), GL_STATIC_DRAW);
		_ebo.BindBufferData(&indices[0], indices.size() * sizeof(unsigned int), GL_STATIC_DRAW);


		//set up positions, normals and texture coords
		_vao.linkVBO(_vbo, 0, 3, GL_FLOAT, sizeof(Vertex),(void*)0);
		_vao.linkVBO(_vbo, 1, 3, GL_FLOAT, sizeof(Vertex),(void*)(offsetof(Vertex, Normal)));
		_vao.linkVBO(_vbo, 2, 2, GL_FLOAT, sizeof(Vertex),(void*)(offsetof(Vertex, TexCoords)));

		_vao.Unbind();

	}
};



#endif