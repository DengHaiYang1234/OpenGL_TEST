#version 330 core
out vec4 FragColor;

uniform sampler2D hdrTex;
in vec2 TexCoords;
uniform float exposure;

void main()
{
    
    const float gamma = 2.2;
    vec3 hdrColor = texture(hdrTex,TexCoords).rgb;
    
    //Reinhard色调映射
//    vec3 mapped = hdrColor / (hdrColor + vec3(1.0));
    //曝光色调映射  晚上高曝光  白天地曝光
    vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    //gamma校正
    mapped = pow(mapped, vec3(1.0 / gamma));
    
    FragColor = vec4(mapped,1);
}
