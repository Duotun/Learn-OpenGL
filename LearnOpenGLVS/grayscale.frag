#version 330
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
	FragColor = texture(screenTexture, TexCoords);
	//make it gray by adding weighted color channels 
	float average = 0.2126f * FragColor.r + 0.7152f*FragColor.g + 0.0722f * FragColor.b;
	FragColor = vec4(average, average, average, 1.0);
}

