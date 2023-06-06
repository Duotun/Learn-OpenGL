#pragma once
#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include<glad/glad.h>
#include "stb_image.h"
#include "shader.hpp"  //need to bind the texture with shader

class Texture {

public:

	GLuint ID;
	GLenum texType;  //GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D for example
	GLenum pixelType;  //the image data type 
	GLint imageType; 
	GLenum texSlot;
	
	//inImageType refers to the image representation type like R8, RGBA, R8_Unorm
	//inpixelType refers to the image data type
	Texture(const char* imagePath, GLenum intexType, GLenum slot, GLint inImageType, GLenum inDataType = GL_UNSIGNED_BYTE, GLenum inpixelType = GL_RGB, GLint wrapMode = GL_REPEAT, bool gammaCorrection = false);
	//Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);  //activate and set sampler for the texture

	void Activate();

	void Bind();

	void UnBind();

	void Delete();

};


#endif // !TEXTURE_CLASS_H
