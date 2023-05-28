#version 330
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

in vec3 fragPos;
in vec3 Normal;

void main()
{
	//simplified ambient Strength 
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float ambitentStrength = 0.1;
	vec3 ambient = ambitentStrength * lightColor;
	
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);  //reflect function expects from light toward space
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);  //pow for shineness
	vec3 specular = spec* lightColor;

	vec3 result = (ambient+diffuse+spec) * objectColor;  //blend with the object color finally
	FragColor =vec4(result, 1.0);
}
