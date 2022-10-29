#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;

in vec2 TexCoords;

void main()
{
    vec4 col = texture(texture_diffuse1,TexCoords);
    
    FragColor = vec4(col.rgb,1);
}
