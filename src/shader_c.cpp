#include "shader_c.h"
#include "shader.hpp"

extern "C" {

Shader *newShader(const char *vertexPath, const char *fragmentPath, const char *geometryPath) {
	return new Shader(vertexPath, fragmentPath, geometryPath);
}

void Shader_use(Shader *shader) {
	shader->use();
}

void Shader_setBool(Shader *shader, const char *name, int value) {
	shader->setBool(name, (bool)value);
}

void Shader_setInt(Shader *shader, const char *name, int value) {
	shader->setInt(name, value);
}

void Shader_setFloat(Shader *shader, const char *name, float value) {
	shader->setFloat(name, value);
}

void Shader_setVec3(Shader *shader, const char *name, float *value) {
	shader->setVec3(name, glm::vec3(value[0], value[1], value[2]));
}

void Shader_setVec3f(Shader *shader, const char *name, float x, float y, float z) {
	shader->setVec3f(name, x, y, z);
}

void Shader_setMatrix4(Shader *shader, const char *name, float *mat) {
	shader->setMatrix4(name, glm::make_mat4(mat));
}

void Shader_destroy(Shader *shader) {
	delete shader;
}

}
