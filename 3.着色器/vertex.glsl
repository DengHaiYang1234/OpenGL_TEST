#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aColor; //颜色变量的属性位置为 1

out vec3 VertexPos;

void main()
{
    gl_Position = vec4(aPos.x,aPos.y, aPos.z, 1.0);
    VertexPos = aPos * 0.5f + 0.5f;
}
