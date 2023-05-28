#version 330

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;  //just like a texture stuff isn't affected by the lights
	float shininess;
};

struct Light{
	vec3 position;
	vec3 ambient;  //light components for different case
	vec3 diffuse;
	vec3 specular;
	
};

uniform Material material;
uniform Light light;

uniform vec3 objectColor;
uniform vec3 viewPos;

in vec3 fragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
	//simplified ambient Strength 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));

	vec3 ambient = vec3(texture(material.diffuse,TexCoords)) * light.ambient;
	
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  //reflect function expects from light toward space
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);  //pow for shineness
	vec3 specular = (spec*vec3(texture(material.specular, TexCoords))) * light.specular;

	vec3 emission = vec3(texture(material.emission, TexCoords));
	vec3 result = (ambient+diffuse+spec+emission);  // light with object colors adding together
	FragColor =vec4(result, 1.0);
}
