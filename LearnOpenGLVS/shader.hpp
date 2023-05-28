#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	//the program ID
	unsigned int ID;

	//constructor reads and builds the shader
	Shader(const char *vertexPath, const char* fragmentPath);
	//use/activate the shader
	void use();
	void Delete();

	//utility uniform functions for uniform attributes
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value)const;
	void setMat4(const std::string &name, glm::mat4 inMat) const;
	void setVec3(const std::string &name, glm::vec3 inVec) const;
	void setVec3(const std::string& name, float x, float y, float z) const;


private:
	void checkCompileErrors(unsigned int shader, std::string type)
	{
		int success;
		char infoLog[1024];
		if (type != "PROGRAM")   //ALL CAPITALIZED
		{
			//vertex or fragment
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout<<"EROR::SHADER_COMPILATION_ERROR of type" <<type<<" \n"<<infoLog<<"\n -- --------------------------------- --"<<std::endl;
			}
		}
		else
		{
			//check shader program
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				std::cout << "EROR::SHADER_COMPILATION_ERROR of type" << type << " \n" << infoLog << "\n -- --------------------------------- --" << std::endl;
			}
		}
	}

};

#endif // !SHADER_H

