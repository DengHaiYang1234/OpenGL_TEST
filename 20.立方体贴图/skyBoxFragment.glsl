#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    //天空盒的顶点坐标都为单位向量，所以采样时只需单位向量，不需要关心大小
    FragColor = texture(skybox,TexCoords);
}
