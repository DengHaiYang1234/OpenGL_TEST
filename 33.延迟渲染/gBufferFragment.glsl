#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
    //存储的都是世界空间的信息
    
    // 存储片段位置向量
    gPosition = FragPos;
    // 存储法线向量
    gNormal = Normal;
    // 存储漫反射颜色
    gAlbedoSpec.rgb = texture(texture_diffuse1,TexCoords).rgb;
    // 存储高光
    gAlbedoSpec.a = texture(texture_specular1,TexCoords).r;
}
