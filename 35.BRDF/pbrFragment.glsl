#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

struct Light
{
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

const int NR_LIGHTS = 4;
const float PI = 3.14159265359;
uniform Light lights[NR_LIGHTS];

uniform vec3 camPos;//相机位置
uniform vec3  albedo;// 反射率  模型本身的颜色
uniform float metallic; // 金属度
uniform float roughness;// 粗糙度
uniform float ao; // 环境光遮蔽
uniform vec3 lightPositions[4];//4个点光源

//菲涅尔方程描述的是在不同的表面角下表面所反射的光线所占的比率
vec3 fresnelSchlick(float cosTheta,vec3 F0)
{
    //cosTheta 表示视角与半角向量的夹角，=0表示垂直，菲涅尔反射最强
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta,5.0);
}

//法线分布函数  估算在受到表面粗糙度的影响下，取向方向与中间向量一致的微平面的数量
//粗糙度越高，高光越弱，范围越广  粗糙度越低，高光越强，范围越小
float DistributionGGX(vec3 N,vec3 H,float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float n_dot_h = max(dot(N,H),0.0);
    float n_dot_h2 = n_dot_h * n_dot_h;
    float nom = a2;
    float denom = (n_dot_h2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
    return nom / denom;
}

//几何函数从统计学上近似的求得了微平面间相互遮蔽的比率，这种相互遮蔽会损耗光线的能量。
//粗糙度较高的表面其微平面间相互遮蔽的概率就越高
float GeometrySchlickGGX(float NdotV,float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    float nom / denom;
}

float GeometrySmith(vec3 N,vec3 V,vec3 L,float roughness)
{
    float NdotV = max(dot(N,V),0.0);
    float NdotL = max(dot(N,L),0.0);
    float ggx2 = GeometrySchlickGGX(NdotV,roughness);
    float ggx1 = GeometrySchlickGGX(NdotL,roughness);
    
    return ggx1 * ggx2;
}

void main()
{
    //法线
    vec3 N = normalize(Normal);
    //视角方向
    vec3 V = normalize(camPos - WorldPos);
    
    vec3 Lo = vec3(0.0);
    
    for (int i = 0; i < NR_LIGHTS; i++) {
        //光源方向
        vec3 L = normalize(lights[i].Position - WorldPos);
        //半角向量
        vec3 H = normalize(L + V);
        //光照距离
        float dis = length(lights[i].Position - WorldPos);
        float attenuation = 1.0 / (dis * dis);
        vec3 radiance = lights[i].Color * attenuation;
        
        //非金属材质
        vec3 F0 = vec3(0.04);
        //插值获取金属或非金属
        F0 = mix(F0,albedo,metallic);
        //获取菲涅尔反射
        vec3 F = fresnelSchlick(max(dot(H,V),0.0),F0);
        //法线分布函数
        float NDF = GeometrySchlickGGX(N,H,roughness);
        //几何遮蔽函数
        float G = GeometrySmith(N,V,L,roughness);
        
        //BRDF
        vec3 nominator = NDF * G * F;
        float denominator = 4.0 * max(dot(N,V),0.0) * max(dot(N,L),0.0) + 0.001;
        vec3 specular = nominator / denominator;
        //ks
        vec3 KS = F;
        //kd
        vec3 KD = vec3(1.0) - KS;
        KD *= 1.0 - metallic;
        
        float NdotL = max(dot(N,L),0.0);
        Lo += (KD * albedo / PI + specular) * radiance * NdotL;
    }
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color   = ambient + Lo;
}
