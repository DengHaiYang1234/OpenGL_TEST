#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0
layout (location = 1) in vec3 aNormal; //位置变量的属性位置为 0
layout (location = 2) in vec2 aTexCoords; //位置变量的属性位置为 0

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

void main()
{
    WorldPos = vec3(model * vec4(aPos, 1.0));
    TexCoords = aTexCoords;
    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalize(normalMatrix * aNormal);
    gl_Position = projection * view * model *  vec4(aPos, 1.0);
}
