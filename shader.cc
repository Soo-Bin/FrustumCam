#include "shader.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

Shader::Shader(std::string glsl_path) {
    glewExperimental = GL_TRUE;
    glewInit();

    // 1. retrieve the vertex/fragment source code from filePath
    string shader_code;
    ifstream shader_file;
    // ensure ifstream objects can throw exceptions:
    shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open files
        shader_file.open(glsl_path.c_str());
        stringstream shader_stream;
        // read file's buffer contents into streams
        shader_stream << shader_file.rdbuf();
        // close file handlers
        shader_file.close();
        // convert stream into string
        shader_code = shader_stream.str();
    } catch (const std::exception &e) {
        cerr << std::string("<shader-files> read failed, ") + e.what() << endl;
    } catch (...) {
        cerr << "<shader-files> read failed, unknown error" << endl;
    }

    size_t pos_version = shader_code.find("#version");
    if (pos_version == string::npos) {
        printf("[SHADER] not found glsl version popup todo");
        return;
    }
    size_t pos_define = shader_code.substr(pos_version).find('\n') + 1;

    string vertex_origin_code = shader_code;
    string fragment_origin_code = shader_code;
    const char *vertex_shader_code =
        vertex_origin_code.insert(pos_define, "#define VERTEX_SHADER\n").c_str();
    const char *fragment_shader_code =
        fragment_origin_code.insert(pos_define, "#define FRAGMENT_SHADER\n").c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader_code, nullptr);
    glCompileShader(vertex);
    check_compile_errors(vertex, "VERTEX");

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader_code, nullptr);
    glCompileShader(fragment);
    check_compile_errors(fragment, "FRAGMENT");

    program_ = glCreateProgram();
    glAttachShader(program_, vertex);
    glAttachShader(program_, fragment);
    glLinkProgram(program_);
    check_compile_errors(program_, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader() {}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void Shader::check_compile_errors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("[SHADER] ERROR::SHADER_COMPILATION_ERROR of type: %s", type.c_str());
            printf("[SHADER] log: %s", (char *)infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("[SHADER] ERROR::PROGRAM_LINKING_ERROR of type: %s", type.c_str());
            printf("[SHADER] log: %s", (char *)infoLog);
        }
    }
}
