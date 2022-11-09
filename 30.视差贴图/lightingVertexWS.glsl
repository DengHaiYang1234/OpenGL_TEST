#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aNormal; //法线变量的属性位置为 1
layout (location = 2) in vec2 aTexcoords; //法线变量的属性位置为 1
layout (location = 3) in vec3 aTangent; //法线变量的属性位置为 1

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;

out VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}vs_out;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    
    vs_out.TexCoords = aTexcoords;
    
    //基于世界空间的光照计算
    vs_out.FragPos = vec3(model * vec4(aPos,1.0));
    
    vec3 T = normalize(model * vec4(aPos,1.0)).xyz;
    vec3 N = normalize(model * vec4(aNormal,0.0)).xyz;
    vec3 B = cross(T,N);

    mat3 TBN = transpose(mat3(T, B, N));
    
    vs_out.TangentLightPos = TBN * lightPos;
    vs_out.TangentViewPos = TBN * viewPos;
    vs_out.TangentFragPos = TBN * vs_out.FragPos;
}
