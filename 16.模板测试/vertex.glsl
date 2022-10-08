#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec2 aTexCoords; //纹理坐标的属性位置 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * model *  vec4(aPos, 1.0);
    
    
}
