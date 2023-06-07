//
//  main.cpp
//  22.高级GLSL
//
//  Created by 邓海洋 on 2022/10/18.
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#import "Shader.hpp"
#import "Camera.hpp"
#import "TextureUtilities.hpp"
#import "CommonUtilities.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH  / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    
    // configure global opengl state
    // -----------------------------
    //开启深度测试
    glEnable(GL_DEPTH_TEST);
    //深度测试比小运算，为了最后绘制天空盒
    glDepthFunc(GL_LEQUAL);
    
    
    // build and compile shaders
    // -------------------------
    ShaderProgram redShader(ApplicationPath + "22.高级GLSL/redVertex.glsl",ApplicationPath + "22.高级GLSL/redFragment.glsl");
    
    ShaderProgram blueShader(ApplicationPath + "22.高级GLSL/blueVertex.glsl",ApplicationPath + "22.高级GLSL/blueFragment.glsl");
    
    ShaderProgram yellowShader(ApplicationPath + "22.高级GLSL/yellowVertex.glsl",ApplicationPath + "22.高级GLSL/yellowFragment.glsl");
    
    ShaderProgram greenShader(ApplicationPath + "22.高级GLSL/greenVertex.glsl",ApplicationPath + "22.高级GLSL/greenFragment.glsl");
    
    
    
    float cubeVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        
        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
        
        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,
        
        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
    };
    
    
    
    
    vector<string> skyBoxFaces =
    {
        ApplicationTexturePath + "skybox/right.jpg",
        ApplicationTexturePath + "skybox/left.jpg",
        ApplicationTexturePath + "skybox/top.jpg",
        ApplicationTexturePath + "skybox/bottom.jpg",
        ApplicationTexturePath + "skybox/front.jpg",
        ApplicationTexturePath + "skybox/back.jpg"
    };
    
    /*
     使用两种不同的更新缓冲对象内存的方式
     */
            
    // cube VAO
    unsigned int cubeVAO1, cubeVBO1,cubeVAO2, cubeVBO2,cubeVAO3, cubeVBO3,cubeVAO4, cubeVBO4;
    glGenVertexArrays(1, &cubeVAO1);
    glGenBuffers(1, &cubeVBO1);
    glBindVertexArray(cubeVAO1);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    //====
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    
    
    glGenVertexArrays(1, &cubeVAO2);
    glGenBuffers(1, &cubeVBO2);
    glBindVertexArray(cubeVAO2);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    //====
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    
    
    glGenVertexArrays(1, &cubeVAO3);
    glGenBuffers(1, &cubeVBO3);
    glBindVertexArray(cubeVAO3);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    //====
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    
    
    glGenVertexArrays(1, &cubeVAO4);
    glGenBuffers(1, &cubeVBO4);
    glBindVertexArray(cubeVAO4);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    //====
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    
    
    
    unsigned int uniformBlockIndexRed = glGetUniformBlockIndex(redShader.ID(),"Matrices");
    unsigned int uniformBlockIndexBlue = glGetUniformBlockIndex(blueShader.ID(),"Matrices");
    unsigned int uniformBlockIndexGreen = glGetUniformBlockIndex(greenShader.ID(),"Matrices");
    unsigned int uniformBlockIndexYellow = glGetUniformBlockIndex(yellowShader.ID(),"Matrices");
    
    glUniformBlockBinding(redShader.ID(),uniformBlockIndexRed,0);
    glUniformBlockBinding(blueShader.ID(),uniformBlockIndexBlue,0);
    glUniformBlockBinding(greenShader.ID(),uniformBlockIndexGreen,0);
    glUniformBlockBinding(yellowShader.ID(),uniformBlockIndexYellow,0);
    
    unsigned int uboMatrices;
    glGenBuffers(1,&uboMatrices);
    
    glBindBuffer(GL_UNIFORM_BUFFER,uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER,2 * sizeof(glm::mat4),NULL,GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    
    glBindBufferRange(GL_UNIFORM_BUFFER,0,uboMatrices,0,2 * sizeof(glm::mat4));
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
    glBindBuffer(GL_UNIFORM_BUFFER,uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(glm::mat4),glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER,0);
    
    TextureUtilities textureLoader;
    
    // render loop
    // -----------
    while(!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // input
        // -----
        processInput(window);
        
        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        
        glBindBuffer(GL_UNIFORM_BUFFER,uboMatrices);
        glBufferSubData(GL_UNIFORM_BUFFER,sizeof(glm::mat4),sizeof(glm::mat4),glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER,0);

        //drawCube
        redShader.use();
        // cubes
        glBindVertexArray(cubeVAO1);
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 0.0f));
        redShader.set_uniform("model", glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        
        //drawCube
        greenShader.use();
        // cubes
        glBindVertexArray(cubeVAO2);
        model = glm::translate(model, glm::vec3(0.0f, 6.0f, 0.0f));
        greenShader.set_uniform("model", glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        //drawCube
        yellowShader.use();
        // cubes
        glBindVertexArray(cubeVAO3);
        model = glm::translate(model, glm::vec3(-3.0f, -3.0f, 0.0f));
        yellowShader.set_uniform("model", glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);


        //drawCube
        blueShader.use();
        // cubes
        glBindVertexArray(cubeVAO4);
        model = glm::translate(model, glm::vec3(6, 0.0, 0.0f));
        blueShader.set_uniform("model", glm::value_ptr(model));
        glDrawArrays(GL_TRIANGLES, 0, 36);

                    
        glBindVertexArray(0);
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO1);
    glDeleteBuffers(1, &cubeVBO1);
    
    glDeleteVertexArrays(1, &cubeVAO2);
    glDeleteBuffers(1, &cubeVBO2);
    
    glDeleteVertexArrays(1, &cubeVAO3);
    glDeleteBuffers(1, &cubeVBO3);
    
    glDeleteVertexArrays(1, &cubeVAO4);
    glDeleteBuffers(1, &cubeVBO4);
    
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
