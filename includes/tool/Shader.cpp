//
//  Shader.cpp
//  OpenGL_TEST
//
//  Created by 邓海洋 on 2022/9/12.
//

#include "Shader.hpp"


Shader::Shader(std::string_view file_path)
: id_{ 0 }
{
    std::ifstream fs{};
    fs.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        fs.open(std::string{ file_path });
        std::stringstream ss{};
        ss << fs.rdbuf();
        fs.close();
        source_ = ss.str();
    }
    catch (std::ifstream::failure e) {
        std::cout << "  ❎❎❎<shader源文件读取失败>❎❎❎    错误地址:"<< "，[路径:" << file_path << "]  "   << file_path << std::endl;
    }
}

Shader::~Shader()
{
    if (id_ != 0)
        glDeleteShader(id_);
}

VertexShader::VertexShader(std::string_view file_path)
: Shader{ file_path }
{
    id_ = glCreateShader(GL_VERTEX_SHADER);
    auto source_str = source_.c_str();
    glShaderSource(id_, 1, &source_str, nullptr);
    glCompileShader(id_);
    
    int success{};
    char log[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id_, 512, nullptr, log);
        std::cout << "  ❎❎❎<编译【顶点】着色器失败>❎❎❎  "<< "，[路径:" << file_path << "]  "   << log << std::endl;
    }
}

GeometryShader::GeometryShader(std::string_view file_path)
: Shader{ file_path }
{
    id_ = glCreateShader(GL_GEOMETRY_SHADER);
    auto source_str = source_.c_str();
    glShaderSource(id_, 1, &source_str, NULL);
    glCompileShader(id_);
    
    int success{};
    char log_info[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id_, 512, nullptr, log_info);
        std::cout << "  ❎❎❎<编译【几何】着色器失败>❎❎❎  "<< "，[路径:" << file_path << "]  "   << log_info << std::endl;
    }
}

FragmentShader::FragmentShader(std::string_view file_path)
: Shader{ file_path }
{
    id_ = glCreateShader(GL_FRAGMENT_SHADER);
    auto source_str = source_.c_str();
    glShaderSource(id_, 1, &source_str, nullptr);
    glCompileShader(id_);
    
    int success{};
    char log_info[512];
    glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id_, 512, nullptr, log_info);
        std::cout << "  ❎❎❎<编译【片段】着色器失败>❎❎❎  " << "，[路径:" << file_path << "]  "  << log_info << std::endl;
    }
}

ShaderProgram::ShaderProgram(std::string_view vertex_shader, std::string_view fragment_shader)
: id_{ 0 }
{
    VertexShader vertex{ vertex_shader };
    FragmentShader fragment{ fragment_shader };
    
    id_ = glCreateProgram();
    glAttachShader(id_, vertex.get_id());
    glAttachShader(id_, fragment.get_id());
    glLinkProgram(id_);
    
    int success{};
    char log_info[512];
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id_, 512, nullptr, log_info);
        std::cout <<  "  ❎❎❎<链接【ShaderProgram】失败>❎❎❎  " << log_info << std::endl;
    }
}

ShaderProgram::ShaderProgram(std::string_view vertex_shader, std::string_view geometry_shader,std::string_view fragment_shader): id_{ 0 }
{
    VertexShader vertex{ vertex_shader };
    GeometryShader geometry{ geometry_shader };
    FragmentShader fragment{ fragment_shader };
    
    id_ = glCreateProgram();
    glAttachShader(id_, vertex.get_id());
    glAttachShader(id_, fragment.get_id());
    glAttachShader(id_, geometry.get_id());
    glLinkProgram(id_);
    
    int success{};
    char log_info[512];
    glGetProgramiv(id_, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(id_, 512, nullptr, log_info);
        std::cout <<  "  ❎❎❎<链接【ShaderProgram】失败>❎❎❎  " << log_info << std::endl;
    }
}

ShaderProgram::~ShaderProgram()
{
    if (id_ != 0)
        glDeleteProgram(id_);
}

void ShaderProgram::set_uniform(std::string_view name, bool value) const noexcept
{
    glUniform1i(glGetUniformLocation(id_, name.data()), static_cast<int>(value));
}

void ShaderProgram::set_uniform(std::string_view name, int value) const noexcept
{
    glUniform1i(glGetUniformLocation(id_, name.data()), value);
}

void ShaderProgram::set_uniform(std::string_view name,unsigned int value) const noexcept
{
    glUniform1i(glGetUniformLocation(id_, name.data()), value);
}

void ShaderProgram::set_uniform(std::string_view name, float value) const noexcept
{
    glUniform1f(glGetUniformLocation(id_, name.data()), value);
}



void ShaderProgram::set_uniform(std::string_view name, float v1,float v2,float v3,float v4) const noexcept
{
    glUniform4f(glGetUniformLocation(id_, name.data()), v1,v2,v3,v4);
}

void ShaderProgram::set_uniform(std::string_view name, float v1,float v2,float v3) const noexcept
{
    glUniform3f(glGetUniformLocation(id_, name.data()), v1,v2,v3);
}

void ShaderProgram::set_uniform(std::string_view name, float v1,float v2) const noexcept
{
    glUniform2f(glGetUniformLocation(id_, name.data()), v1,v2);
}

void ShaderProgram::set_uniform(std::string_view name,float* v) const noexcept
{
    //参数2：如果目标统一变量不是矩阵数组，则为1，如果是矩阵数组，则为1或更多。
    //参数3：是否转置
    glUniformMatrix4fv(glGetUniformLocation(id_, name.data()),1,GL_FALSE,v);
}

void ShaderProgram::use() const noexcept
{
    glUseProgram(id_);
}
