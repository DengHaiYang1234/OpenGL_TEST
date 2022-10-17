#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 col1 = texture(texture1,TexCoords);
    FragColor = col1;
}
