#version 330 core


out vec4 FragColor;


uniform vec3 lightColor;
//反光度：一个物体的反光度越高，反射光的能力越强，散射得越少，高光点就会越小 影响镜面高光的散射/半径越小，反之亦然
uniform float shininess;


uniform sampler2D brickwall;
uniform sampler2D normalMap;

uniform float far_plane;

in VS_OUT
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
}fs_in;


void main()
{
    vec3 lightPos = fs_in.TangentLightPos;
    vec3 viewPos = fs_in.TangentViewPos;
    vec3 fragPos = fs_in.TangentFragPos;
    
        
    vec3 objectColor = texture(brickwall,fs_in.TexCoords).rgb;
    
//    FragColor = vec4(objectColor,1);
    
    vec3 normal = texture(normalMap,fs_in.TexCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);
//    normal = normalize(fs_in.TBN * normal);
    
//    FragColor = vec4(normal,1);

    //基于世界空间的光照计算
    vec3 lightDir = normalize(lightPos - fragPos);

    //⭐️环境光
    float ambientStrength = 0.1;

    vec3 ambient = ambientStrength * lightColor;

    //⭐️漫反射
    float diff = max(dot(normal,lightDir),0);
    vec3 diffuse = lightColor.rgb * diff;

    //⭐️镜面反射  冯氏光照模型
    //缺点：当反射度为0.1时，会出现断层
    //当物体的反光度非常小时，它产生的镜面高光半径足以让这些相反方向的光线对亮度产生足够大的影响
    vec3 viewDir = normalize(viewPos - fragPos);
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

    vec3 result = (ambient + diffuse + specular) * objectColor;

    FragColor = vec4(result,1);
}
