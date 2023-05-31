#version 330

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	//sampler2D emission;  //just like a texture stuff isn't affected by the lights
	float shininess;
};

struct DirLight{	
	vec3 direction;  //for directional light, only need the directions of light
	
	vec3 ambient;  //light components for different case
	vec3 diffuse;
	vec3 specular;
	
};
uniform DirLight dirLight;
vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir);

struct PointLight{
	
	vec3 position; 

	vec3 ambient;  //light components for different case
	vec3 diffuse;
	vec3 specular;
	
	float constant;
	float linear;
	float quadratic;

};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  //fragpos used for the attenuations

//utilize the comparison between cosines of angles to determine the light range
//for the learning demo, we assume 1 spotlight following with the camera position
struct SpotLight
{
	vec3 position;
	vec3 direction;
	float InnerCutoff;
	float OuterCutoff;
	
	float constant;
	float linear;
	float quadratic;

	vec3 ambient;  //light components for different case
	vec3 diffuse;
	vec3 specular;
};
uniform SpotLight spotLight;
vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos,  vec3 viewDir);


uniform Material material;

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
	vec3 viewDir = normalize(viewPos - fragPos);

	//Phase 1 Directional lighting
	vec3 result = CalDirLight(dirLight, norm, viewDir);

	//Phase 2 point lights
	for(int i=0; i<NR_POINT_LIGHTS; i++)
		result += CalPointLight(pointLights[i], norm, fragPos, viewDir);

	//Phase 3 spot light with camera movement
	result += CalSpotLight(spotLight, norm, fragPos, viewDir);

	//vec3 emission = vec3(texture(material.emission, TexCoords));   //emission as a light
	FragColor =vec4(result, 1.0);
}

vec3 CalDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0f);

	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	return (ambient + diffuse + specular);

}

vec3 CalPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	//specular shading
	vec3 reflectDir = reflect(-lightDir, normal);   //reflect using toward light direction
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	//attenuation
	float dist = length(light.position- fragPos);
	float attenuation = 1.0/(light.constant + light.linear * dist + light.quadratic * dist * dist);

	//combien results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);

}

vec3 CalSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.InnerCutoff - light.OuterCutoff;   //cosine values
	float intensity = clamp((theta - light.OuterCutoff)/epsilon, 0.0f, 1.0f);

	//diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	//spec shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(reflectDir, viewDir),0.0), material.shininess);

	//attenuation 
	float dist = length(light.position - fragPos);
	float attenuation = 1.0/(light.constant + light.linear * dist + light.quadratic * dist * dist);

	//combine the results together 
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	vec3 diffuse = light.diffuse * diff  * vec3(texture(material.diffuse, TexCoords));
	
	diffuse *= intensity;
	specular *= intensity;
	ambient *= intensity;

	diffuse *=attenuation;
	specular *= attenuation;
	ambient *= attenuation;

	return ambient + diffuse + specular;
}
