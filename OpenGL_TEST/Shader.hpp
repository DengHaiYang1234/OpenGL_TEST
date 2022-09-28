//
//  Shader.hpp
//  OpenGL_TEST
//
//  Created by 邓海洋 on 2022/9/12.
//

#ifndef Shader_hpp
#define Shader_hpp

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <string_view>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>

class Shader {
public:
    explicit Shader(std::string_view file_path);
    
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    Shader(Shader&& other) noexcept = default;
    Shader& operator=(Shader&& other) noexcept = default;
    
    ~Shader();
    
    constexpr unsigned get_id() const noexcept { return id_; }
    
protected:
    unsigned id_;
    std::string source_;
};

class VertexShader: public Shader {
public:
    explicit VertexShader(std::string_view file_path);
};

class FragmentShader: public Shader {
public:
    explicit FragmentShader(std::string_view file_path);
};

class ShaderProgram {
public:
    ShaderProgram(std::string_view vertex_shader, std::string_view fragment_shader);
    
    ~ShaderProgram();
    
    void use() const noexcept;
    
    void set_uniform(std::string_view name, bool value) const noexcept;
    void set_uniform(std::string_view name, int value) const noexcept;
    void set_uniform(std::string_view name, unsigned int value) const noexcept;
    void set_uniform(std::string_view name, float value) const noexcept;
    void set_uniform(std::string_view name, float v1,float v2,float v3) const noexcept;
    void set_uniform(std::string_view name, float v1,float v2,float v3,float v4) const noexcept;
    void set_uniform(std::string_view name,float* v) const noexcept;
    
private:
    unsigned id_;
};

#endif /* Shader_hpp */
