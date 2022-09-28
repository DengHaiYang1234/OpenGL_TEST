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

#define NR_POINT_LIGHTS 1

out vec4 FragColor;

uniform Material material;
uniform DirectionLight directionLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float matrixlight;
uniform float matrixmove;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

in vec3 FragPos;//顶点世界坐标
in vec3 Normal;
in vec2 TexCoords;

//计算方向光
vec3 CalDirectionLight(DirectionLight light,vec3 normal,vec3 viewDir)
{
    vec3 lightDir = normalize(-directionLight.direction);
    
    //reflect要求第一个向量是从光源指向片段位置的向量
    //等同于 I - 2.0 * dot(N, I) * N   I = -lightDir   N = norm
    vec3 reflectDir = reflect(-lightDir,normal);
    
    vec4 diffuseCol = texture(material.diffuse,TexCoords);
    
    //环境光
    vec3 ambient = diffuseCol.rgb * directionLight.ambient;
    
    //漫反射
    float diff = max(dot(normal,lightDir),0);
    vec3 diffuse = diffuseCol.rgb * diff * directionLight.diffuse;
    
    vec4 specularCol = texture(material.specular,TexCoords);
    //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularCol.rgb * spec * directionLight.specular;

    //结果
    return ambient + diffuse + specular;
    
}

vec3 CalPointLight(PointLight pointLight,vec3 normal,vec3 fragPos,vec3 viewDir)
{
    vec3 lightDir = normalize(pointLight.position - fragPos);
    //光照衰减
    float lightDistance = length(pointLight.position - fragPos);
    //光照衰减
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * lightDistance +
                             pointLight.quadratic * (lightDistance * lightDistance));
    
    //reflect要求第一个向量是从光源指向片段位置的向量
    //等同于 I - 2.0 * dot(N, I) * N   I = -lightDir   N = norm
    vec3 reflectDir = reflect(-lightDir,normal);
    
    vec4 diffuseCol = texture(material.diffuse,TexCoords);
    
    //环境光
    vec3 ambient = diffuseCol.rgb * pointLight.ambient;
    
    //漫反射
    float diff = max(dot(normal,lightDir),0);
    vec3 diffuse = diffuseCol.rgb * diff * pointLight.diffuse;
    
    vec4 specularCol = texture(material.specular,TexCoords);
    //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularCol.rgb * spec * pointLight.specular;

    //结果
    return (ambient + diffuse + specular) * attenuation;
}

vec3 CalSpotLight(SpotLight spotLight,vec3 normal,vec3 fragPos,vec3 viewDir)
{
    //⭐️⭐️⭐️⭐️聚光灯
    vec3 lightDir = normalize(spotLight.position - fragPos);
    float theta = dot(lightDir,normalize(-spotLight.direction));
    float epsilon   = spotLight.cutoff - spotLight.outerCutOff;
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    
    //reflect要求第一个向量是从光源指向片段位置的向量
    //等同于 I - 2.0 * dot(N, I) * N   I = -lightDir   N = norm
    vec3 reflectDir = reflect(-lightDir,normal);
    
    vec4 diffuseCol = texture(material.diffuse,TexCoords);
    
    //环境光
    vec3 ambient = diffuseCol.rgb * spotLight.ambient * intensity;
    
    //漫反射
    float diff = max(dot(normal,lightDir),0);
    vec3 diffuse = diffuseCol.rgb * diff * spotLight.diffuse * intensity;
    
    vec4 specularCol = texture(material.specular,TexCoords);
    //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = specularCol.rgb * spec * spotLight.specular;

    //结果
    return ambient + diffuse + specular;
}

void main()
{
    //基于世界空间的光照计算
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // 第一阶段：定向光照
    vec3 result = CalDirectionLight(directionLight,norm,viewDir);
    
    // 第二阶段：点光源
    for (int i = 0; i < NR_POINT_LIGHTS; i++) {
        result += CalPointLight(pointLights[i],norm,FragPos,viewDir);
    }
//
//    // 第三阶段：聚光
    result += CalSpotLight(spotLight,norm,FragPos,viewDir);
    
    FragColor = vec4(result,1);
}
