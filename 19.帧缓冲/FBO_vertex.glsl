#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec2 aTexCoords; //纹理坐标的属性位置 1

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    
    gl_Position = vec4(aPos.x,aPos.y,0.0,1.0);
}
