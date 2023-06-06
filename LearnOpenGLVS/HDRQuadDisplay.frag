#version 330
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D hdrBuffer;
uniform bool hdr;
uniform float exposure;

void main()
{
	const float gamma = 2.2;  //apply in the end only
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;

	if(hdr)
	{
		//reinhard tone mapping
		//vec3 result = hdrColor / (hdrColor + vec3(1.0));
		
		//exposure tone mapping
		vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
		result = pow(result, vec3(1.0/gamma));  //gamma correction;
		FragColor = vec4(result, 1.0);

	}
	else
	{
		vec3 result = pow(hdrColor, vec3(1.0/gamma));
		FragColor = vec4(result, 1.0);
	}


}