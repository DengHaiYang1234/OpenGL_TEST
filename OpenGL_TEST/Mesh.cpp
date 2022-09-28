//
//  Mesh.cpp
//  OpenGL_TEST
//
//  Created by 邓海洋 on 2022/9/28.
//

#include "Mesh.hpp"


Mesh::Mesh(vector<Vertex> vertices,vector<unsigned int> indices,vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    
    setupMesh();
}

void Mesh::Draw(ShaderProgram shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    
    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);// 在绑定之前激活相应的纹理单元
        string number;
        string name = textures[i].type;
        if(name == "texture_diffuse")
        {
            number = std::to_string(diffuseNr++);
        }
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);
            
        shader.set_uniform(("material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D,textures[i].id);
    }
    
    glActiveTexture(GL_TEXTURE0);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
 
void Mesh:: setupMesh()
{
    //生成
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    
    //绑定
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size() * sizeof(vertices),&vertices[0],GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size() * sizeof(unsigned int),&indices[0],GL_STATIC_DRAW);
    
    //填值
    //顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)0);
    
    //顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, Normal));
    
    //顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex, TexCoords));
    
    glBindVertexArray(0);
}
