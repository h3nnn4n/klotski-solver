#include "shader.hpp"

std::string Shader::readFile(const char *filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) {
        std::cerr << "Failed to open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

Shader::Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath) {
    std::string vertexCode   = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    unsigned int geometry = 0;
    if (geometryPath) {
        std::string geometryCode = readFile(geometryPath);
        const char *gShaderCode  = geometryCode.c_str();
        geometry                 = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, nullptr);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometry)
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometry)
        glDeleteShader(geometry);
}

void Shader::use() { glUseProgram(ID); }

void Shader::setBool(const char *name, bool value) const { glUniform1i(glGetUniformLocation(ID, name), (int)value); }

void Shader::setInt(const char *name, int value) const { glUniform1i(glGetUniformLocation(ID, name), value); }

void Shader::setFloat(const char *name, float value) const { glUniform1f(glGetUniformLocation(ID, name), value); }

void Shader::setVec3(const char *name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::setVec3f(const char *name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ID, name), x, y, z);
}

void Shader::setMatrix4(const char *name, const float *mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, mat);
}

void Shader::checkCompileErrors(unsigned int shader, const char *type) {
    int  success;
    char infoLog[1024];

    if (type != std::string("PROGRAM")) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader compilation error (" << type << "):\n" << infoLog << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Program linking error:\n" << infoLog << std::endl;
        }
    }
}
