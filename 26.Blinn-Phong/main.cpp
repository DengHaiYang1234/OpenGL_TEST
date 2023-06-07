//
//  main.cpp
//  26.Blinn-Phong
//
//  Created by 邓海洋 on 2022/10/29.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#import "Shader.hpp"
#import "Camera.hpp"
#import "TextureUtilities.hpp"
#import "CommonUtilities.hpp"

float floorVertices[] = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
     5.0f, -0.5f,  5.0f,0.0f,  1.0f,  0.0f,  2.0f, 0.0f,
    -5.0f, -0.5f,  5.0f,0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,0.0f,  1.0f,  0.0f,  0.0f, 2.0f,
     5.0f, -0.5f,  5.0f,0.0f,  1.0f,  0.0f,  2.0f, 0.0f,
    -5.0f, -0.5f, -5.0f,0.0f,  1.0f,  0.0f,  0.0f, 2.0f,
     5.0f, -0.5f, -5.0f,0.0f,  1.0f,  0.0f,  2.0f, 2.0f
};

float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

void framebuffer_size_callback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(char const * path);

int screenWidth = 800;
int screenHeight = 600;
float yaw,pitch;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

//相机位置
glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,3.0f);
//相机看向方向
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
//相机向上的方向
glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);

float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一帧的时间

int main(int argc, const char * argv[]) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenWidth, "LearnOpenGL", NULL, NULL);
    if(window == NULL)
    {
        std::cout << "Failed to Creat GLFW window"<<std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scroll_callback);
    
    //GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前我们需要初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout<<"Failed to initialize GLAD"<<std::endl;
        return -1;
    }
    
    //设置视口大小
    glViewport(0,0,800,600);
    
    
    unsigned int LightVAO,LightVBO;
    glGenVertexArrays(1,&LightVAO);
    glBindVertexArray(LightVAO);
    
    glGenBuffers(1,&LightVBO);
    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER,LightVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),&cubeVertices[0],GL_STATIC_DRAW);
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6 * sizeof(float),(void*)(3 * sizeof(float)));
    
    //世界空间的光照实现
    ShaderProgram lightingShader(ApplicationPath + "26.Blinn-Phong/lightingVertexWS.glsl",ApplicationPath + "26.Blinn-Phong/lightingFragmentWS.glsl");
    
    ShaderProgram lightShader(ApplicationPath + "26.Blinn-Phong/lightVertex.glsl",ApplicationPath + "26.Blinn-Phong/lightFragment.glsl");
            
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        //当调用glClear函数，清除颜色缓冲之后，整个颜色缓冲都会被填充为glClearColor里所设置的颜色
        //glClearColor函数是一个状态设置函数，而glClear函数则是一个状态使用的函数，它使用了当前的状态来获取应该清除为的颜色
        glClearColor(0.1f,0.1f,0,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//每次渲染迭代之前清除深度缓冲（否则前一帧的深度信息仍然保存在缓冲中）
            
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        
        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        
        glm::mat4 model = glm::mat4(1.0f);
        
        glm::vec3 lightColor = glm::vec3(1.0f);
        
        glm::vec3 lightPos = glm::vec3(1.2f, 1.0f, 2.0f);

        lightShader.use();
        lightShader.set_uniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightShader.set_uniform("projection", glm::value_ptr(projection));
        lightShader.set_uniform("view", glm::value_ptr(view));
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightShader.set_uniform("model",glm::value_ptr(model));
        
        glBindVertexArray(LightVAO);
        glDrawArrays(GL_TRIANGLES,0,36);
        
        lightingShader.use();
        model = glm::mat4(1.0f);
        lightingShader.set_uniform("projection", glm::value_ptr(projection));
        lightingShader.set_uniform("view", glm::value_ptr(view));
        lightingShader.set_uniform("model", glm::value_ptr(model));

        
        lightingShader.set_uniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightingShader.set_uniform("lightPos", lightPos.x,lightPos.y,lightPos.z);
        
        glm::vec3 objectColor = glm::vec3(1.0f, 0.5f, 0.31f);
        lightingShader.set_uniform("objectColor", objectColor.x, objectColor.y, objectColor.z);
        lightingShader.set_uniform("viewPos", camera.Position.x,camera.Position.y,camera.Position.z);
        lightingShader.set_uniform("shininess", 0.1f);
        
        
        glBindVertexArray(LightVAO);
        glDrawArrays(GL_TRIANGLES,0,36);
        
        /*
         双缓冲(Double Buffer)
         
         应用程序使用单缓冲绘图时可能会存在图像闪烁的问题。 这是因为生成的图像不是一下子被绘制出来的，而是按照从左到右，由上而下逐像素地绘制而成的。最终图像不是在瞬间显示给用户，而是通过一步一步生成的，这会导致渲染的结果很不真实。为了规避这些问题，我们应用双缓冲渲染窗口应用程序。前缓冲保存着最终输出的图像，它会在屏幕上显示；而所有的的渲染指令都会在后缓冲上绘制。当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来，之前提到的不真实感就消除了。
         */
        
        //交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
        glfwSwapBuffers(window);
        //glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
        glfwPollEvents();
    }
    
    
    glDeleteVertexArrays(1, &LightVAO);
    glDeleteBuffers(1, &LightVBO);
    
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
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    
    lastX = xpos;
    lastY = ypos;
    
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
