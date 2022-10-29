#version 330 core


out vec4 FragColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D diffuse;
//反光度：一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小 影响镜面高光的散射/半径
uniform float shininess;


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
    
    //反射光
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * directionLight.specular;

    //结果
    return ambient + diffuse + specular;
    
}

void main()
{
    //基于世界空间的光照计算
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    //光照重新写哈
    
    FragColor = vec4(result,1);
}
