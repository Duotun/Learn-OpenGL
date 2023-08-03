#pragma once
#ifndef MODEL_H
#define MODEL_H

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
//for model import
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "mesh.h"
#include "shader.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>


unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma =false);

class Model
{
public:
	Model(const char* path)
	{
		loadModel(path);
	}

	void Draw(Shader& shader)
	{
		//loop all the meshes to draw
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			meshes[i].Draw(shader);
		}
	}

private:
	//model data
	std::vector<MeshTexture> textures_loaded;  //store all the textured loaded for the model so far
	std::vector<Mesh> meshes;
	std::string directory;  //for texture loading

	void loadModel(std::string path)
	{
	   //aiScene is the root object of Assimp's data interface
	   Assimp::Importer importer;
	   const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate| aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace);
	   
	   //check for errors
	   if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
	   {
			std::cout<<"ERROR::ASSIMP:: "<<importer.GetErrorString() <<std::endl;
			return ;
	   }

	   //retrieve the directory path of the filepath
	   directory = path.substr(0, path.find_last_of('/'));

	   //process Assimp's root node recursively
	   processNode(scene->mRootNode, scene);
	}
	void processNode(aiNode* node, const aiScene* scene)
	{
		//process meshes at the current stage
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene));
		}

		//recursively process children's nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	//process from assimp into glm for easy control and understanding 
	Mesh processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<MeshTexture>textures;

		//process vertex data
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex vertex;
			glm::vec3 tmpVector;
			tmpVector.x = mesh->mVertices[i].x;
			tmpVector.y = mesh->mVertices[i].y;
			tmpVector.z = mesh->mVertices[i].z;
			vertex.Position = tmpVector;

			//normals
			if (mesh->HasNormals())
			{
				tmpVector.x = mesh->mNormals[i].x;
				tmpVector.y = mesh->mNormals[i].y;
				tmpVector.z = mesh->mNormals[i].z;
				vertex.Normal = tmpVector;
			}

			//texture coordiantes, check the first set of textrue coordinates (although up to 8 is acceptble)
			if (mesh->mTextureCoords[0])  //does the mesh contain texture coordiantes?
			{
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			
				//tangents and bitangents
				/*tmpVector.x = mesh->mTangents[i].x;
				tmpVector.y = mesh->mTangents[i].y;
				tmpVector.z = mesh->mTangents[i].z;
				vertex.Tangent = tmpVector;
				tmpVector.x = mesh->mBitangents[i].x;
				tmpVector.y = mesh->mBitangents[i].y;
				tmpVector.z = mesh->mBitangents[i].z;
				vertex.Bitangent = tmpVector;
				*/
			}
			else vertex.TexCoords= glm::vec2(0.0f, 0.0f);

			vertices.push_back(vertex);
		}
		//std::cout<<"Vertices: "<<vertices.size()<<std::endl;

		//process indices for mesh's faces
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		//process material 
		if (mesh->mMaterialIndex >= 0)  //check do has the material
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			//diffuse maps
			std::vector<MeshTexture> diffuseMaps = loadMaterialTerxtures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			//specular maps
			std::vector<MeshTexture> specularMaps = loadMaterialTerxtures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			//normal maps
			std::vector<MeshTexture> normalMaps = loadMaterialTerxtures(material, aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			//height maps
			std::vector<MeshTexture> heightMaps = loadMaterialTerxtures(material, aiTextureType_AMBIENT, "texture_height");
			textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
		}
		
		//return a mesh object created from the extracted mesh data from files
		return Mesh(vertices, indices, textures);
	}
	//loop all the texture locations according to the type
	std::vector<MeshTexture> loadMaterialTerxtures(aiMaterial* mat, aiTextureType type,
		std::string typeName)
	{
		std::vector<MeshTexture> textures;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < textures_loaded.size(); j++)
			{
				if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
				{
				   textures.push_back(textures_loaded[j]);
				   skip = true;
				   break;
				}
			}
			if (!skip)
			{
				//if texture hasn't been loaded already, load it
				MeshTexture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.C_Str();
				textures.push_back(texture);
				textures_loaded.push_back(texture);
			}
		}

		return textures;
	}


};
	unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
	{
		std::string filename = std::string(path);
		filename = directory + '/' + filename;

		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, nrComponents;
		unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
		if (data)
		{
			GLenum format;
			if (nrComponents == 1)
				format = GL_RED;
			else if (nrComponents == 3)
				format = GL_RGB;
			else if (nrComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			std::cout << "Texture failed to load at path: " << path << std::endl;
			stbi_image_free(data);
		}

		return textureID;
	}


#endif // !MODEL_H
