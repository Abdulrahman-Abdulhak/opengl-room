#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

void Shader::setInt(const std::string& name, int value) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or was optimized out\n";
        return;
    }

    glUniform1i(location, value);
}

void Shader::setFloat(const std::string& name, float value) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or was optimized out\n";
        return;
    }

    glUniform1f(location, value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or was optimized out\n";
        return;
    }

    glUniform2fv(location, 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or was optimized out\n";
        return;
    }

    glUniform3fv(location, 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const
{
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or was optimized out\n";
        return;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
