#version 330 core
out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 viewPos;

in vec3 FragPos;//顶点世界坐标
in vec3 Normal;
in vec3 LightPos;//需要将世界空间的灯光坐标转为相机空间

void main()
{
    //基于世界空间的光照计算
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    //在相机空间中，视角位置始终未原点，所以视角方向为(0,0,0) - FragPos
    vec3 viewDir = normalize(-FragPos);
    //reflect要求第一个向量是从光源指向片段位置的向量
    //I - 2.0 * dot(N, I) * N
    vec3 reflectDir = reflect(-lightDir,norm);
    vec3 I = -lightDir;
    vec3 N = norm;
    reflectDir = I - 2.0 * dot(N, I) * N;
    
    //漫反射
    float diff = max(dot(norm,lightDir),0);
    vec3 diffuse = diff * lightColor;
    
    //环境光
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    
    //反射光
    //若视角与光线的反射方向垂直则强度最大
    float specularStrength = 1;
    //反光度：一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小
    float shininess = 32;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    //结果
    vec3 result = (ambient + diffuse + specular) * objectColor;
    
    FragColor = vec4(result,1);
}
