#version 330 core
layout (location = 0) in vec2 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aColor; //位置变量的属性位置为 0

out VS_OUT
{
    vec3 color;
}vs_out;

void main()
{
    gl_Position = vec4(aPos.x,aPos.y,0,1.0);
    vs_out.color = aColor;
}
