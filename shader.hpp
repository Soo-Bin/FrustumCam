#ifndef __SHADER_HPP__
#define __SHADER_HPP__

#include "GL/glew.h"

#include <string>

#include <glm/glm.hpp>

class Shader {
  public:
    Shader(std::string glsl_file);
    ~Shader();

    // activate the shader
    // ------------------------------------------------------------------------
    void use() { glUseProgram(program_); }

    // utility uniform functions
    // ------------------------------------------------------------------------
    void set_bool(const std::string &name, bool value) const {
        glUniform1i(glGetUniformLocation(program_, name.c_str()), (int)value);
    }
    void set_int(const std::string &name, int value) const {
        glUniform1i(glGetUniformLocation(program_, name.c_str()), value);
    }
    void set_float(const std::string &name, float value) const {
        glUniform1f(glGetUniformLocation(program_, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void set_vec2(const std::string &name, const glm::vec2 &value) const {
        glUniform2fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
    }
    void set_vec2(const std::string &name, float x, float y) const {
        glUniform2f(glGetUniformLocation(program_, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void set_vec3(const std::string &name, const glm::vec3 &value) const {
        glUniform3fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
    }
    void set_vec3(const std::string &name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(program_, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void set_vec4(const std::string &name, const glm::vec4 &value) const {
        glUniform4fv(glGetUniformLocation(program_, name.c_str()), 1, &value[0]);
    }
    void set_vec4(const std::string &name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(program_, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void set_mat2(const std::string &name, const glm::mat2 &mat) const {
        glUniformMatrix2fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void set_mat3(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix3fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    void set_mat4(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(program_, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

  private:
    void check_compile_errors(GLuint shader, std::string type);

  private:
    GLuint program_;
};

#endif
