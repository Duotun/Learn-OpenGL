#version 330

out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;

}fs_in;

//diffuse light only  (assume point light)
struct Light{
	 vec3 Position;
	 vec3 Color;

};

uniform Light lights[16];
uniform sampler2D diffuseTexture;

void main()
{
	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
	vec3 normal = normalize(fs_in.Normal);

	//ambient
	vec3 ambient = 0.0 * color;   //vec3(1.5f, 0.0f, 0.0f);

	//lighting 
	vec3 lighting = vec3(0.0f);
	
	for(int i=0; i<16; i++)
	{
		//diffuse with attenuation...
		vec3 lightDir = normalize(lights[i].Position - fs_in.FragPos);
		float diff = max(dot(lightDir, normal), 0.0);
		vec3 diffuse = lights[i].Color * diff * color;
		vec3 result = diffuse;
		float dist = length(fs_in.FragPos - lights[i].Position);
	    result *= 1.0f /(dist * dist);
		lighting += result;

	}
	
	FragColor = vec4(ambient + lighting, 1.0);

}