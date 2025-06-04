#include "Line3D.h"
#include <glad/glad.h>
#include <freetype/freetype.h>
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
Line3D::Line3D(std::string name, glm::vec3 start, glm::vec3 end, glm::vec3 color) {
    this->name = name;
    this->startPoint = start;
    this->endPoint = end;
    this->color = color;
    isSelected = false;
    setupLine();
}
Line3D::Line3D() {
    VAO = 0;
    VBO = 0;
}
void Line3D::Draw(unsigned int shaderProgram) {
    glUseProgram(shaderProgram);

    if (isSelected) {
        glUniform3f(glGetUniformLocation(shaderProgram, "lineColor"), 0, 1, 0);
    }
    else {
        glUniform3f(glGetUniformLocation(shaderProgram, "lineColor"), color.x, color.y, color.z);
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}
void Line3D::setColor(glm::vec3 newColor) {
    color = newColor;
}
std::string Line3D::getName() {
    return name;
}
void Line3D::Select(bool flag) {
    isSelected = flag;
}
void Line3D::ApplyMatrix(const CoordinateTransformer& transformer) {
    startPoint = transformer.transformPoint(startPoint);
    endPoint = transformer.transformPoint(endPoint);
    setupLine();
}
void Line3D::Serialize(std::ofstream& out) {
    size_t nameSize = name.size();
    out.write(reinterpret_cast<const char*>(&nameSize), sizeof(size_t));
    out.write(name.c_str(), nameSize);
    out.write(reinterpret_cast<const char*>(&startPoint), sizeof(glm::vec3));
    out.write(reinterpret_cast<const char*>(&endPoint), sizeof(glm::vec3));
    out.write(reinterpret_cast<const char*>(&color), sizeof(glm::vec3));
    out.write(reinterpret_cast<const char*>(&isSelected), sizeof(bool));
}
void Line3D::Deserialize(std::ifstream& in) {
    size_t nameSize;
    in.read(reinterpret_cast<char*>(&nameSize), sizeof(size_t));
    name.resize(nameSize);
    in.read(&name[0], nameSize);
    in.read(reinterpret_cast<char*>(&startPoint), sizeof(glm::vec3));
    in.read(reinterpret_cast<char*>(&endPoint), sizeof(glm::vec3));
    in.read(reinterpret_cast<char*>(&color), sizeof(glm::vec3));
    in.read(reinterpret_cast<char*>(&isSelected), sizeof(bool));
    setupLine();
}
void Line3D::setupLine() {
    float vertices[] = {
        startPoint.x, startPoint.y, startPoint.z,
        endPoint.x, endPoint.y, endPoint.z
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}