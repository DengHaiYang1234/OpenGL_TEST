#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aNormal; //法线变量的属性位置为 1
layout (location = 2) in vec2 aTexcoords; //法线变量的属性位置为 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;//顶点世界坐标
out vec3 Normal;
out vec2 Texcoords;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    Texcoords = aTexcoords;
    
    //基于世界空间的光照计算
    FragPos = vec3(model * vec4(aPos,1.0));
/*
    法线矩阵被定义为「模型矩阵左上角3x3部分的逆矩阵的转置矩阵」

    矩阵求逆是一项对于着色器开销很大的运算，因为它必须在场景中的每一个顶点上进行，所以应该尽可能地避免在着色器中进行求逆运算。以学习为目的的话这样做还好，
    但是对于一个高效的应用来说，你最好先在CPU上计算出法线矩阵，再通过uniform把它传递给着色器（就像模型矩阵一样）。
 */
    Normal = mat3(transpose(inverse(model))) * aNormal;
}
