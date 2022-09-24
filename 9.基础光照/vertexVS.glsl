#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aNormal; //法线变量的属性位置为 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;

out vec3 FragPos;//顶点世界坐标
out vec3 Normal;
out vec3 LightPos;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    //基于相机空间的光照计算
    FragPos = vec3(view * model * vec4(aPos,1.0));
    Normal = mat3(transpose(inverse(view * model))) * aNormal;
    //需要将世界空间的灯光坐标转为相机空间
    LightPos = vec3(view * vec4(lightPos,1.0));
}
