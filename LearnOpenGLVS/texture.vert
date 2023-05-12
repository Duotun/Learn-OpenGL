#version 330 
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;
layout(location=2) in vec2 aTexCoord;

out vec3 outColor; //output a color to the fragment shader
out vec2 TexCoord;

uniform mat4 transform;   // transform test

void main()
{
	//gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
	gl_Position =  transform * vec4(aPos, 1.0f);
	outColor = aColor;
	TexCoord = aTexCoord;
	}

