#ifndef SRC_SHADER_C_H_
#define SRC_SHADER_C_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Shader Shader;

Shader *newShader(const char *vertexPath, const char *fragmentPath, const char *geometryPath);
void    Shader_use(Shader *shader);
void    Shader_setBool(Shader *shader, const char *name, int value);
void    Shader_setInt(Shader *shader, const char *name, int value);
void    Shader_setFloat(Shader *shader, const char *name, float value);
void    Shader_setVec3(Shader *shader, const char *name, float *value);
void    Shader_setVec3f(Shader *shader, const char *name, float x, float y, float z);
void    Shader_setMatrix4(Shader *shader, const char *name, float *mat);
void    Shader_destroy(Shader *shader);

#ifdef __cplusplus
}
#endif

#endif
