#version 330 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform float near_plan; // 投影矩阵的近平面
uniform float far_plan; // 投影矩阵的远平面

float linearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plan * far_plan) / (far_plan + near_plan - z * (far_plan - near_plan));
}

void main()
{
    //存储的都是世界空间的信息
    
    // 存储片段位置向量
    gPositionDepth.xyz = FragPos;
    gPositionDepth.a = linearizeDepth(gl_FragCoord.z);
    // 存储法线向量
    gNormal = Normal;
    // 存储漫反射颜色
    gAlbedoSpec = vec3(0.95);
}
