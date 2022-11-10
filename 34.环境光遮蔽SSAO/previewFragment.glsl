#version 330 core
out vec4 FragColor;

uniform sampler2D image;
in vec2 TexCoords;

void main()
{
    vec3 color = texture(image,TexCoords).rgb;
    FragColor = vec4(color,1);
}
