//
//  main.cpp
//  2.你好，三角形
//
//  Created by 邓海洋 on 2022/9/11.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#import "CommonUtilities.hpp"

/*
 
 顶点数组对象：Vertex Array Object，VAO
 顶点缓冲对象：Vertex Buffer Object，VBO  它会在GPU内存（通常被称为显存）中储存大量顶点
 元素缓冲对象：Element Buffer Object，EBO 或 索引缓冲对象 Index Buffer Object，IBO
 
 */

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"

    "void main()\n"
    "{\n"
        "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";

const char *fragmentShaderSource1 = "#version 330 core\n"
    "out vec4 FragColor;\n"

    "void main()\n"
    "{\n"
        "FragColor = vec4(0.5f, 0.5f, 1.0f, 1.0f);\n"
    "}\0";

//三角形
float triangleVertices[] =
{
    -0.5f,-0.5f,0.0f,//左下
    0.5f,-0.5f,0.0f,//右下
    0.0f,0.5f,0.0f//正上
};

//矩形
float rectangleVertices[] =
{
    0.5f,0.5f,0.0f,//右上
    0.5f,-0.5f,0.0f,//右下
    -0.5f,-0.5f,0.0f,//左下
    -0.5f,0.5f,0.0f,//左上
};

unsigned int rectangleIndices[] =
{
    //⭐️ 顺时针
    // 注意索引从0开始!
    // 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
    // 这样可以由下标代表顶点组合成矩形
    0,1,3,
    1,2,3
};

//两个相连的三角形
float closeNeighbourTriangleVertices[] =
{
    0.5f,1.0f,0,
    1.0f,0.5f,0.0f,
    0.0f,0.5f,0.0f,
    -0.5f,1.0f,0.0f,
    -1.0f,0.5f,0.0f,
};

unsigned int closeNeighbourIndices[] =
{
    0,1,2,
    3,2,4
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
  
    //###########⭐️⭐️⭐️⭐️一个标准VBO的数据绑定及使用流程(渲染会绑定数据)⭐️⭐️⭐️
//    unsigned int VBO;
//    //一个缓冲ID生成一个VBO对象
//    glGenBuffers(1,&VBO);
//    //创建的缓冲绑定到GL_ARRAY_BUFFER目标上
//    glBindBuffer(GL_ARRAY_BUFFER,VBO);
//    //把之前定义的顶点数据复制到缓冲的内存中
//    /*
//     GL_STATIC_DRAW ：数据不会或几乎不会改变。
//     GL_DYNAMIC_DRAW：数据会被改变很多。
//     GL_STREAM_DRAW ：数据每次绘制时都会改变。
//     */
//    glBufferData(GL_ARRAY_BUFFER,sizeof(veetices),veetices,GL_STATIC_DRAW);
//
//    //解析顶点数据
//    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
//    //以顶点属性位置值作为参数，启用顶点属性
//    glEnableVertexAttribArray(0);
    
    ////###########⭐️⭐️⭐️⭐️一个标准VBO的数据绑定及使用流程(渲染会绑定数据)⭐️⭐️⭐️
//    unsigned int EBO;
//    glGenBuffers(1,&EBO);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    
    //创建VAO
    unsigned int VAO,VAO1;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
    
    //VBO
    unsigned int VBO,VBO1;
    //一个缓冲ID生成一个VBO对象
    glGenBuffers(1,&VBO);
    
    //EBO
    unsigned int EBO,EBO1;
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
    glBufferData(GL_ARRAY_BUFFER,sizeof(closeNeighbourTriangleVertices),closeNeighbourTriangleVertices,GL_STATIC_DRAW);
    
    //绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(closeNeighbourIndices),closeNeighbourIndices,GL_STATIC_DRAW);
    
    //解析顶点数据
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    
    
    glGenVertexArrays(1,&VAO1);
    glBindVertexArray(VAO1);
    glGenBuffers(1,&VBO1);
    glGenBuffers(1,&EBO1);
    //绑定VBO
    //创建的缓冲绑定到GL_ARRAY_BUFFER目标上
    glBindBuffer(GL_ARRAY_BUFFER,VBO1);
    //把之前定义的顶点数据复制到缓冲的内存中
    /*
     GL_STATIC_DRAW ：数据不会或几乎不会改变。
     GL_DYNAMIC_DRAW：数据会被改变很多。
     GL_STREAM_DRAW ：数据每次绘制时都会改变。
     */
    glBufferData(GL_ARRAY_BUFFER,sizeof(rectangleVertices),rectangleVertices,GL_STATIC_DRAW);
    
    //绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(rectangleIndices),rectangleIndices,GL_STATIC_DRAW);
    
    //解析顶点数据
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3 * sizeof(float),(void*)0);
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    
    
    //创建并编译顶点着色器
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //编译的着色器对象作为第一个参数。第二参数指定了传递的源码字符串数量，这里只有一个。第三个参数是顶点着色器真正的源码
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    glCompileShader(vertexShader);
    
    
    int success;
    char infoLog[512];
    //检测顶点着色器是否编译成功
    glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
        std::cout << "==>  [顶点着色器编译失败!]" << infoLog << std::endl;
    }
    
    //创建并编译片段着色器
    unsigned int fragmentShader,fragmentShader1;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    //编译的着色器对象作为第一个参数。第二参数指定了传递的源码字符串数量，这里只有一个。第三个参数是顶点着色器真正的源码
    glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
    glCompileShader(fragmentShader);
    
    fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    //编译的着色器对象作为第一个参数。第二参数指定了传递的源码字符串数量，这里只有一个。第三个参数是顶点着色器真正的源码
    glShaderSource(fragmentShader1,1,&fragmentShaderSource1,NULL);
    glCompileShader(fragmentShader1);
    
    
    //检测片段着色器是否编译成功
    glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
        std::cout << "==>  [fragmentShader片段着色器编译失败!]" << infoLog << std::endl;
    }
    
    //检测片段着色器是否编译成功
    glGetShaderiv(fragmentShader1,GL_COMPILE_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader1,512,NULL,infoLog);
        std::cout << "==>  [fragmentShader1片段着色器编译失败!]" << infoLog << std::endl;
    }
    
    //创建着色器程序对象，用于渲染对象的时候激活这个着色器程序
    unsigned int shaderProgram,shaderProgram1;
    shaderProgram = glCreateProgram();
    shaderProgram1 =glCreateProgram();
    //把之前编译的着色器附加到程序对象上
    glAttachShader(shaderProgram,vertexShader);
    glAttachShader(shaderProgram,fragmentShader);
    glLinkProgram(shaderProgram);
    
    glAttachShader(shaderProgram1,vertexShader);
    glAttachShader(shaderProgram1,fragmentShader1);
    glLinkProgram(shaderProgram1);
    
    //检测着色器程序对象是否编译成功
    glGetShaderiv(shaderProgram,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(shaderProgram,512,NULL,infoLog);
        std::cout << "==>  [链接着色器程序失败!]" << infoLog << std::endl;
    }
    
    //检测着色器程序对象是否编译成功
    glGetShaderiv(shaderProgram1,GL_LINK_STATUS,&success);
    if(!success)
    {
        glGetShaderInfoLog(shaderProgram1,512,NULL,infoLog);
        std::cout << "==>  [链接着色器程序失败!]" << infoLog << std::endl;
    }
    
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
        glUseProgram(shaderProgram);
        
        glBindVertexArray(VAO);
        /*
         第一个参数指定了我们绘制的模式，这个和glDrawArrays的一样;
         第二个参数是我们打算绘制顶点的个数，这里填6，也就是说我们一共需要绘制6个顶点;
         第三个参数是索引的类型，这里是GL_UNSIGNED_INT;
         最后一个参数里我们可以指定EBO中的偏移量（或者传递一个索引数组，但是这是当你不在使用索引缓冲对象的时候），但是我们会在这里填写0。
         */
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        
//        glDrawArrays(GL_TRIANGLES,0,3);
        
        //激活这个程序对象
        glUseProgram(shaderProgram1);
        
        glBindVertexArray(VAO1);
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
    
    //在把着色器对象链接到程序对象以后，可以删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
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

