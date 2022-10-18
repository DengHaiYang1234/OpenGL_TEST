#version 330 core
layout (location = 0) in vec3 aPos; //位置变量的属性位置为 0

uniform mat4 view;
uniform mat4 projection;

out vec3 TexCoords;

void main()
{
    TexCoords = aPos;
    
    vec4 prjoPos = projection * view *  vec4(aPos, 1.0);
    

    gl_Position = prjoPos.xyww;
    
}
