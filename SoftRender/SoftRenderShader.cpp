//
//  SoftRenderShader.cpp
//  SoftRender
//
//  Created by 邓海洋 on 2022/11/11.
//

#ifndef SOFTRENDERSHADER_CPP
#define SOFTRENDERSHADER_CPP

#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include "hyMath.cpp"

using namespace std;

//定义顶点着色器和片段着色器的上下文，顶点着色器设置完数据后传递给片段着色器使用
struct ShaderContext
{
    //varying变量是vertex和fragment shader之间做数据传递用的
    //一般vertex shader修改varying变量的值，然后fragment shader使用该varying变量的值
    map<int, float> varying_float;//顶点索引+float数据
    map<int, Vec2f> varying_vec2f;//顶点索引+二维float数据
    map<int, Vec3f> varying_vec3f;//顶点索引+三维float数据
    map<int, Vec4f> varying_vec4f;//顶点索引+四维float数据
};


typedef function<Vec4f(int index, ShaderContext &output)> Hy_VertexShader;

typedef function<Vec4f(ShaderContext &input)> Hy_FragmentShader;

#endif
