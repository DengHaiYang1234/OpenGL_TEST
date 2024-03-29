//
//  main.cpp
//  3.着色器
//
//  Created by 邓海洋 on 2022/9/11.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#import "Shader.hpp"
#import "CommonUtilities.hpp"
#import "TextureUtilities.hpp"

/*
 纹理环绕方式：
 GL_REPEAT    对纹理的默认行为。重复纹理图像。
 GL_MIRRORED_REPEAT    和GL_REPEAT一样，但每次重复图片是镜像放置的。
 GL_CLAMP_TO_EDGE    纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
 GL_CLAMP_TO_BORDER    超出的坐标为用户指定的边缘颜色。
 
 纹理过滤方式：
 GL_NEAREST   会选择中心点最接近纹理坐标的那个像素
 GL_LINEAR    它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大
 GL_NEAREST_MIPMAP_NEAREST    使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
 GL_LINEAR_MIPMAP_NEAREST    使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
 GL_NEAREST_MIPMAP_LINEAR    在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
 GL_LINEAR_MIPMAP_LINEAR    在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
 纹理过滤方式：
 */


//三角形
float vertices[] =
{
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
};

//float vertices[] = {
//        // positions          // colors           // texture coords (note that we changed them to 'zoom in' on our texture image)
//         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.55f, 0.55f, // top right
//         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   0.55f, 0.45f, // bottom right
//        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.45f, 0.45f, // bottom left
//        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.45f, 0.55f  // top left
//    };

unsigned int indices[] =
{
    //⭐️ 顺时针
    // 注意索引从0开始!
    // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
    // 这样可以由下标代表顶点组合成矩形
    0,1,3,
    1,2,3
};


void framebuffer_size_callback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow *window);

int main(int argc, const char * argv[]) {
    
    
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to Creat GLFW window"<<std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    //GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前我们需要初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"Failed to initialize GLAD"<<std::endl;
        return -1;
    }
    
    //设置视口大小
    glViewport(0,0,800,600);
    
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,&nrAttributes);
    std::cout << "当前设备支持的最大顶点属性个数： " << nrAttributes << std::endl;
  

    //创建VAO
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    
    //VBO
    unsigned int VBO;
    //一个缓冲ID生成一个VBO对象
    glGenBuffers(1,&VBO);
    
    unsigned int EBO;
    glGenBuffers(1,&EBO);
    
    //绑定VBO
    //创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    //把之前定义的顶点数据复制到缓冲的内存中
    /*
     GL_STATIC_DRAW ：数据不会或几乎不会改变。
     GL_DYNAMIC_DRAW：数据会被改变很多。
     GL_STREAM_DRAW ：数据每次绘制时都会改变。
     */
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    

    //解析位置数据                                //步长：每个顶点有3个float构成的pos和3个float构成的color
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)0);
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    //解析颜色数据                                                   //偏移：每个顶点需偏移3个float才能找到颜色数据
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(3 * sizeof(float)));
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(1);
    //解析纹理坐标数据                                                   //偏移：每个顶点需偏移3个float才能找到颜色数据
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(6 * sizeof(float)));
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(2);
    
    ShaderProgram ourShader(ApplicationPath + "4.纹理/vertex.glsl",ApplicationPath + "4.纹理/fragment.glsl");

    //线框模式
//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    
    TextureUtilities textureUtilities;
    
    //图像加载时翻转y轴
    textureUtilities.SetFlipVertically(true);
    
    unsigned int texture,texture1;
    texture =textureUtilities.LoadTextureFromPath(ApplicationTexturePath + "container.jpeg");
    texture1 =textureUtilities.LoadTextureFromPath(ApplicationTexturePath + "awesomeface.png");
    
    

    //激活这个程序对象
    ourShader.use();
    
    //这里的索引0或1 会与下方的GL_TEXTURE0或GL_TEXTURE1绑定
    ourShader.set_uniform("texture1", 0);
    ourShader.set_uniform("texture2", 1);
    
    
    
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        //当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
        //glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色
        glClearColor(0.1f,0.1f,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,texture1);
        
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);

        glBindVertexArray(0);
        
        /*
         双缓冲(Double Buffer)

         应用程序使用单缓冲绘图时可能会存在图像闪烁的问题。 这是因为生成的图像不是一下子被绘制出来的，而是按照从左到右，由上而下逐像素地绘制而成的。最终图像不是在瞬间显示给用户，而是通过一步一步生成的，这会导致渲染的结果很不真实。为了规避这些问题，我们应用双缓冲渲染窗口应用程序。前缓冲保存着最终输出的图像，它会在屏幕上显示；而所有的的渲染指令都会在后缓冲上绘制。当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来，之前提到的不真实感就消除了。
         */
        
        //交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
        glfwSwapBuffers(window);
        //glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
        glfwPollEvents();
    }
    
    glfwTerminate();
    return  0;
}


//每次窗口大小被调整的时候被调用
void framebuffer_size_callback(GLFWwindow* window,int width,int height)
{
    glViewport(0,0,width,height);
}

//接收一个按键输入
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

