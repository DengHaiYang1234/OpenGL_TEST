#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    //反光度：一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小 影响镜面高光的散射/半径
    float shininess;
};

struct Light
{
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

uniform Material material;
uniform Light light;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float matrixlight;
uniform float matrixmove;

in vec3 FragPos;//顶点世界坐标
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    //基于世界空间的光照计算
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    //reflect要求第一个向量是从光源指向片段位置的向量
    //等同于 I - 2.0 * dot(N, I) * N   I = -lightDir   N = norm
    vec3 reflectDir = reflect(-lightDir,norm);
    
    vec4 emissionCol = matrixlight * texture(material.emission,vec2(TexCoords.x,TexCoords.y+matrixmove));
    vec4 diffuseCol = texture(material.diffuse,TexCoords);
    diffuseCol.rgb = mix(diffuseCol.rgb,emissionCol.rgb,emissionCol.g);
    
    //环境光
    vec3 ambient = diffuseCol.rgb * light.ambient;
    
    //漫反射
    float diff = max(dot(norm,lightDir),0);
    vec3 diffuse = diffuseCol.rgb * diff * light.diffuse;
    
    vec4 specularCol = texture(material.specular,TexCoords);
    
    
    //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularCol.rgb * spec * light.specular;

    //结果
    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(result,1);
}
