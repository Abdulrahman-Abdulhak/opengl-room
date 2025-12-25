#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../../config.h"

// TODO: move implementation to Shader.cpp file
class Shader {
public:
    Shader(const std::string& shader)
        : vertPath(SHADERS_DIR + "/" + shader + "/" + shader + ".vert"), fragPath(SHADERS_DIR + "/" + shader + "/" + shader + ".frag")
    {
        compileAndLink();
        lastVertWrite = std::filesystem::last_write_time(vertPath);
        lastFragWrite = std::filesystem::last_write_time(fragPath);
    }

    Shader(const std::filesystem::path& vertPath,
           const std::filesystem::path& fragPath)
        : vertPath(vertPath), fragPath(fragPath)
    {
        compileAndLink();
        lastVertWrite = std::filesystem::last_write_time(vertPath);
        lastFragWrite = std::filesystem::last_write_time(fragPath);
    }

    ~Shader() {
        if (program) glDeleteProgram(program);
    }

    void bind() const { glUseProgram(program); }
    void unbind() const { glUseProgram(0); }

    bool reloadIfChanged() {
        try {
            auto v = std::filesystem::last_write_time(vertPath);
            auto f = std::filesystem::last_write_time(fragPath);
            if (v != lastVertWrite || f != lastFragWrite) {
                lastVertWrite = v; lastFragWrite = f;
                std::cout << "[Shader] Change detected, reloading: "
                          << vertPath << " / " << fragPath << std::endl;
                GLuint oldProg = program;
                if (compileAndLink()) {
                    if (oldProg) glDeleteProgram(oldProg);
                    return true;
                } else {
                    return false;
                }
            }
        } catch (std::exception& e) {
            std::cerr << "[Shader] Reload check failed: " << e.what() << std::endl;
        }
        return false;
    }

    GLuint getProgram() const { return program; }

    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& matrix) const;

private:
    std::filesystem::path vertPath, fragPath;
    std::filesystem::file_time_type lastVertWrite, lastFragWrite;
    GLuint program = 0;

    static std::string readFile(const std::filesystem::path& p) {
        std::ifstream in(p, std::ios::binary);
        if (!in) throw std::runtime_error("Failed to open shader file: " + p.string());
        std::ostringstream ss;
        ss << in.rdbuf();
        std::string s = ss.str();

        // Trim possible UTF-8 BOM
        if (s.size() >= 3 && (unsigned char)s[0] == 0xEF &&
            (unsigned char)s[1] == 0xBB && (unsigned char)s[2] == 0xBF)
            s.erase(0, 3);

        std::string out;
        out.reserve(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\r') {
                if (i + 1 < s.size() && s[i + 1] == '\n') continue;
                else out.push_back('\n');
            } else out.push_back(s[i]);
        }
        return out;
    }

    static std::string prepareSourceForGLSL(const std::string& source) {
        size_t i = 0;
        while (i < source.size() && isspace((unsigned char)source[i])) ++i;

        if (source.compare(i, 8, "#version") == 0) {
            size_t pos = source.find('\n', i);
            std::string result;
            if (pos == std::string::npos) {
                result = source;
            } else {
                result.reserve(source.size() + 32);
                result = source.substr(0, pos + 1);
                result += "#line 1\n";
                result += source.substr(pos + 1);
            }
            return result;
        } else {
            return "#line 1\n" + source;
        }
    }

    static bool compileShader(GLuint shader, const std::string& src, const std::string& tag) {
        const char* cstr = src.c_str();
        glShaderSource(shader, 1, &cstr, nullptr);
        glCompileShader(shader);

        GLint status = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(length);
            glGetShaderInfoLog(shader, length, &length, log.data());
            std::cerr << "=== Shader compile error in " << tag << " ===\n"
                      << log.data() << "\n"
                      << "========================================\n";
            return false;
        }
        return true;
    }

    bool compileAndLink() {
        try {
            std::string vsrc = readFile(vertPath);
            std::string fsrc = readFile(fragPath);

            vsrc = prepareSourceForGLSL(vsrc);
            fsrc = prepareSourceForGLSL(fsrc);

            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

            if (!compileShader(vs, vsrc, vertPath.string())) {
                glDeleteShader(vs); glDeleteShader(fs); return false;
            }
            if (!compileShader(fs, fsrc, fragPath.string())) {
                glDeleteShader(vs); glDeleteShader(fs); return false;
            }

            GLuint newProgram = glCreateProgram();
            glAttachShader(newProgram, vs);
            glAttachShader(newProgram, fs);
            glLinkProgram(newProgram);

            GLint linkStatus = 0;
            glGetProgramiv(newProgram, GL_LINK_STATUS, &linkStatus);
            if (linkStatus == GL_FALSE) {
                GLint length = 0;
                glGetProgramiv(newProgram, GL_INFO_LOG_LENGTH, &length);
                std::vector<char> log(length);
                glGetProgramInfoLog(newProgram, length, &length, log.data());
                std::cerr << "=== Program link error ===\n" << log.data()
                          << "\n=========================\n";
                glDeleteProgram(newProgram);
                glDeleteShader(vs);
                glDeleteShader(fs);
                return false;
            }

            if (program) glDeleteProgram(program);
            program = newProgram;
            glDetachShader(program, vs);
            glDetachShader(program, fs);
            glDeleteShader(vs);
            glDeleteShader(fs);

            return true;
        } catch (std::exception& e) {
            std::cerr << "[Shader] Exception: " << e.what() << std::endl;
            return false;
        }
    }
};
