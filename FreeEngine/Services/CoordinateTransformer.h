#pragma once
#include <vector>
#include <glm/glm.hpp>

class CoordinateTransformer {
public:
    CoordinateTransformer();
    void setTransformMatrix(const std::vector<std::vector<double>>& matrix);
    glm::vec3 transformPoint(const glm::vec3& point) const;

private:
    std::vector<std::vector<double>> matrix;
};