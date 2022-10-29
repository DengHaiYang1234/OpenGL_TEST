#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aNormal; //法线变量的属性位置为 1
layout (location = 2) in vec2 aTexCoords; //法线变量的属性位置为 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT
{
    vec2 texCoords;
}vs_out;

void main()
{
    //projection * view * model *
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vs_out.texCoords = aTexCoords;
}
