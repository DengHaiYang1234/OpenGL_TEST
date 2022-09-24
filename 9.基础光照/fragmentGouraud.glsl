#version 330 core
out vec4 FragColor;

in vec3 ResultColor;//顶点世界坐标

void main()
{
    FragColor = vec4(ResultColor,1);
}
