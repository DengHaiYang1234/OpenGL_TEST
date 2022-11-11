//
//  Render.cpp
//  SoftRender
//
//  Created by 邓海洋 on 2022/11/11.
//

#ifndef RENDER_CPP
#define RENDER_CPP

#include "Bitmap.cpp"
#include "hyMath.cpp"
#include "SoftRenderShader.cpp"

class Render
{
private:
    Hy_VertexShader _vertex_shader;     //顶点着色器
    Hy_FragmentShader _fragment_shader; //片段着色器
    Bitmap *_frame_buffer;           //像素缓存
    float **_depth_buffer;           //深度缓存  指针的指针实质就是一个二维数组
    bool _render_frame;              // 是否绘制线框
    bool _render_pixel;              // 是否填充像素
    bool _msaa;                      //开启反走样
    int _fb_width;                   // frame buffer 宽度
    int _fb_height;                  // frame buffer 高度
    uint32_t _color_fg;              // 前景色：画线时候用
    uint32_t _color_bg;              // 背景色：Clear 时候用
    // 三角形外接矩形范围
    int _min_x;
    int _min_y;
    int _max_x;
    int _max_y;

private:
    struct Vertex
    {
        ShaderContext context; //上下文
        float rhw;             //w的倒数
        Vec4f pos;             //坐标
        Vec2f spf;             //浮点数的屏幕坐标
        Vec2i spi;             //整数的屏幕坐标
    };

    Vertex _vertex[3]; // 三角形的三个顶点

public:
    Render(int width, int height)
    {
        _frame_buffer = NULL;
        _depth_buffer = NULL;
        _render_frame = false;
        _render_pixel = true;
        Init(width, height);
    }

    //初始化 帧缓存 深度缓存
    void Init(int width, int height)
    {
        Reset();
        _frame_buffer = new Bitmap(width, height);
        _fb_width = width;
        _fb_height = height;
        _depth_buffer = new float *[height]; //指针数组 存储的是每行一维数组的首地址
        for (int j = 0; j < height; j++)
        {
            //_depth_buffer[j]也是指针 指向每行一维数组的首地址
            _depth_buffer[j] = new float[width];
        }
        Clear();
    }

    //设置渲染状态 renderFrame:渲染线框  renderPixel:渲染像素  MSAA:是否开启反走样
    void SetRenderState(bool renderFrame, bool renderPixel, bool MSAA)
    {
        _render_frame = renderFrame;
        _render_pixel = renderPixel;
        _msaa = MSAA;
    }

    void DrawPrimitiveOpengl()
    {
        if (_frame_buffer == NULL || _vertex_shader == NULL)
            return;

        float f1 = (50 - 0.1) / 2.0;
        float f2 = (50 + 0.1) / 2.0;

        for (int k = 0; k < 3; k++)
        {
            //顶点信息
            Vertex &vertex = _vertex[k];

            //清空当前顶点中的所有数据
            vertex.context.varying_float.clear();
            vertex.context.varying_vec2f.clear();
            vertex.context.varying_vec3f.clear();
            vertex.context.varying_vec4f.clear();

            //填充vertex.context数据
            vertex.pos = _vertex_shader(k, vertex.context);
            cout << "w:" << vertex.pos.w << endl;
            //齐次坐标裁剪
            vertex.pos.x /= vertex.pos.w;
            vertex.pos.y /= vertex.pos.w;
            vertex.pos.z /= vertex.pos.w;
            cout << "齐次坐标裁剪:" << vertex.pos.x << "," << vertex.pos.y << endl;
            //屏幕坐标
            vertex.spf.x = vertex.pos.x * 0.5f + 0.5f;
            vertex.spf.x *= _fb_width;
            vertex.spf.y = 1 - (vertex.pos.y * 0.5f + 0.5f);
            vertex.spf.y *= _fb_height;

            cout << "屏幕坐标:" << vertex.spf.x << "," << vertex.spf.y << endl;

            vertex.spi.x = (int)(vertex.spf.x + 0.5f);
            vertex.spi.y = (int)(vertex.spf.y + 0.5f);
        }
        if (_render_frame)
        {
            // //由上往左下
            DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
            //由上往右下
            DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[2].spi.x, _vertex[2].spi.y);
            // // //由右往左
            DrawLine(_vertex[2].spi.x, _vertex[2].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
        }
        else
        {
            //三角形的外接矩形
            _min_x = Min(_vertex[0].spi.x, Min(_vertex[1].spi.x, _vertex[2].spi.x));
            _max_x = Max(_vertex[0].spi.x, Max(_vertex[1].spi.x, _vertex[2].spi.x));
            _min_y = Min(_vertex[0].spi.y, Min(_vertex[1].spi.y, _vertex[2].spi.y));
            _max_y = Max(_vertex[0].spi.y, Max(_vertex[1].spi.y, _vertex[2].spi.y));
            _min_x = Clamp(0, _fb_width, _min_x);
            _max_x = Clamp(0, _fb_width, _max_x);
            _min_y = Clamp(0, _fb_height, _min_y);
            _max_y = Clamp(0, _fb_height, _max_y);

            if (_msaa)
            {
                cout << "MASS" << endl;
                Vec2f pixel_grid_4x[] =
                    {
                        {0.25f, 0.25f}, //左下
                        {0.75f, 0.25f}, //右下
                        {0.25f, 0.75f}, //左上
                        {0.75f, 0.75f}, //右上
                    };

                for (int cy = _min_y; cy < _max_y; cy++)
                {
                    for (int cx = _min_x; cx < _max_x; cx++)
                    {
                        float minDepth = 1000000.0f;

                        int validPixelGridCount = 0;

                        for (int i = 0; i < 4; i++)
                        {
                            Vec2f px = {(float)cx + pixel_grid_4x[i].x, (float)cy + pixel_grid_4x[i].y};
                            bool isInside = insideTriangle(px, _vertex[0].spf, _vertex[1].spf, _vertex[2].spf);

                            if (!isInside)
                            {
                                continue;
                            }

                            float alpha = 0;
                            float beta = 0;
                            float gamma = 0;

                            //https://zhuanlan.zhihu.com/p/448575965  重心插值以及投影校正插值
                            computeBarycentric2D(px, _vertex[0].spf, _vertex[1].spf, _vertex[2].spf, alpha, beta, gamma);
                            //投影变换前的深度值z   因为投影变换后的z与投影变换前的z值完全不一样了，所以需要重新计算
                            float w_reciprocal = alpha / _vertex[0].pos.w + beta / _vertex[1].pos.w + gamma / _vertex[2].pos.w;
                            minDepth = Min(minDepth, w_reciprocal);
                            validPixelGridCount++;
                        }
                        
                        //有效像素
                        if (validPixelGridCount != 0)
                        {
                            if (minDepth < _depth_buffer[cy][cx])
                            {
                                Vec2f px = {(float)cx + 0.5f, (float)cy + 0.5f};
                                float alpha = 0;
                                float beta = 0;
                                float gamma = 0;

                                //https://zhuanlan.zhihu.com/p/448575965  重心插值以及投影校正插值
                                computeBarycentric2D(px, _vertex[0].spf, _vertex[1].spf, _vertex[2].spf, alpha, beta, gamma);
                                //投影变换前的深度值z   因为投影变换后的z与投影变换前的z值完全不一样了，所以需要重新计算
                                float w_reciprocal = alpha / _vertex[0].pos.w + beta / _vertex[1].pos.w + gamma / _vertex[2].pos.w;

                                //还原当前像素重心插值后的w
                                float w = 1.0f / ((w_reciprocal != 0.0f) ? w_reciprocal : 1.0f);

                                //插值属性值
                                float c0 = (alpha * w) / _vertex[0].pos.w;
                                float c1 = (beta * w) / _vertex[1].pos.w;
                                float c2 = (gamma * w) / _vertex[2].pos.w;
                                // cout << "z_interpolated:" << z_interpolated << endl;
                                _depth_buffer[cy][cx] = w_reciprocal;
                                //开始对顶点数据进行插值运算
                                ShaderContext input;

                                ShaderContext &i0 = _vertex[0].context;
                                ShaderContext &i1 = _vertex[1].context;
                                ShaderContext &i2 = _vertex[2].context;

                                for (auto const &it : i0.varying_vec4f)
                                {
                                    int key = it.first;
                                    const Vec4f &f0 = i0.varying_vec4f[key];
                                    const Vec4f &f1 = i1.varying_vec4f[key];
                                    const Vec4f &f2 = i2.varying_vec4f[key];

                                    // Vec4f f_interpolated = alpha * f0 / _vertex[0].pos.w + beta * f1 / _vertex[1].pos.w + gamma * f2 / _vertex[2].pos.w;
                                    // f_interpolated *= w_reciprocal;
                                    // input.varying_vec4f[key] = f_interpolated;
                                    input.varying_vec4f[key] = (f0 * c0 + f1 * c1 + f2 * c2) * (validPixelGridCount / 4.0f);
                                }

                                Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};

                                //将顶点的varying传递给片段着色器
                                if (_fragment_shader != NULL)
                                {
                                    color = _fragment_shader(input);
                                }
                                //片段着色器填充像素
                                //其实这里应该还有一个透明度测试是与深度缓存挂钩  这样透明度混合才是正确的
                                _frame_buffer->SetPixel(cx, cy, color);

                                _depth_buffer[cy][cx] = minDepth;
                            }
                        }
                    }
                }
            }
            else
            {
                cout << "NORMAL" << endl;
                for (int cy = _min_y; cy < _max_y; cy++)
                {
                    for (int cx = _min_x; cx < _max_x; cx++)
                    {
                        Vec2f px = {(float)cx + 0.5f, (float)cy + 0.5f};

                        bool isInside = insideTriangle(px, _vertex[0].spf, _vertex[1].spf, _vertex[2].spf);

                        if (!isInside)
                        {
                            continue;
                        }

                        float alpha = 0;
                        float beta = 0;
                        float gamma = 0;

                        //https://zhuanlan.zhihu.com/p/448575965  重心插值以及投影校正插值
                        computeBarycentric2D(px, _vertex[0].spf, _vertex[1].spf, _vertex[2].spf, alpha, beta, gamma);

                        //投影变换前的深度值z   因为投影变换后的z与投影变换前的z值完全不一样了，所以需要重新计算
                        float w_reciprocal = alpha / _vertex[0].pos.w + beta / _vertex[1].pos.w + gamma / _vertex[2].pos.w;

                        if (w_reciprocal < _depth_buffer[cy][cx])
                        {
                            //还原当前像素重心插值后的w
                            float w = 1.0f / ((w_reciprocal != 0.0f) ? w_reciprocal : 1.0f);

                            //插值属性值
                            float c0 = (alpha * w) / _vertex[0].pos.w;
                            float c1 = (beta * w) / _vertex[1].pos.w;
                            float c2 = (gamma * w) / _vertex[2].pos.w;
                            // cout << "z_interpolated:" << z_interpolated << endl;
                            _depth_buffer[cy][cx] = w_reciprocal;
                            //开始对顶点数据进行插值运算
                            ShaderContext input;

                            ShaderContext &i0 = _vertex[0].context;
                            ShaderContext &i1 = _vertex[1].context;
                            ShaderContext &i2 = _vertex[2].context;

                            for (auto const &it : i0.varying_vec4f)
                            {
                                int key = it.first;
                                const Vec4f &f0 = i0.varying_vec4f[key];
                                const Vec4f &f1 = i1.varying_vec4f[key];
                                const Vec4f &f2 = i2.varying_vec4f[key];

                                // Vec4f f_interpolated = alpha * f0 / _vertex[0].pos.w + beta * f1 / _vertex[1].pos.w + gamma * f2 / _vertex[2].pos.w;
                                // f_interpolated *= w_reciprocal;
                                // input.varying_vec4f[key] = f_interpolated;
                                input.varying_vec4f[key] = f0 * c0 + f1 * c1 + f2 * c2;
                            }

                            Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};

                            //将顶点的varying传递给片段着色器
                            if (_fragment_shader != NULL)
                            {
                                color = _fragment_shader(input);
                            }
                            //片段着色器填充像素
                            //其实这里应该还有一个透明度测试是与深度缓存挂钩  这样透明度混合才是正确的
                            _frame_buffer->SetPixel(cx, cy, color);
                        }
                    }
                }
            }
        }
    }

    //根据顶点和片段着色器渲染
    bool DrawPrimitiveDX()
    {
        if (_frame_buffer == NULL || _vertex_shader == NULL)
            return false;

        for (int k = 0; k < 3; k++)
        {
            //顶点信息
            Vertex &vertex = _vertex[k];

            //清空当前顶点中的所有数据
            vertex.context.varying_float.clear();
            vertex.context.varying_vec2f.clear();
            vertex.context.varying_vec3f.clear();
            vertex.context.varying_vec4f.clear();

            //填充vertex.context数据
            vertex.pos = _vertex_shader(k, vertex.context);
            cout << "变换后的顶点:" << vertex.pos.x << "," << vertex.pos.y << "," << vertex.pos.z << "," << vertex.pos.w << endl;
            //https://zhuanlan.zhihu.com/p/70470309
            //CVV:渲染系统完成projection(投影)和随后的clipping(裁剪)，
            //这会把视景体(view volume)转换成一个单位立方体(unit cube)，
            //它的顶点坐标范围为(-1, -1, -1)和(1, 1, 1)。
            //这个单位立方体被称为标准视景体(canonical view volume)

            // 裁剪空间后的w，表示到屏幕的距离
            float w = vertex.pos.w;
            // cout << " 齐次坐标 w:" << w  << endl;
            //将顶点坐标限制在标准CVV空间中，超出的部分裁剪
            if (w == 0.0f)
                return false;
            //在dx中z的取值范围为(0,w)  opengl中z的取值范围为(-1,1)
            if (vertex.pos.z < 0 || vertex.pos.z > w)
            {
                cout << " z:" << vertex.pos.z << endl;
                return false;
            }
            if (vertex.pos.x < -w || vertex.pos.x > w)
            {
                cout << " x:" << vertex.pos.x << endl;
                return false;
            }
            if (vertex.pos.y < -w || vertex.pos.y > w)
            {
                cout << " y:" << vertex.pos.y << endl;
                return false;
            }

            //MVP之后的齐次坐标的w值的倒数，用于计算归一化的CVV
            vertex.rhw = 1.0f / w;
            // cout << " 齐次坐标 1/w:" << vertex.rhw  << endl;
            //获取归一化的cvv -1<x<1 -1<y<1 0<z<1
            vertex.pos *= vertex.rhw;

            // cout << " 齐次裁剪 pos:" << vertex.pos.x << "," << vertex.pos.y << "," << vertex.pos.z << "," << endl;

            //将CVV坐标变换至屏幕坐标 [-1,1] -> [0,1]
            vertex.spf.x = (vertex.pos.x + 1.0f) * _fb_width * 0.5f;
            vertex.spf.y = (1.0f - vertex.pos.y) * _fb_height * 0.5f;

            //获取整数屏幕坐标
            vertex.spi.x = (int)(vertex.spf.x + 0.5f);
            vertex.spi.y = (int)(vertex.spf.y + 0.5f);

            //更新外接矩形的范围
            if (k == 0)
            {
                _min_x = _max_x = Clamp(0, _fb_width - 1, vertex.spi.x);
                _min_y = _max_y = Clamp(0, _fb_height - 1, vertex.spi.y);
            }
            else
            {
                _min_x = Clamp(0, _fb_width - 1, Min(_min_x, vertex.spi.x));
                _max_x = Clamp(0, _fb_width - 1, Max(_max_x, vertex.spi.x));
                _min_y = Clamp(0, _fb_height - 1, Min(_min_y, vertex.spi.y));
                _max_y = Clamp(0, _fb_height - 1, Max(_max_y, vertex.spi.y));
            }
        }

        if (_render_frame) //画线框
        {
            // //由上往左下
            DrawLine(_vertex[0].spi.x, _vertex[0].spf.y, _vertex[1].spi.x, _vertex[1].spi.y);
            //由上往右下
            DrawLine(_vertex[0].spi.x, _vertex[0].spf.y, _vertex[2].spi.x, _vertex[2].spi.y);
            // // //由右往左
            DrawLine(_vertex[2].spi.x, _vertex[2].spf.y, _vertex[1].spi.x, _vertex[1].spi.y);
        }

        //如果不填充像素就退出
        if (_render_pixel == false)
            return false;
        //指向顶点的左上向量
        Vec4f v01 = _vertex[1].pos - _vertex[0].pos;
        //指向顶点的右上向量
        Vec4f v02 = _vertex[2].pos - _vertex[0].pos;
        //叉乘
        Vec4f normal = vector_cross(v01, v02);
        cout << "z:" << normal.z << endl;
        //使用指针数组
        Vertex *vtx[3] = {&_vertex[0], &_vertex[1], &_vertex[2]};

        //叉乘 行列式 https://www.matongxue.com/lessons/559/parts/775 中的行列式及叉乘相关
        //保证三角形形成的面是朝向屏幕的 如果反向，那么就交换顶点  保证 edge equation 判断的符号为正
        if (normal.z > 0.0f)
        {
            vtx[1] = &_vertex[2];
            vtx[2] = &_vertex[1];
        }
        else if (normal.z == 0.0f) //叉乘结果为零 表示两个向量是平行的，叉乘没有意义
        {
            return false;
        }

        //获取三个顶点的屏幕坐标
        Vec2i p0 = vtx[0]->spi;
        Vec2i p1 = vtx[1]->spi;
        Vec2i p2 = vtx[2]->spi;

        //p0指向p1的向量 * p0指向p2的向量  得到面积
        float s = Abs(vector_cross(p1 - p0, p2 - p0));
        if (s <= 0)
        {
            return false;
        }

        // EdgeEquation(vtx);

        EdgeWalking(_vertex);

        return true;
    }

    void DrawLine(int x1, int y1, int x2, int y2)
    {
        if (_frame_buffer)
            _frame_buffer->DrawLine(x1, y1, x2, y2, _color_fg);
    }

    void EdgeEquation(Vertex *vtx[3])
    {
        Vec2i p0 = vtx[0]->spi;
        Vec2i p1 = vtx[1]->spi;
        Vec2i p2 = vtx[2]->spi;
        //OpenGL 和 DirectX 是如何在只知道顶点的情况下得出像素位置的？ - 韦易笑的回答 - 知乎
        //https://www.zhihu.com/question/48299522/answer/799333394
        // 三角形填充时，左面和上面的边上的点需要包括，右方和下方边上的点不包括
        // 先判断是否是 TopLeft，判断出来后会和下方 Edge Equation 一起决策
        //关于 Edge Equation https://www.zhihu.com/question/34100482/answer/58424576
        //简单来说为了避免同一个像素被相邻的三角形画两次，使用Top-Left Fill Rule，当一个像素刚好压在三角形边上的时候，只有这条边在三角形的左边，或者上面的时候，才判定这个像素被三角形覆盖
        //还有就是检测是否为左上边时，采用顺时针方式，避免检测出现错误 p0 -> p1 -> p2 -> p0
        bool TopLeft01 = IsTopLeft(p0, p1);
        bool TopLeft12 = IsTopLeft(p1, p2);
        bool TopLeft20 = IsTopLeft(p2, p0);

        //遍历三角形的外接矩形所有点
        for (int cy = _min_y; cy < _max_y; cy++)
        {
            for (int cx = _min_x; cx < _max_x; cx++)
            {
                //根据两点式算出直线一般方程，=0表示在直线上，>0表示在直线右侧  <0表示在直线左侧
                //使用整数避免浮点误差
                int E01 = -(cx - p0.x) * (p1.y - p0.y) + (cy - p0.y) * (p1.x - p0.x);
                int E12 = -(cx - p1.x) * (p2.y - p1.y) + (cy - p1.y) * (p2.x - p1.x);
                int E20 = -(cx - p2.x) * (p0.y - p2.y) + (cy - p2.y) * (p0.x - p2.x);

                // 如果是左上边，用 E >= 0 判断合法，如果右下边就用 E > 0 判断合法
                // 这里通过引入一个误差 1 ，来将 < 0 和 <= 0 用一个式子表达
                //这一步是用来计算像素点点是否在三角形内部的同时，并且还要满足Top-Left Fill Rule
                if (E01 < (TopLeft01 ? 0 : 1))
                    continue; //在01边的外侧
                if (E12 < (TopLeft12 ? 0 : 1))
                    continue; //在12边的左侧
                if (E20 < (TopLeft20 ? 0 : 1))
                    continue; //在20边的左侧

                Vec2f px = {(float)cx + 0.5f, (float)cy + 0.5f};

                //当前点到3个顶点的矢量
                Vec2f s0 = vtx[0]->spf - px;
                Vec2f s1 = vtx[1]->spf - px;
                Vec2f s2 = vtx[2]->spf - px;

                //重心坐标系：计算内部子三角形面积 a / b / c
                //注意 此时的面积其实是两个点围城的平行四边形的面积
                float a = Abs(vector_cross(s1, s2)); // 子三角形 Px-P1-P2 面积
                float b = Abs(vector_cross(s2, s0)); // 子三角形 Px-P2-P0 面积
                float c = Abs(vector_cross(s0, s1)); // 子三角形 Px-P0-P1 面积
                float s = a + b + c;                 // 大三角形 P0-P1-P2 面积

                if (s == 0.0f)
                    continue;

                //归一化  除以总面积，以保证：a + b + c = 1，方便用作插值系数
                a = a * (1.0f / s);
                b = b * (1.0f / s);
                c = c * (1.0f / s);

                //根据重心坐标插值计算当前像素的深度
                // 计算当前点的 1/w，因 1/w 和屏幕空间呈线性关系，故直接重心插值
                //这里对用w来表示深度缓存是因为 w的值即为变换前视图空间下z的值

                //OpenGL 和 DirectX 是如何在只知道顶点的情况下得出像素位置的
                //https://www.zhihu.com/question/48299522/answer/799333394
                //变量插值的时候需要进行透视矫正，根据平面方程和透视投影公式，
                //可以证明屏幕空间内的像素和 1/w 是线性相关的，而三维空间的 x / w, y / w, z / w 和屏幕空间也是线性相关的，
                //也即各个 varying 变量按屏幕空间插值时需要先 / w，
                //然后按照屏幕空间每迭代一个点时再除以最新的（1/w）就可以还原改点的真实 varying 变量值
                //近大远小
                float rhw = vtx[0]->rhw * a + vtx[1]->rhw * b + vtx[2]->rhw * c;

                //深度测试
                if (rhw < _depth_buffer[cy][cx])
                    continue;

                _depth_buffer[cy][cx] = rhw; //更新深度缓存

                //还原当前像素重心插值后的w
                float w = 1.0f / ((rhw != 0.0f) ? rhw : 1.0f);

                // 计算三个顶点插值 varying 的系数
                // 先除以各自顶点的 w 然后进行屏幕空间插值然后再乘以当前 w
                // 这一步是获取该点在不同顶点出的深度插值后再做透视处理，得到最终的插值
                float c0 = vtx[0]->rhw * a * w;
                float c1 = vtx[1]->rhw * b * w;
                float c2 = vtx[2]->rhw * c * w;

                //开始对顶点数据进行插值运算
                ShaderContext input;

                ShaderContext &i0 = vtx[0]->context;
                ShaderContext &i1 = vtx[1]->context;
                ShaderContext &i2 = vtx[2]->context;

                for (auto const &it : i0.varying_float)
                {
                    int key = it.first;
                    float f0 = i0.varying_float[key];
                    float f1 = i1.varying_float[key];
                    float f2 = i2.varying_float[key];

                    input.varying_float[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                for (auto const &it : i0.varying_vec2f)
                {
                    int key = it.first;
                    const Vec2f &f0 = i0.varying_vec2f[key];
                    const Vec2f &f1 = i1.varying_vec2f[key];
                    const Vec2f &f2 = i2.varying_vec2f[key];

                    input.varying_vec2f[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                for (auto const &it : i0.varying_vec3f)
                {
                    int key = it.first;
                    const Vec3f &f0 = i0.varying_vec3f[key];
                    const Vec3f &f1 = i1.varying_vec3f[key];
                    const Vec3f &f2 = i2.varying_vec3f[key];

                    input.varying_vec3f[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                for (auto const &it : i0.varying_vec4f)
                {
                    int key = it.first;
                    const Vec4f &f0 = i0.varying_vec4f[key];
                    const Vec4f &f1 = i1.varying_vec4f[key];
                    const Vec4f &f2 = i2.varying_vec4f[key];

                    input.varying_vec4f[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};

                //将顶点的varying传递给片段着色器
                if (_fragment_shader != NULL)
                {
                    color = _fragment_shader(input);
                }

                //片段着色器填充像素
                //其实这里应该还有一个透明度测试是与深度缓存挂钩  这样透明度混合才是正确的
                _frame_buffer->SetPixel(cx, cy, color);
            }
        }
    }

    void EdgeWalking(Vertex _vertex[3])
    {
        Vertex p0 = _vertex[0];
        Vertex p1 = _vertex[1];
        Vertex p2 = _vertex[2];

        //将坐标转为左下为(0,0)
        // p0.spf.y = _fb_height * 1.0 - p0.spf.y;
        // p1.spf.y = _fb_height * 1.0 - p1.spf.y;
        // p2.spf.y = _fb_height * 1.0 - p2.spf.y;

        if (p0.spf.y > p1.spf.y)
        {
            cout << "1" << endl;
            Vertex tmp = p0;
            p0 = p1;
            p1 = tmp;
        }

        if (p1.spf.y > p2.spf.y)
        {
            cout << "2" << endl;
            Vertex tmp = p1;
            p1 = p2;
            p2 = tmp;
        }

        if (p0.spf.y > p1.spf.y)
        {
            cout << "3" << endl;
            Vertex tmp = p0;
            p0 = p1;
            p1 = tmp;
        }

        cout << "p0:" << p0.spf.x << "," << p0.spf.y << endl;
        cout << "p1:" << p1.spf.x << "," << p1.spf.y << endl;
        cout << "p2:" << p2.spf.x << "," << p2.spf.y << endl;

        Vertex *vtx[3] = {&p0, &p1, &p2};

        //p0 在最下面  p2在最上面
        //中间跟上面的相等，是底三角形
        if (p1.spf.y == p2.spf.y)
        {
            DownTriangle(p1, p2, p0);
        } //顶三角形
        else if (p1.spf.y == p0.spf.y)
        {
            UpTriangle(p1, p0, p2);
        }
        else
        {
            //插值求出中间点对面的那个点，划分为两个新的三角形
            float weight = (p2.spf.y - p1.spf.y) / (p2.spf.y - p0.spf.y);
            Vertex newEdge;
            newEdge.spf = Lerp(p2.spf, p0.spf, weight);
            cout << "newEdge  spf:" << newEdge.spf.x << "," << newEdge.spf.y << endl;
            newEdge.context.varying_vec4f[0] = Lerp(p2.context.varying_vec4f[0], p0.context.varying_vec4f[0], weight);
            cout << "newEdge  color:" << newEdge.spf.x << "," << newEdge.spf.y << endl;
            UpTriangle(p1, newEdge, p2);
            DownTriangle(p1, newEdge, p0);
        }
    }

    void UpTriangle(const Vertex &v1, const Vertex &v2, const Vertex &v3)
    {
        Vertex left, right, top, newLeft, newRight;
        newLeft = left = v1.spf.x > v2.spf.x ? v2 : v1;
        newRight = right = v1.spf.x > v2.spf.x ? v1 : v2;
        top = v3;
        left.spf.x = int(left.spf.x);
        int dy = top.spf.y - left.spf.y;
        int nowY = top.spf.y;
        //从上往下插值
        for (int i = dy; i >= 0; i--)
        {
            float weight = 0;
            if (dy != 0)
            {
                weight = (float)i / dy;
            }
            newLeft.spf = Lerp(left.spf, top.spf, weight);
            newLeft.context.varying_vec4f[0] = Lerp(left.context.varying_vec4f[0], top.context.varying_vec4f[0], weight);
            newRight.spf = Lerp(right.spf, top.spf, weight);
            newRight.context.varying_vec4f[0] = Lerp(right.context.varying_vec4f[0], top.context.varying_vec4f[0], weight);
            newLeft.spf.x = int(newLeft.spf.x);
            newRight.spf.x = int(newRight.spf.x + 0.5);
            newLeft.spf.y = newRight.spf.y = nowY;
            ScanLine(newLeft, newRight);
            nowY--;
        }
    }

    void DownTriangle(Vertex &v1, Vertex &v2, Vertex &v3)
    {
        Vertex left, right, bottom, newLeft, newRight;
        newLeft = left = v1.spf.x > v2.spf.x ? v2 : v1;
        newRight = right = v1.spf.x > v2.spf.x ? v1 : v2;

        bottom = v3;
        int dy = left.spf.y - bottom.spf.y;
        int nowY = left.spf.y;
        //从上往下插值
        for (int i = 0; i < dy; i++)
        {
            float weight = 0;
            if (dy != 0)
            {
                weight = (float)i / dy;
            }
            newLeft.spf = Lerp(left.spf, bottom.spf, weight);
            newLeft.context.varying_vec4f[0] = Lerp(left.context.varying_vec4f[0], bottom.context.varying_vec4f[0], weight);
            newRight.spf = Lerp(right.spf, bottom.spf, weight);
            newRight.context.varying_vec4f[0] = Lerp(right.context.varying_vec4f[0], bottom.context.varying_vec4f[0], weight);
            newLeft.spf.x = int(newLeft.spf.x);
            newRight.spf.x = int(newRight.spf.x + 0.5);
            newLeft.spf.y = newRight.spf.y = nowY;
            ScanLine(newLeft, newRight);
            nowY--;
        }
    }
    void ScanLine(Vertex &left, Vertex &right)
    {
        int length = right.spf.x - left.spf.x;
        for (int i = 0; i < length; i++)
        {
            Vec2f v = Lerp(left.spf, right.spf, (float)i / length);
            v.x = left.spf.x + i;
            v.y = left.spf.y;

            ShaderContext input;

            Vec4f leftColor = left.context.varying_vec4f[0];
            Vec4f rightColor = right.context.varying_vec4f[0];
            input.varying_vec4f[0] = Lerp(leftColor, rightColor, (float)i / length);

            Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};

            //将顶点的varying传递给片段着色器
            if (_fragment_shader != NULL)
            {
                color = _fragment_shader(input);
            }

            _frame_buffer->SetPixel((int)v.x, (int)v.y, color);
        }
    }

    // 设置 顶点着色器
    void SetVertexShader(Hy_VertexShader vs)
    {
        _vertex_shader = vs;
    }

    //设置 片段着色器
    void SetFragmenthader(Hy_FragmentShader fs)
    {
        _fragment_shader = fs;
    }

    // 判断一条边是不是在三角形的左上边
    bool IsTopLeft(const Vec2i &a, const Vec2i &b)
    {
        return ((a.y == b.y) && (a.x < b.x)) || (a.y > b.y);
    }

    void SaveFile(const char *filename)
    {
        if (_frame_buffer)
            _frame_buffer->SaveFile(filename);
    }

    //重置所有
    void Reset()
    {
        _vertex_shader = NULL;
        _fragment_shader = NULL;

        if (_frame_buffer)
            delete _frame_buffer;

        if (_depth_buffer)
        {
            for (int j = 0; j < _fb_height; j++)
            {
                //删除每行的数据
                if (_depth_buffer[j])
                    delete[] _depth_buffer[j];
                _depth_buffer[j] = NULL;
            }
            delete[] _depth_buffer;
            _depth_buffer = NULL;
        }

        _color_fg = 0xffffffff;
        _color_bg = 0xff191970;
    }

    //清空 像素缓存以及深度缓存
    void Clear()
    {
        if (_frame_buffer)
        {
            //清空颜色
            _frame_buffer->Fill(_color_bg);
        }
        if (_depth_buffer)
        {
            for (int j = 0; j < _fb_height; j++)
            {
                for (int i = 0; i < _fb_width; i++)
                {
                    //重置深度缓存
                    _depth_buffer[j][i] = 0.0f;
                }
            }
        }
    }

    bool insideTriangle(Vec2f P, Vec2f A, Vec2f B, Vec2f C)
    {
        Vec2f BA = B - A;
        Vec2f CB = C - B;
        Vec2f AC = A - C;

        Vec2f PA = P - A;
        Vec2f PB = P - B;
        Vec2f PC = P - C;
        
        //叉乘判断是否位于三角形内
        //二维空间叉乘 V1(x1, y1) X V2(x2, y2) = x1y2 -y1x2
        return BA.x * PA.y - BA.y * PA.x < 0 && CB.x * PB.y - CB.y * PB.x < 0 && AC.x * PC.y - AC.y * PC.x < 0;
    }

    void computeBarycentric2D(Vec2f P, Vec2f A, Vec2f B, Vec2f C, float &a, float &b, float &c)
    {
        // a = (P.x * (B.y - C.y) + (C.x - B.x) * P.y + B.x * C.y - C.x * B.y) / (A.x * (B.y - C.y) + (C.x - B.x) * A.y + B.x * C.y - C.x * B.y);
        // b = (P.x * (C.y - A.y) + (A.x - C.x) * P.y + C.x * A.y - A.x * C.y) / (B.x * (C.y - A.y) + (A.x - C.x) * B.y + C.x * A.y - A.x * C.y);
        // c = (P.x * (A.y - B.y) + (B.x - A.x) * P.y + A.x * B.y - B.x * A.y) / (C.x * (A.y - B.y) + (B.x - A.x) * C.y + A.x * B.y - B.x * A.y);

        //当前点到3个顶点的矢量
        Vec2f s0 = A - P;
        Vec2f s1 = B - P;
        Vec2f s2 = C - P;

        //重心坐标系：计算内部子三角形面积 a / b / c
        //注意 此时的面积其实是两个点围城的平行四边形的面积
        a = Abs(vector_cross(s1, s2)); // 子三角形 Px-P1-P2 面积
        b = Abs(vector_cross(s2, s0)); // 子三角形 Px-P2-P0 面积
        c = Abs(vector_cross(s0, s1)); // 子三角形 Px-P0-P1 面积
        float s = a + b + c;           // 大三角形 P0-P1-P2 面积

        if (s == 0.0f)
        {
            cout << "重心坐标 面积 = 0" << endl;
            return;
        }

        //归一化  除以总面积，以保证：a + b + c = 1，方便用作插值系数
        a = a * (1.0f / s);
        b = b * (1.0f / s);
        c = c * (1.0f / s);
    }
};
#endif
