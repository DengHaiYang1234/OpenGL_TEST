#version 330 core
out vec4 FragColor;

uniform sampler2D gPositionTex;
uniform sampler2D gNormalTex;
uniform sampler2D gColorSpecTex;

struct Light
{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;
in vec2 TexCoords;

void main()
{
    // 从G Buffer中获取数据
    vec3 FragPos = texture(gPositionTex,TexCoords).rgb;
    vec3 Normal = texture(gNormalTex,TexCoords).rgb;
    vec4 ColorSpec = texture(gColorSpecTex,TexCoords);
    vec3 diffuse = ColorSpec.rgb;
    float Specular = ColorSpec.a;
    
    // 计算光照
    vec3 lighting = diffuse * 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    for (int i = 0; i < NR_LIGHTS; i++) {
        vec3 lightDir = normalize(lights[i].Position - FragPos);
        // 漫反射
        vec3 diffuse = max(dot(Normal,lightDir),0.0) * diffuse * lights[i].Color;
        // 镜面反射
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal,halfwayDir),0.0),16.0);
        vec3 specularColor = lights[i].Color * spec * Specular;
        // 环境光
        float d = length(lights[i].Position - FragPos);
        float attenuation = 1.0 / (1.0 + lights[i].Linear * d  + lights[i].Quadratic * d * d);
        diffuse *= attenuation;
        specularColor *= attenuation;
        lighting += diffuse + specularColor;
    }

    FragColor = vec4(lighting,1);
}
