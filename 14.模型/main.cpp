#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#import "Shader.hpp"
#import "Model.hpp"
#import "Camera.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
Mesh creatLightMesh();
void setupLighting();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//点光源位置
glm::vec3 pointLightPositions[] = {
    glm::vec3( 0.7f,  0.2f,  2.0f),
    glm::vec3( 2.3f, -3.3f, -4.0f),
    glm::vec3(-4.0f,  2.0f, -12.0f),
    glm::vec3( 0.0f,  0.0f, -3.0f)
};

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
    
    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    Texture texture;
    texture.SetFlipVertically(true);
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // build and compile shaders
    // -------------------------
    
    ShaderProgram lightingShader("/Users/denghaiyang/OpenGL_TEST/14.模型/lightingVertexWS.glsl","/Users/denghaiyang/OpenGL_TEST/14.模型/lightingFragmentWS.glsl");
    
    ShaderProgram lightShader("/Users/denghaiyang/OpenGL_TEST/14.模型/lightVertex.glsl","/Users/denghaiyang/OpenGL_TEST/14.模型/lightFragment.glsl");
    
    
    // load models
    // -----------
    Model ourModel("/Users/denghaiyang/OpenGL_TEST/Models/nanosuit/nanosuit.obj");
    
    
    Mesh lightMesh = creatLightMesh();
    
    // draw in wireframe
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glEnable(GL_DEPTH_TEST);
    
    glm::vec3 lightColor = glm::vec3(1.0f);
    glm::vec3 diffuseColor = lightColor * glm::vec3(0.8f);//漫反射
    glm::vec3 ambientColor = lightColor * glm::vec3(0.1f);//环境光
    glm::vec3 specularColor = glm::vec3(1.0f);//高光
    
    float materialShininess = 32.0f;
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
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
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
//        // don't forget to enable shader before setting uniforms
        lightingShader.use();
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        lightingShader.set_uniform("projection", glm::value_ptr(projection));
        lightingShader.set_uniform("view", glm::value_ptr(view));

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));    // it's a bit too big for our scene, so scale it down
        lightingShader.set_uniform("model", glm::value_ptr(model));
        
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
        
        ourModel.Draw(lightingShader);
        
        
        
        //光照的mesh
        lightShader.use();
        glm::vec3 lightColor = glm::vec3(1.0f);
        lightShader.set_uniform("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightShader.set_uniform("projection", glm::value_ptr(projection));
        lightShader.set_uniform("view", glm::value_ptr(view));

        for(unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
            lightShader.set_uniform("model",glm::value_ptr(model));
            lightMesh.DrawTriangles(lightShader);
        }

        
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
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


Mesh  creatLightMesh()
{
    float cubeVerticesPositions[] = {
        // positions
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f,
        0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        
        -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };
    
    float cubeVerticesNormals[] = {
        // normals         rds
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        0.0f,  0.0f, 1.0f,
        
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,
        
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        0.0f, -1.0f,  0.0f,
        
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
        0.0f,  1.0f,  0.0f,
    };
    
    float cubeVerticesTextureCoords[] = {
        // texture coords
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,
        
        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };
    
    int cubeIndices[] =
    {
        
        //上
        0, 1, 2,
        1, 2, 3,
        //下
        4, 5, 6,
        5, 6, 7,
        //左
        2, 3, 6,
        3, 6, 7,
        //右
        0, 1, 4,
        1, 4, 5,
        //前
        0, 2, 4,
        2, 4, 6,
        //后
        1, 3, 7,
        1, 7, 5
        
        
    };
    
    vector<Vertex> lightVertex;
    
    int cubeVerticesCount = sizeof(cubeVerticesPositions) / sizeof(float) / 3;
    
    for (int i = 0; i < cubeVerticesCount; i++) {
        Vertex vertex;
        int index = i * 3;
        
        glm::vec3 position;
        position.x = cubeVerticesPositions[index];
        position.y = cubeVerticesPositions[index + 1];
        position.z = cubeVerticesPositions[index + 2];
        vertex.Position = position;
        
        glm::vec3 normal;
        normal.x = cubeVerticesNormals[index];
        normal.y = cubeVerticesNormals[index + 1];
        normal.z = cubeVerticesNormals[index + 2];
        vertex.Normal = normal;
        
        glm::vec3 texCoord;
        texCoord.x = cubeVerticesTextureCoords[index];
        texCoord.y = cubeVerticesTextureCoords[index + 1];
        vertex.TexCoords = texCoord;
        
        lightVertex.push_back(vertex);
    }
    
    int cubeIndicesCount = sizeof(cubeIndices) / sizeof(int);
    
    vector<unsigned int> indices;
    
    for (int i = 0; i < cubeIndicesCount; i++) {
        indices.push_back(cubeIndices[i]);
    }
    
    return Mesh(lightVertex,indices);
}


