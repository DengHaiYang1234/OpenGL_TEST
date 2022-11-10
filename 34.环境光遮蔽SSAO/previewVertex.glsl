#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec2 aTexCoords; //位置变量的属性位置为 0

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    
    TexCoords = aTexCoords;
}
