#version 330
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0f - 1.0f;   //back to NDC
	return (2.0*near *far)/(far + near - z *(far-near));  //substitute depth=z+1/2 back
}

void main()
{
	FragColor = texture(texture1, TexCoords);

	//try depth visualization
	//float depth = LinearizeDepth(gl_FragCoord.z)/far;
	//FragColor = vec4(vec3(depth), 1/0);
}