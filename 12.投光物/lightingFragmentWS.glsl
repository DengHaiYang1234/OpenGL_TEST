#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    //反光度：一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小 影响镜面高光的散射/半径
    float shininess;
};

struct DirectionLight
{
    vec3 direction;//方向光方向
    vec3 ambient;//环境光强度
    vec3 diffuse;//满反射强度
    vec3 specular;//高光强度
};

struct PointLight
{
    vec3 position;//点光源位置
    
    vec3 ambient;//环境光强度
    vec3 diffuse;//满反射强度
    vec3 specular;//高光强度
    
    //点光源光照衰减
    float constant;//常数项
    float linear;//一次项
    float quadratic;//二次项
};

struct SpotLight
{
    vec3 position;  //聚光灯位置
    vec3 direction;//聚光灯方向
    float cutoff;//内切光角正弦值
    float outerCutOff;//外切光角正弦值
    
    vec3 ambient;//环境光强度
    vec3 diffuse;//满反射强度
    vec3 specular;//高光强度
};

out vec4 FragColor;

uniform Material material;
uniform DirectionLight directionLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
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
    vec3 lightDir;
    float attenuation = 1.0f;//光照衰减
    vec3 lightAmbient,lightDiffuse,lightSpecular;
    
    
//    //⭐️⭐️⭐️⭐️点光源
//    lightDir = normalize(pointLight.position - FragPos);
//    //光照衰减
//    float lightDistance = length(pointLight.position.xyz - FragPos);
//    attenuation = 1.0 / (pointLight.constant + pointLight.linear * lightDistance +
//                         pointLight.quadratic * (lightDistance * lightDistance));
//
//    lightAmbient = pointLight.ambient;
//    lightDiffuse = pointLight.diffuse;
//    lightSpecular = pointLight.specular;
    
//    //⭐️⭐️⭐️⭐️方向光
//    lightDir = normalize(-directionLight.direction);
//    lightAmbient = directionLight.ambient;
//    lightDiffuse = directionLight.diffuse;
//    lightSpecular = directionLight.specular;
    
    
    //⭐️⭐️⭐️⭐️聚光灯
    lightDir = normalize(pointLight.position - FragPos);
    float theta = dot(lightDir,normalize(-spotLight.direction));
    float epsilon   = spotLight.cutoff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    
    lightAmbient = spotLight.ambient * intensity;
    lightDiffuse = spotLight.diffuse * intensity;
    lightSpecular = spotLight.specular;
    
    vec3 viewDir = normalize(viewPos - FragPos);
    //reflect要求第一个向量是从光源指向片段位置的向量
    //等同于 I - 2.0 * dot(N, I) * N   I = -lightDir   N = norm
    vec3 reflectDir = reflect(-lightDir,norm);
    
    vec4 diffuseCol = texture(material.diffuse,TexCoords);
    
    //环境光
    vec3 ambient = diffuseCol.rgb * lightAmbient;
    
    //漫反射
    float diff = max(dot(norm,lightDir),0);
    vec3 diffuse = diffuseCol.rgb * diff * lightDiffuse;
    
    vec4 specularCol = texture(material.specular,TexCoords);
    //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularCol.rgb * spec * lightSpecular;

    //结果
    vec3 result = (ambient + diffuse + specular) * attenuation;
    
    
    
    FragColor = vec4(result,1);
}
