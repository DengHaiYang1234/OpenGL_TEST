//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include "hyMath.cpp"
//#include "Render.cpp"
//#import "Shader.hpp"
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <tool/stb_image.h>
//#include <initializer_list>
//#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#import "Shader.hpp"
#import "Camera.hpp"
#import "Texture.hpp"
#import "Model.hpp"
#include "Render.cpp"

using namespace std;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void renderDefaultTriangle(int width, int height);
void renderMVPTriangle(int width, int height);
void renderTexture(int width, int height);

Texture textureLoader;

const char *filename = "/Users/denghaiyang/OpenGL_TEST/Textures/softRenderOutput.bmp";

int main()
{

    int windowRenderBufferWith = 800;
    int windowRenderBufferHeight = 600;

    //glfw初始化
    glfwInit();
    //设置opengl3.3版本
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //创建窗口对象
    GLFWwindow *window = glfwCreateWindow(800, 600, "hySoftRender", NULL, NULL);
    if (window == NULL)
    {
        cout << "创建glfw窗口对象失败！！！" << endl;
        glfwTerminate();
        return -1;
    }

    //设置上下文
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "加载glad失败!!" << endl;
        return -1;
    }

    //设置渲染视口大小
    glViewport(0, 0, windowRenderBufferWith, windowRenderBufferHeight);
    //⭐️⭐️⭐️软渲染器测试入口
    renderMVPTriangle(windowRenderBufferWith, windowRenderBufferHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //加载opengl顶点和片段着色器
    ShaderProgram ourShader("/Users/denghaiyang/OpenGL_TEST/SoftRender/vertex.glsl", "/Users/denghaiyang/OpenGL_TEST/SoftRender/fragment.glsl");

    float vertices[] =
        {
            //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
            1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // 右上
            1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // 右下
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0, 0.0f, // 左下
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // 左上
        };

    // 索引数据
    unsigned int indices[] = {
        0, 1, 3, // 三角形一
        1, 2, 3  // 三角形二
    };

    //创建缓冲对象
    unsigned int VBO, VAO, EBO;
    //创建VAO对象
    glGenVertexArrays(1, &VAO);
    //创建VBO对象
    glGenBuffers(1, &VBO);
    //创建EBO对象
    glGenBuffers(1, &EBO);

    //绑定VAO
    glBindVertexArray(VAO);
    //绑定VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //填充VBO数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    //绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //填充EBO数据
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //设置顶点位置属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    //设置顶点颜色属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 设置顶点纹理坐标属性指针
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    //生成纹理指针
    unsigned int tex = textureLoader.TextureLoad(filename);
    
    ourShader.use();
    ourShader.set_uniform("tex", 0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 渲染指令
        // ...
        glClearColor(25.0 / 255.0, 25.0 / 255.0, 25.0 / 255.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void renderDefaultTriangle(int width, int height)
{
    //初始化渲染器和帧缓存的大小
    Render render(width, height);

    //GLSL中有三种变量类型 uniform，attribute，varying
    //1.uniform变量是外部application程序传递给（vertex和fragment）shader的变量
    //2.attribute变量是只能在vertex shader中使用的变量
    //3.varying变量是vertex和fragment shader之间做数据传递用的
    //定义一个varying的key
    const int VARYING_COLOR = 0;

    //定义三个顶点，并设置位置和颜色
    struct
    {
        Vec4f pos;
        Vec4f color;
    } vs_input[3] = {
        {{0.0, 0.7, 0.90, 1}, {1, 0, 0, 1}},
        {{-0.6, -0.2, 0.01, 1}, {0, 1, 0, 1}},
        {{+0.6, -0.2, 0.01, 1}, {0, 0, 1, 1}},
    };

    //[]为lambda的捕获区，用于捕获外部变量 https://www.jianshu.com/p/6482fbd3abdf
    //  = 表示传值
    //  & 表示传入引用
    //  [&a] 表示传入 a 变量的引用，而[a]表示传入值

    //设置顶点着色器的方法 接收顶点信息输入 输出片段着色器所需的顶点颜色，返回顶点坐标
    render.SetVertexShader([&](int index, ShaderContext &output) -> Vec4f {
        output.varying_vec4f[VARYING_COLOR] = vs_input[index].color;

        return vs_input[index].pos;
    });
    //设置片段着色器方法 根据片段的输入 获取颜色
    render.SetFragmenthader([&](ShaderContext &input) -> Vec4f {
        return input.varying_vec4f[VARYING_COLOR];
    });

    render.SetRenderState(false, true,false);
    render.DrawPrimitiveDX();
    render.SaveFile(filename);
}

void renderMVPTriangle(int width, int height)
{
    Render render(width, height);

    //opengl 模型变换是右手坐标系 看下-z方向   z值越大越近  越小越远
    Mat4x4f mat_model = matrix_set_identity();
    //opengl 视图变换是右手坐标系 看下-z方向  所以这里的看向方向需要看下-z
    Mat4x4f mat_view = matrix_set_lookat_opengl({0, 0, 1}, {0, 0, 0}, {0, 1, 0}); //参数1 看向方向  参数2:中心点  参数3:向上方向
    //opengl 裁剪和NDC坐标系都是左手坐标系 看下+z 所以这里z值需要取反
    Mat4x4f mat_proj = matrix_set_perspective_opengl(90, width * 1.0f / height * 1.0f, 0.1f, 50.0f);
    Mat4x4f mat_mvp = mat_proj * mat_view * mat_model;

    //顶点输入
    struct VertexAttrib
    {
        Vec4f pos;
        Vec4f color;
    } vs_input[3];

    //顶点着色器
    render.SetVertexShader([&](int index, ShaderContext &output) {
        output.varying_vec4f[0] = vs_input[index].color;
        cout << "变换前的顶点:" << vs_input[index].pos.x << "," << vs_input[index].pos.y << "," << vs_input[index].pos.z << "," << vs_input[index].pos.w << endl;
        Vec4f pos = mat_mvp * vs_input[index].pos;
        cout << "变换后的顶点:" << pos.x << "," << pos.y << "," << pos.z << "," << pos.w << endl;
        return pos;
    });

    //片段着色器
    render.SetFragmenthader([&](ShaderContext &input) {
        return input.varying_vec4f[0];
    });

    VertexAttrib vertex[] = {
        {{2, 0, -1, 1}, {1, 0, 0, 1}},
        {{0, 2, -1, 1}, {0, 0, 1, 1}},
        {{-2, 0, -1, 1}, {0.5f, 0.5f, 0, 1}},
        {{3.5, -1, -2, 1}, {0.5f, 1, 0, 1}},
        {{2.5, 1.5, -2, 1}, {0.6f, 0.3f, 0, 1}},
        {{-1, 0.5, -2, 1}, {0.2f, 0.8f, 0, 1}}};

    render.SetRenderState(true, false,true);

    vs_input[0] = vertex[0];
    vs_input[1] = vertex[1];
    vs_input[2] = vertex[2];
    render.DrawPrimitiveOpengl();

    vs_input[0] = vertex[3];
    vs_input[1] = vertex[4];
    vs_input[2] = vertex[5];
    render.DrawPrimitiveOpengl();

    render.SaveFile(filename);
}

void renderTexture(int width, int height)
{
    Render render(width, height);

    Bitmap texture(256, 256);
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            int k = (x / 32 + y / 32) & 1;
            texture.SetPixel(x, y, k ? 0xffffffff : 0xff3fbcef);
        }
    }

    //定义变换矩阵 模型矩阵变换 摄像机矩阵变换 透视矩阵变换

    //⭐️ 该测试脚本中的矩阵采用的行主序（DX），采用左乘的方式运算。

    //模型矩阵
    Mat4x4f mat_model = matrix_set_identity();
    //计算相机变换矩阵
    Mat4x4f mat_view = matrix_set_lookat({0, 0, -1}, {0, 0, 0}, {0, 1, 0});
    //透视矩阵
    Mat4x4f mat_proj = matrix_set_perspective(3.1415926f * 0.5f, height * 1.0f / width * 1.0f, 1.0f, 500.0f);
    //行主序的矩阵采用左乘
    Mat4x4f mat_mvp = mat_model * mat_view * mat_proj;
    //顶点输入
    struct VertexAttrib
    {
        Vec4f pos;
        Vec2f texuv;
    } vs_input[3];

    const int VARYING_TEXUV = 0;

    //顶点着色器
    render.SetVertexShader([&](int index, ShaderContext &output) {
        Vec4f pos = vs_input[index].pos * mat_mvp;
        output.varying_vec2f[VARYING_TEXUV] = vs_input[index].texuv;
        return pos;
    });

    //片段着色器
    render.SetFragmenthader([&](ShaderContext &input) {
        Vec2f coord = input.varying_vec2f[VARYING_TEXUV];
        return texture.Sample2D(coord);
    });

    VertexAttrib vertex[] = {
        {{1, -1, -1, 1}, {0, 0}},
        {{1, 1, -1, 1}, {1, 0}},
        {{-1, 1, -1, 1}, {1, 1}},
        {{-1, -1, -1, 1}, {0, 1}},
    };

    vs_input[0] = vertex[0];
    vs_input[1] = vertex[1];
    vs_input[2] = vertex[2];
    render.DrawPrimitiveDX();

    vs_input[0] = vertex[2];
    vs_input[1] = vertex[3];
    vs_input[2] = vertex[0];
    render.DrawPrimitiveDX();

    render.SaveFile(filename);
}
