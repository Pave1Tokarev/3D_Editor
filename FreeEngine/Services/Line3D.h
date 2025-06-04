#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include "CoordinateTransformer.h"
class Line3D {
public:
    Line3D(std::string name, glm::vec3 start, glm::vec3 end, glm::vec3 color);
    Line3D();
    void Draw(unsigned int shaderProgram);
    void setColor(glm::vec3 newColor);
    std::string getName();
    void Select(bool flag);
    void ApplyMatrix(const CoordinateTransformer& transformer);
    void Serialize(std::ofstream& out);
    void Deserialize(std::ifstream& in);

private:
    std::string name;
    bool isSelected;
    glm::vec3 startPoint;
    glm::vec3 endPoint;
    glm::vec3 color;
    unsigned int VAO, VBO;
    void setupLine();
};