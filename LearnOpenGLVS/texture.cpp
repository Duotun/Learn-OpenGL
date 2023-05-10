#include "texture.hpp"

Texture::Texture(const char* imagePath, GLenum intexType, GLenum slot, GLint inImageType, GLenum inDataType, GLenum inpixelType, GLint wrapMode)
{
	//assign properties
	texType = intexType;
	pixelType = inpixelType;  //generally the
	imageType = inImageType;
	texSlot = slot;

	//stores the width, height and the numberof color channels of the image
	int texWidth, texHeight, nrChannels;
	stbi_set_flip_vertically_on_load(true);  //flip the y-coordinates starting point
	unsigned char* data = stbi_load(imagePath, &texWidth, &texHeight, &nrChannels, 0);
	if (data == nullptr)
	{
		std::cout << "Failed to load texture" << std::endl;
		return; 
	}

	//Generate a texture object
	glGenTextures(1, &ID);
	Activate();

	
	//Assigns the texture properties
	//configure the wrap mode
	glTexParameteri(texType, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, wrapMode);
	//configure the type of algorith that is used to make the texture smaller or bigger
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Assign the image data in cpu
	glTexImage2D(texType, 0, imageType, texWidth, texHeight, 0, pixelType, GL_UNSIGNED_BYTE, data);  //type is very important, generally channels must match like rgba image type to rgba pixel data
	//Generate mipmap
	glGenerateMipmap(texType);

	//Delete the image data
	stbi_image_free(data);

	UnBind();
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)  //activate and set sampler for the texture
{
	//Activate the shader
	shader.use();
	//Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	glUniform1i(texUni, unit);
}

void Texture::Activate()
{
	glActiveTexture(texSlot);
	glBindTexture(texType, ID);

}

void Texture::Bind()
{
	glBindTexture(texType, ID);
}

void Texture::UnBind()
{
	glBindTexture(texType, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}