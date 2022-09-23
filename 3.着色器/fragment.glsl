#version 330 core
out vec4 FragColor;
uniform vec3 ourColor;

in vec3 VertexPos;

void main()
{
    FragColor = vec4(VertexPos,1.0f);
}
