//
//  main.cpp
//  10.材质
//
//  Created by 邓海洋 on 2022/9/11.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#import "Shader.hpp"
#import "Camera.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/*
 
 */


//三角形
float vertices[] =
{
    //     ---- 位置 ----
    0.5f,  0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f,
};

float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

//点光源位置
glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
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
    glBufferData(GL_ARRAY_BUFFER,sizeof(cubeVertices),cubeVertices,GL_STATIC_DRAW);
    
    //解析位置数据                                //步长：每个顶点有3个float构成的pos和3个float构成的color
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)0);
    
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    
    //解析法线数据                                //步长：每个顶点有3个float构成的pos和3个float构成的color
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //解析纹理坐标数据                                //步长：每个顶点有3个float构成的pos和3个float构成的color
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    unsigned int LightVAO;
    glGenVertexArrays(1,&LightVAO);
    glBindVertexArray(LightVAO);
    // 只需要绑定VBO不用再次设置VBO的数据，因为箱子的VBO数据中已经包含了正确的立方体顶点数据
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    //设置灯立方体的顶点属性（对我们的灯来说仅仅只有位置数据）
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8 * sizeof(float),(void*)0);
    //以顶点属性位置值作为参数，启用顶点属性
    glEnableVertexAttribArray(0);
    
    
    //世界空间的光照实现
    ShaderProgram lightingShader("/Users/denghaiyang/OpenGL_TEST/13.多光源/lightingVertexWS.glsl","/Users/denghaiyang/OpenGL_TEST/13.多光源/lightingFragmentWS.glsl");
    
    
    ShaderProgram lightShader("/Users/denghaiyang/OpenGL_TEST/13.多光源/lightVertex.glsl","/Users/denghaiyang/OpenGL_TEST/13.多光源/lightFragment.glsl");
    
    //线框模式
    //    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    
    //光的位置
    glm::vec3 lightPos(1.2f, 0.5f, 2.0f);
    
    //图像加载时翻转y轴
    stbi_set_flip_vertically_on_load(true);
    
    unsigned int diffuseTex,specularTex;
    diffuseTex = loadTexture("/Users/denghaiyang/OpenGL_TEST/Textures/container2.png");
    specularTex = loadTexture("/Users/denghaiyang/OpenGL_TEST/Textures/container2_specular.png");
    
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
        glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f);//漫反射
        glm::vec3 ambientColor = lightColor * glm::vec3(0.1f);//环境光
        glm::vec3 specularColor = glm::vec3(1.0f);//高光
        
        float materialShininess = 32.0f;
                
        //激活这个程序对象
        lightingShader.use();
        lightingShader.set_uniform("viewPos", camera.Position.x,camera.Position.y,camera.Position.z);
        lightingShader.set_uniform("projection", glm::value_ptr(projection));
        lightingShader.set_uniform("view", glm::value_ptr(view));
        lightingShader.set_uniform("material.diffuse",  0);
        lightingShader.set_uniform("material.specular", 1);
        
        lightingShader.set_uniform("material.shininess", materialShininess);
        lightingShader.set_uniform("matrixlight", (float)((1.0+sin(glfwGetTime()))/2+0.5));
        lightingShader.set_uniform("matrixmove", (float)glfwGetTime());
        lightingShader.set_uniform("lightColor", 1.0f, 1.0f, 1.0f);
        
        
        //⭐️⭐️⭐️⭐️点光源
        lightingShader.set_uniform("pointLights[0].position", pointLightPositions[0].x,pointLightPositions[0].y,pointLightPositions[0].z);
        lightingShader.set_uniform("pointLights[0].ambient",  ambientColor.x, ambientColor.y, ambientColor.z);
        lightingShader.set_uniform("pointLights[0].diffuse",  diffuseColor.x, diffuseColor.y, diffuseColor.z); // 将光照调暗了一些以搭配场景
        lightingShader.set_uniform("pointLights[0].specular", specularColor.x,specularColor.y,specularColor.z);
        lightingShader.set_uniform("pointLights[0].constant",  1.0f);
        lightingShader.set_uniform("pointLights[0].linear",    0.09f);
        lightingShader.set_uniform("pointLights[0].quadratic", 0.032f);
        
        lightingShader.set_uniform("pointLights[1].position", pointLightPositions[1].x,pointLightPositions[1].y,pointLightPositions[1].z);
        lightingShader.set_uniform("pointLights[1].ambient",  ambientColor.x, ambientColor.y, ambientColor.z);
        lightingShader.set_uniform("pointLights[1].diffuse",  diffuseColor.x, diffuseColor.y, diffuseColor.z); // 将光照调暗了一些以搭配场景
        lightingShader.set_uniform("pointLights[1].specular", specularColor.x,specularColor.y,specularColor.z);
        lightingShader.set_uniform("pointLights[1].constant",  1.0f);
        lightingShader.set_uniform("pointLights[1].linear",    0.09f);
        lightingShader.set_uniform("pointLights[1].quadratic", 0.032f);
        
        lightingShader.set_uniform("pointLights[2].position", pointLightPositions[2].x,pointLightPositions[2].y,pointLightPositions[2].z);
        lightingShader.set_uniform("pointLights[2].ambient",  ambientColor.x, ambientColor.y, ambientColor.z);
        lightingShader.set_uniform("pointLights[2].diffuse",  diffuseColor.x, diffuseColor.y, diffuseColor.z); // 将光照调暗了一些以搭配场景
        lightingShader.set_uniform("pointLights[2].specular", specularColor.x,specularColor.y,specularColor.z);
        lightingShader.set_uniform("pointLights[2].constant",  1.0f);
        lightingShader.set_uniform("pointLights[2].linear",    0.09f);
        lightingShader.set_uniform("pointLights[2].quadratic", 0.032f);
        
        lightingShader.set_uniform("pointLights[3].position", pointLightPositions[3].x,pointLightPositions[3].y,pointLightPositions[3].z);
        lightingShader.set_uniform("pointLights[3].ambient",  ambientColor.x, ambientColor.y, ambientColor.z);
        lightingShader.set_uniform("pointLights[3].diffuse",  diffuseColor.x, diffuseColor.y, diffuseColor.z); // 将光照调暗了一些以搭配场景
        lightingShader.set_uniform("pointLights[3].specular", specularColor.x,specularColor.y,specularColor.z);
        lightingShader.set_uniform("pointLights[3].constant",  1.0f);
        lightingShader.set_uniform("pointLights[3].linear",    0.09f);
        lightingShader.set_uniform("pointLights[3].quadratic", 0.032f);
//        
        
        //⭐️⭐️⭐️⭐️方向光
        lightingShader.set_uniform("directionLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.set_uniform("directionLight.ambient",  ambientColor.x, ambientColor.y, ambientColor.z);
        lightingShader.set_uniform("directionLight.diffuse",  diffuseColor.x, diffuseColor.y, diffuseColor.z); // 将光照调暗了一些以搭配场景
        lightingShader.set_uniform("directionLight.specular", specularColor.x,specularColor.y,specularColor.z);
        
        //⭐️⭐️⭐️⭐️聚光灯
        lightingShader.set_uniform("spotLight.position", camera.Position.x, camera.Position.y, camera.Position.z);
        lightingShader.set_uniform("spotLight.direction", camera.Front.x,camera.Front.y,camera.Front.z);
        lightingShader.set_uniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));//内切光角正弦值
        lightingShader.set_uniform("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));//外切光角正弦值
        lightingShader.set_uniform("spotLight.ambient",  ambientColor.x, ambientColor.y, ambientColor.z);
        lightingShader.set_uniform("spotLight.diffuse",  diffuseColor.x, diffuseColor.y, diffuseColor.z); // 将光照调暗了一些以搭配场景
        lightingShader.set_uniform("spotLight.specular", specularColor.x,specularColor.y,specularColor.z);
       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,diffuseTex);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,specularTex);
    
        glBindVertexArray(VAO);
        
        for(unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightingShader.set_uniform("model", glm::value_ptr(model));

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        
        lightShader.use();
        lightShader.set_uniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightShader.set_uniform("projection", glm::value_ptr(projection));
        lightShader.set_uniform("view", glm::value_ptr(view));
        
        glBindVertexArray(LightVAO);
        
        for(unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            lightShader.set_uniform("model",glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES,0,36);
        }
        
        /*
         双缓冲(Double Buffer)
         
         应用程序使用单缓冲绘图时可能会存在图像闪烁的问题。 这是因为生成的图像不是一下子被绘制出来的，而是按照从左到右，由上而下逐像素地绘制而成的。最终图像不是在瞬间显示给用户，而是通过一步一步生成的，这会导致渲染的结果很不真实。为了规避这些问题，我们应用双缓冲渲染窗口应用程序。前缓冲保存着最终输出的图像，它会在屏幕上显示；而所有的的渲染指令都会在后缓冲上绘制。当所有的渲染指令执行完毕后，我们交换(Swap)前缓冲和后缓冲，这样图像就立即呈显出来，之前提到的不真实感就消除了。
         */
        
        //交换颜色缓冲（它是一个储存着GLFW窗口每一个像素颜色值的大缓冲），它在这一迭代中被用来绘制，并且将会作为输出显示在屏幕上
        glfwSwapBuffers(window);
        //glfwPollEvents函数检查有没有触发什么事件（比如键盘输入、鼠标移动等）、更新窗口状态，并调用对应的回调函数（可以通过回调方法手动设置）。
        glfwPollEvents();
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteVertexArrays(1, &LightVAO);
    glDeleteBuffers(1, &VBO);
    
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

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
