#version 330 
out vec4 FragColor;

in vec4 outColor;    //set the variable through gl c++ cpu part 

void main()
{
	FragColor = outColor;
}