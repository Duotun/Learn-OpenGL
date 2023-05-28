#version 330
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float time;
const float offset = 1.0/300.0;

void main()
{
	//define offsets for accessing neighbor pixels
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset, 0.0f),
		vec2(-offset, -offset),
		vec2(0.0f, -offset),
		vec2(offset, -offset)
	);

	float kernel[9] = float[](
		 1.0/16.0, 2/16.0, 1/16.0,
		 2.0/16.0, 4/16.0, 2.0/16.0,
		 1.0/16.0, 2.0/16.0, 1.0/16.0
	);

	vec3 sampleTex[9]; vec3 col = vec3(0.0);
	for(int i=0; i<9;i++)
	{
		sampleTex[i] =  vec3(texture(screenTexture, TexCoords.st + offsets[i]*time));
		col += sampleTex[i] * kernel[i];
	}

	FragColor = vec4(col, 1.0);
}

