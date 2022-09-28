//
//  Mesh.hpp
//  OpenGL_TEST
//
//  Created by 邓海洋 on 2022/9/28.
//


#ifndef Mesh_hpp
#define Mesh_hpp

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <string>
#include <vector>
#import "Shader.hpp"

using namespace std;

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 TexCoords;
};

struct Texture
{
    unsigned int id;
    string type;
};

class Mesh
{
public:
    vector<Vertex> vertices; //网格顶点
    vector<unsigned int> indices; //网格索引顺序
    vector<Texture> textures; //网格纹理
    
    Mesh(vector<Vertex> vertices,vector<unsigned int> indices,vector<Texture> textures);
    void Draw(ShaderProgram shader);
    
private:
    unsigned int VAO,VBO,EBO;
    void setupMesh();
};

#endif /* Mesh_hpp */
