#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture,TexCoords).rgb;
    FragColor = vec4(col,1);
//    FragColor = vec4(TexCoords.x,TexCoords.y,1,1);
}
