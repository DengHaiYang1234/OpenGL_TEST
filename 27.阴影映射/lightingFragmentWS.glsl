#version 330 core


out vec4 FragColor;


uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
//反光度：一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小 影响镜面高光的散射/半径越小，反之亦然
uniform float shininess;

uniform sampler2D wood;
uniform sampler2D shadowMap;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
}fs_in;

float ShadowCalculation(vec4 fragPosLightSpace,vec3 normal,vec3 lightDir)
{
    //进行齐次裁剪，将xyz归为[-1,1]区间
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    //投影值屏幕坐标
    projCoords = projCoords *  0.5 + 0.5;
    
    //获取当前片段在光照空间下最近的深度值
    float closestDepth = texture(shadowMap,projCoords.xy).r;
        
    //当前片段的在光照空间的实际深度值
    float currentDepth = projCoords.z;
    
    //阴影偏移，用于解决阴影失真的问题
    //表面法线与光照方向的点积决定偏移程度，若光照与法线相互垂直，则说明不太需要偏移，反之亦然
    float bias = max(0.05 * (1.0 - dot(normal,lightDir)),0.005);
    
    //若当前深度值大于最近的深度值，说明在阴影里，反之亦然
    float shadow = 0;
    
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; y++) {
            float curClosestDepth = texture(shadowMap,projCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth - bias > curClosestDepth  ? 1.0 : 0.0;
        }
    }
    
    shadow /= 9;
    
    //解决超出视锥体范围的区域被判定为阴影的情况，这里要么增加视锥体的远平面，要么判定深度>1.
    if(projCoords.z > 1.0)
        shadow = 0.0;
    
    return shadow;
}

void main()
{
    //基于世界空间的光照计算
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    //⭐️环境光
    float ambientStrength = 0.1;

    vec3 ambient = ambientStrength * lightColor;

    //⭐️漫反射
    float diff = max(dot(normal,lightDir),0);
    vec3 diffuse = lightColor.rgb * diff;

    //⭐️镜面反射  冯氏光照模型
    //缺点：当反射度为0.1时，会出现断层
    //当物体的反光度非常小时，它产生的镜面高光半径足以让这些相反方向的光线对亮度产生足够大的影响
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
//    vec3 reflectDir = reflect(-lightDir,normal);
//    float spec = pow(max (dot(viewDir, reflectDir), 0.0), shininess);

    //Blinn-Phong光照模型
    //Blinn-Phong模型不再依赖于反射向量，而是采用了所谓的半程向量(Halfway Vector)，即光线与视线夹角一半方向上的一个单位向量。当半程向量与法线向量越接近时，镜面光分量就越大。
    //当视线正好与（现在不需要的）反射向量对齐时，半程向量就会与法线完美契合。所以当观察者视线越接近于原本反射光线的方向时，镜面高光就会越强。

    //⚠️：若光源在表面以下，那么半程向量与表面法线之间的夹角还是会超过90度。
    vec3 halfwayDir = normalize(lightDir+viewDir);
    
    //⭐️：如果场景特别亮，物体四周后方阴影特别明显的假，改一下反射度试试
    float spec = pow(max (dot(viewDir, halfwayDir), 0.0), 64);

    float specularStrength = 0.5;
    vec3 specular = specularStrength * spec * lightColor.rgb;

    vec3 objectColor = texture(wood,fs_in.TexCoords).rgb;
    
    //阴影计算
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace,normal,lightDir);
    
    shadow = min(shadow, 0.75);
        
    vec3 result = (ambient + (1.0 - shadow) * ( diffuse + specular)) * objectColor;

    FragColor = vec4(result,1);
}
