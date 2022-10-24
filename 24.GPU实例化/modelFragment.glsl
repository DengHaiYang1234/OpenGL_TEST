#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform sampler2D texture_diffuse1;

in vec2 TexCoords;

void main()
{
    FragColor = texture(texture_diffuse1,TexCoords);
}
