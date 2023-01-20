#include <string>
#ifndef SHADER_H
#define SHADER_H

class Shader {
  public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void set4f(const std::string &name, float r, float g, float b, float a) const;
};

#endif
