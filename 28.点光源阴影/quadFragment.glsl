#version 330 core
out vec4 FragColor;

uniform sampler2D shadowMap;
in vec2 TexCoords;

void main()
{
    vec4 col = texture(shadowMap,TexCoords);
    FragColor = vec4(col.r,col.r,col.r,1.0);
}
