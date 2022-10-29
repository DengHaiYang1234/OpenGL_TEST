#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aNormal; //法线变量的属性位置为 1
layout (location = 2) in vec2 aTexCoords; //法线变量的属性位置为 1

uniform mat4 model;
uniform mat4 view;

out VS_OUT
{
    vec3 normal;
}vs_out;

void main()
{
    gl_Position = view * model * vec4(aPos, 1.0);
    
    //法向量变换矩阵（法线变换矩阵）是顶点变换矩阵的逆矩阵的转置矩阵
    mat3 normalMat = mat3(transpose(inverse(view * model)));
    
    vec3 viewSpaceNormal = normalMat * aNormal;
    
    vs_out.normal = normalize(viewSpaceNormal);
}
