#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aNormal; //法线变量的属性位置为 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 ResultColor;//顶点光照计算后的输出颜色

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vec3 FragPos;//顶点世界坐标
    vec3 Normal;
    
    //基于世界空间的光照计算
    
    FragPos = vec3(model * vec4(aPos,1.0));
/*
    法线矩阵被定义为「模型矩阵左上角3x3部分的逆矩阵的转置矩阵」

    矩阵求逆是一项对于着色器开销很大的运算，因为它必须在场景中的每一个顶点上进行，所以应该尽可能地避免在着色器中进行求逆运算。以学习为目的的话这样做还好，
    但是对于一个高效的应用来说，你最好先在CPU上计算出法线矩阵，再通过uniform把它传递给着色器（就像模型矩阵一样）。
 */
    Normal = mat3(transpose(inverse(model))) * aNormal;
    
    //基于世界空间的光照计算
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    //reflect要求第一个向量是从光源指向片段位置的向量
    //I - 2.0 * dot(N, I) * N
    vec3 reflectDir = reflect(-lightDir,norm);
    vec3 I = -lightDir;
    vec3 N = norm;
    reflectDir = I - 2.0 * dot(N, I) * N;
    
    //漫反射
    float diff = max(dot(norm,lightDir),0);
    vec3 diffuse = diff * lightColor;
    
    //环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    //反射光
    //若视角与光线的反射方向垂直则强度最大
    float specularStrength = 1;
    //反光度：一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float shininess = 32;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    //结果
    ResultColor = (ambient + diffuse + specular) * objectColor;
}
