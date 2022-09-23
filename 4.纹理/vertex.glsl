#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aColor; //颜色变量的属性位置为 1
layout (location = 2) in vec2 aTexCoord; //纹理采样坐标位置为 2

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos.x,aPos.y, aPos.z, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
