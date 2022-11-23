#version 330 core
in vec4 vertexColor;

out vec4 FragColor;

//uniform vec4 color1;

void main()
{
//    FragColor = vec4(1.0, 0.5, 0.2, 1.0);
    FragColor = vertexColor;
}
