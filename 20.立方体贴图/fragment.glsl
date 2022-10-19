#version 330 core
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skyBox;

void main()
{
    //入射光，从view、指向观察点
    vec3 I = normalize(Position - cameraPos);
    
    //reflect要求第一个向量是从光源指向片段位置的向量
    //I - 2.0 * dot(N, I) * N
    //反射
    vec3 R1 = reflect(I,normalize(Normal));
    
    //折射率
    float ratio = 1.00 / 1.52;
    
    vec3 R2 = refract(I, normalize(Normal), ratio);

    //天空盒的顶点坐标都为单位向量，所以采样时只需单位向量，不需要关心大小
    vec4 col = texture(skyBox,R2);
    FragColor = vec4(col.rgb,1.0);

}
