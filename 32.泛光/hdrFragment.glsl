#version 330 core
out vec4 FragColor;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
in vec2 TexCoords;
uniform float exposure;

void main()
{
    float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    hdrColor = hdrColor + bloomColor;
    
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result = pow(result, vec3(1.0 / gamma));
    
    FragColor = vec4(result, 1.0);
}
