#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aColor; //颜色变量的属性位置为 1
layout (location = 2) in vec2 aTexCoord; //纹理采样坐标位置为 2

out vec3 ourColor;
out vec2 TexCoord;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos.x,aPos.y, aPos.z, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
