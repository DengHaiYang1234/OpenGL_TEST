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
    //反射光
    vec3 R = reflect(I,normalize(Normal));

    vec4 col = texture(skyBox,R);
    FragColor = vec4(col.rgb,1.0);

}
