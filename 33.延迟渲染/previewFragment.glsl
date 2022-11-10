#version 330 core
out vec4 FragColor;

uniform sampler2D image;
in vec2 TexCoords;

void main()
{
    const float gamma = 2.2;
    vec3 color = texture(image,TexCoords).rgb;
//    color = pow(color, vec3(1.0 / gamma));
    FragColor = vec4(color,1);
}
