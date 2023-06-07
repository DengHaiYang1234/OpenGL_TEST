//
//  main.cpp
//  3.着色器
//
//  Created by 邓海洋 on 2022/9/11.
//

//#include <glad/glad.h>
//#include <iostream>
//#include <glm/glm.hpp>
//#import "Shader.hpp"
#import "Model.hpp"
#include <GLFW/glfw3.h>
#import "CommonUtilities.hpp"

/*
 
 Uniform:是一种从CPU中的应用向GPU中的着色器发送数据的方式，但uniform和顶点属性有些不同。首先，uniform是全局的(Global)。全局意味着uniform变量必须在每个着色器程序对象中都是独一无二的，而且它可以被着色器程序的任意着色器在任意阶段访问。
    第二，无论你把uniform值设置成什么，uniform会一直保存它们的数据，直到它们被重置或更新。
 
 */


//三角形
float triangleVertices[] =
{
    //位置                颜色
    -0.5f,-0.5f,0.0f,   1.0f,0.0f,0.0f,
    0.5f,-0.5f,0.0f,    0.0f,1.0f,0.0f,
    0.0f,0.5f,0.0f,     0.0f,0.0f,1.0f,
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
    
    //绑定VBO
    //创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    //把之前定义的顶点数据复制到缓冲的内存中
    /*
     GL_STATIC_DRAW ：数据不会或几乎不会改变。
     GL_DYNAMIC_DRAW：数据会被改变很多。
     GL_STREAM_DRAW ：数据每次绘制时都会改变。
     */
    glBufferData(GL_ARRAY_BUFFER,sizeof(triangleVertices),triangleVertices,GL_STATIC_DRAW);
    

    //解析位置数据                                //步长：每个顶点有3个float构成的pos和3个float构成的color
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)0);
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    //解析颜色数据                                                   //偏移：每个顶点需偏移3个float才能找到颜色数据
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)(3 * sizeof(float)));
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(1);
    
    ShaderProgram ourShader(ApplicationPath + "3.着色器/vertex.glsl",ApplicationPath + "3.着色器/fragment.glsl");

    //线框模式
//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    
    
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        
        //当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
        //glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色
        glClearColor(0.1f,0.1f,0,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //激活这个程序对象
        ourShader.use();
        
        float timeValue = glfwGetTime();
        float greenValue = sin(timeValue) / 2.0f + 0.5f;
        ourShader.set_uniform("ourColor", 0.0f, greenValue, 0.0f);

        glBindVertexArray(VAO);

        glDrawArrays(GL_TRIANGLES,0,3);

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

