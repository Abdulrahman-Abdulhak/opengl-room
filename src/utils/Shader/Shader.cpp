#include "Shader.h"

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

void Shader::setMat4(const std::string& name, const glm::mat4& matrix) const
{
    GLint location = glGetUniformLocation(program, name.c_str());
    if (location == -1) {
        std::cerr << "Warning: uniform '" << name << "' doesn't exist or was optimized out\n";
        return;
    }

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
