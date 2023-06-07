//
//  main.cpp
//  34.环境光遮蔽SSAO
//
//  Created by 邓海洋 on 2022/11/10.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#import "Shader.hpp"
#import "Camera.hpp"
#import "TextureUtilities.hpp"
#import "Model.hpp"
#import "CommonUtilities.hpp"

void framebuffer_size_callback(GLFWwindow* window,int width,int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void RenderLightObject(ShaderProgram &lightShader);
void RenderQuad();
void RenderCube();
void RenderScene(ShaderProgram &shader,Model &modelObj);
GLfloat Lerp(GLfloat a,GLfloat b,GLfloat f);

const GLuint SHADW_WIDTH = 1024,SHADOW_HEIGHT = 1024;

int screenWidth = 800;
int screenHeight = 600;
float yaw,pitch;
float lastX = screenWidth / 2.0f;
float lastY = screenHeight / 2.0f;
bool firstMouse = true;
GLfloat height_scale = 0.1;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//相机位置
glm::vec3 cameraPos = glm::vec3(0.0f,0.0f,3.0f);
//相机看向方向
glm::vec3 cameraFront = glm::vec3(0.0f,0.0f,-1.0f);
//相机向上的方向
glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);
//灯光初始位置
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

float deltaTime = 0.0f;//当前帧与上一帧的时间差
float lastFrame = 0.0f;//上一帧的时间

TextureUtilities textureLoader;

int main(int argc, const char * argv[]) {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    ShaderProgram gBUfferShader(ApplicationPath + "34.环境光遮蔽SSAO/gBufferVertex.glsl",ApplicationPath + "34.环境光遮蔽SSAO/gBufferFragment.glsl");
        
    ShaderProgram lightingShader(ApplicationPath + "34.环境光遮蔽SSAO/lightingVertex.glsl",ApplicationPath + "34.环境光遮蔽SSAO/lightingFragment.glsl");
    
    ShaderProgram lightObjectShader(ApplicationPath + "34.环境光遮蔽SSAO/lightObjectVertex.glsl",ApplicationPath + "34.环境光遮蔽SSAO/lightObjectFragment.glsl");
    
    ShaderProgram previewShader(ApplicationPath + "34.环境光遮蔽SSAO/previewVertex.glsl",ApplicationPath + "34.环境光遮蔽SSAO/previewFragment.glsl");
    
    ShaderProgram ssaoShader(ApplicationPath + "34.环境光遮蔽SSAO/ssaoVertex.glsl",ApplicationPath + "34.环境光遮蔽SSAO/ssaoFragment.glsl");
    
    ShaderProgram ssaoBlurShader(ApplicationPath + "34.环境光遮蔽SSAO/ssaoBlurVertex.glsl",ApplicationPath + "34.环境光遮蔽SSAO/ssaoBlurFragment.glsl");
    
    Model backpack(ApplicationPath + "Models/backpack/backpack.obj");
    
    std::vector<glm::vec3> objectPositions;
    objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
    objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
    objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
    objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
    objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
    objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
    objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
    objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
    objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));
    
    // - ⭐️GBuufer 生成相关
    GLuint gBuffer;
    glGenFramebuffers(1,&gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER,gBuffer);
    GLuint gPositionDepth,gNormal,gAlbedoSpec;
    
    // - 位置颜色缓冲 + 深度缓存
    glGenTextures(1,&gPositionDepth);
    glBindTexture(GL_TEXTURE_2D,gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB16F,screenWidth,screenHeight,0,GL_RGBA,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,gPositionDepth,0);
    
    // - 法线颜色缓冲
    glGenTextures(1,&gNormal);
    glBindTexture(GL_TEXTURE_2D,gNormal);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB16F,screenWidth,screenHeight,0,GL_RGB,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,GL_TEXTURE_2D,gNormal,0);
    
    // - 颜色
    glGenTextures(1,&gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D,gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,screenWidth,screenHeight,0,GL_RGB,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT2,GL_TEXTURE_2D,gAlbedoSpec,0);
    
    //必须加入深度缓存，不然拷贝出来的纹理会出现深度测试失败的问题
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3,attachments);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout <<  "  ❎❎❎<编译【gBuffer】失败>❎❎❎  " << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    glEnable(GL_DEPTH_TEST);
    
    glm::vec3 lightPos = glm::vec3(2.0, 4.0, -2.0);
    glm::vec3 lightColor = glm::vec3(0.2, 0.2, 0.7);
    
    // - ⭐️SSAO半球相关
    std::uniform_int_distribution<GLfloat> randomFloats(0.0,1.0);
    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernal;
    for (GLuint i = 0; i < 64; i++) {
        glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator) * 2.0 - 1.0,
                         randomFloats(generator));
        sample = glm::normalize(sample);
        sample *= randomFloats(generator);
        GLfloat scale = GLfloat(i) / 64.0;
        scale = Lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernal.push_back(sample);
    }
    
    std::vector<glm::vec3>  ssaoNoise;
    for (GLuint i = 0; i < 16; i++) {
        glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                        randomFloats(generator) * 2.0 - 1.0,
                        0.0);
        ssaoNoise.push_back(noise);
    }
    
    GLuint noiseTexture;
    glGenTextures(1,&noiseTexture);
    glBindTexture(GL_TEXTURE_2D,noiseTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB16F,4,4,0,GL_RGB,GL_FLOAT,&ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // - 创建ssao的FBO
    GLuint ssaoFBO;
    glGenFramebuffers(1,&ssaoFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,ssaoFBO);
    GLuint ssaoColorBuffer;
    glGenTextures(1,&ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D,ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,screenWidth,screenHeight,0,GL_RGB,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
    unsigned int rboDepth1;
    glGenRenderbuffers(1, &rboDepth1);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth1);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth1);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    
    // - 创建ssao模糊处理的FBO
    GLuint ssaoBlurFBO;
    glGenFramebuffers(1,&ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER,ssaoBlurFBO);
    GLuint ssaoBlurColorBuffer;
    glGenTextures(1,&ssaoBlurColorBuffer);
    glBindTexture(GL_TEXTURE_2D,ssaoBlurColorBuffer);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RED,screenWidth,screenHeight,0,GL_RGB,GL_FLOAT,NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoBlurColorBuffer, 0);
    unsigned int rboDepth2;
    glGenRenderbuffers(1, &rboDepth2);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth2);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenWidth, screenHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth2);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);
        
        // - 1️⃣几何处理阶段：渲染到G缓冲中
        glBindFramebuffer(GL_FRAMEBUFFER,gBuffer);
//        camera.解决Mac视网膜屏幕显示不全的问题(false,screenWidth,screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gBUfferShader.use();
        gBUfferShader.set_uniform("projection", glm::value_ptr(projection));
        gBUfferShader.set_uniform("view", glm::value_ptr(view));
        gBUfferShader.set_uniform("near_plan", 0.1f);
        gBUfferShader.set_uniform("far_plan", 100.0f);
        RenderScene(gBUfferShader,backpack);
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        
        // - 2️⃣ 使用gbuffer渲染ssao纹理
        glBindFramebuffer(GL_FRAMEBUFFER,ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssaoShader.use();
        for (GLuint i = 0; i < 64; i++)
        {
            ssaoShader.set_uniform("samples[" + std::to_string(i) + "]",ssaoKernal[i].x,ssaoKernal[i].y,ssaoKernal[i].z);
        }
        ssaoShader.set_uniform("projection", glm::value_ptr(projection));
        ssaoShader.set_uniform("gPosition", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionDepth);
        ssaoShader.set_uniform("gNormal", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        ssaoShader.set_uniform("texNoise", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        
        // - 3️⃣ ssao模糊处理
        glBindFramebuffer(GL_FRAMEBUFFER,ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ssaoBlurShader.use();
        ssaoBlurShader.set_uniform("ssaoInput", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        
        
        camera.解决Mac视网膜屏幕显示不全的问题(true,screenWidth,screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // - 4️⃣ 光照渲染
        lightingShader.use();
        lightingShader.set_uniform("gPosition", 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionDepth);
        lightingShader.set_uniform("gNormal", 1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        lightingShader.set_uniform("gAlbedo", 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        lightingShader.set_uniform("ssao", 3);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);

        glm::vec3 lightPosView = glm::vec3(camera.GetViewMatrix() * glm::vec4(lightPos, 1.0));
        lightingShader.set_uniform("light.Position",lightPosView.x,lightPosView.y,lightPosView.z);
        lightingShader.set_uniform("light.Color",lightColor.x,lightColor.y,lightColor.z);
        const float linear    = 0.09f;
        const float quadratic = 0.032f;
        lightingShader.set_uniform("light.Linear",linear);
        lightingShader.set_uniform("light.Quadratic",quadratic);
        RenderQuad();
        
//        previewShader.use();
//        previewShader.set_uniform("image", 0);
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
//        RenderQuad();

        glfwSwapBuffers(window);
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
    
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        height_scale -= 0.001;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        height_scale += 0.001;
    
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

void RenderLightObject(ShaderProgram &lightShader)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
    lightShader.set_uniform("model", glm::value_ptr(model));
    RenderCube();
}

GLfloat Lerp(GLfloat a,GLfloat b,GLfloat f)
{
    return a + f * (b - a);
}

void RenderScene(ShaderProgram &shader,Model &modelObj)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0, 7.0f, 0.0f));
    model = glm::scale(model, glm::vec3(7.5f, 7.5f, 7.5f));
    shader.set_uniform("model", glm::value_ptr(model));
    RenderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
    model = glm::scale(model, glm::vec3(1.0f));
    shader.set_uniform("model", glm::value_ptr(model));
    modelObj.Draw(shader);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void RenderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
            1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}


// RenderCube() Renders a 1x1 3D cube in NDC.
unsigned int cubeVAO = 0;
unsigned int cubeVBO = 0;
void RenderCube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
            -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
            1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
            1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
            -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
