#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

in vec3 FragPos;//顶点世界坐标
in vec3 Normal;

void main()
{
    //基于世界空间的光照计算
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    //reflect要求第一个向量是从光源指向片段位置的向量
    //等同于 I - 2.0 * dot(N, I) * N   I = -lightDir   N = norm
    vec3 reflectDir = reflect(-lightDir,norm);
    
    //环境光
    vec3 ambient = material.ambient * light.ambient;
    
    //漫反射
    float diff = max(dot(norm,lightDir),0);
    vec3 diffuse = (material.diffuse * diff) * light.diffuse;
    
    //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (spec * material.specular) * light.specular;

    //结果
    vec3 result = ambient + diffuse + specular;
    
    FragColor = vec4(result,1);
}
