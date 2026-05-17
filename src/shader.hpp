#ifndef SRC_SHADER_HPP_
#define SRC_SHADER_HPP_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

class Shader {
public:
	unsigned int ID;

	Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);
	void use();
	void setBool(const char *name, bool value) const;
	void setInt(const char *name, int value) const;
	void setFloat(const char *name, float value) const;
	void setVec3(const char *name, const glm::vec3 &value) const;
	void setVec3f(const char *name, float x, float y, float z) const;
	void setMatrix4(const char *name, const glm::mat4 &mat) const;

private:
	void checkCompileErrors(unsigned int shader, const char *type);
	std::string readFile(const char *filePath);
};

#endif
