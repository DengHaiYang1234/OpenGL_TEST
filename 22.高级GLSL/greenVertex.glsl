#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};


uniform mat4 model;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
