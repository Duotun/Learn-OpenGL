#version 460 core

out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixval;

void main()
{
   vec2 reverseTexCoord = vec2(TexCoord.x*2.0, TexCoord.y*2.0);   //reverse should be 1 - org, since the range is in [0, 1]
   FragColor = mix(texture(texture1, TexCoord), texture(texture2, reverseTexCoord), mixval);  //20% the second of mix
}
