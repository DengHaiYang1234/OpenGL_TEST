#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0

uniform mat4 model;

void main()
{
    gl_Position = model *  vec4(aPos, 1.0);
}
