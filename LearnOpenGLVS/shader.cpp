#include "shader.hpp"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	//1.retrieve the vertex/fragment source code from filePaht
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	//ensure ifstream objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	//add ifstrem objects can throw exceptions
	try
	{
		//open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		//read fil's buffer contens into streams
		vShaderStream<< vShaderFile.rdbuf();
		fShaderStream<< fShaderFile.rdbuf();

		//close the file handlers
		vShaderFile.close();
		fShaderFile.close();

		//convert stream into strin
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		 std::cout<<"ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ "<<e.what()<<std::endl;
	}

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	//2. compile the Shader
	unsigned int  vertex, fragment;
	//vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	//fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	//shader program
	ID = glCreateProgram();   //build shader Program ID
	GLenum errorCode;
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");

	//delete the shaders as they are linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);

}

//activate the shader
void Shader::use()
{
	glUseProgram(ID);
}


void Shader::Delete()
{
	glDeleteProgram(ID);
}


void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);

}

void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value)const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

