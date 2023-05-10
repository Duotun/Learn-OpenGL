#version 330 
out vec4 FragColor;

in vec3 outColor;    //set the variable through gl c++ cpu part 
in vec2 TexCoord;

uniform sampler2D outTexture0;
uniform sampler2D outTexture1;

void main()
{
	//FragColor = vec4(outColor, 1.0);
	//FragColor = texture(outTexture, TexCoord) * vec4(outColor, 1.0);
	FragColor = mix(texture(outTexture0, TexCoord) , texture(outTexture1, TexCoord*2.0), 0.2);  //80% the first, 20% the second

}