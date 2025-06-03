#include "CoordinateTransformer.h"

CoordinateTransformer::CoordinateTransformer() {
    matrix = {
        {1.0, 0.0, 0.0, 0.0},
        {0.0, 1.0, 0.0, 0.0},
        {0.0, 0.0, 1.0, 0.0},
        {0.0, 0.0, 0.0, 1.0}
    };
}

void CoordinateTransformer::setTransformMatrix(const std::vector<std::vector<double>>& newMatrix) {
    if (newMatrix.size() == 4 && newMatrix[0].size() == 4) {
        matrix = newMatrix;
    }
}

glm::vec3 CoordinateTransformer::transformPoint(const glm::vec3& point) const {
    std::vector<double> input = { point.x, point.y, point.z, 1.0 };
    std::vector<double> result(4, 0.0);

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[i] += matrix[i][j] * input[j];
        }
    }

    // Ќормализаци€ только если w != 0 и w != 1
    if (result[3] != 0.0 && result[3] != 1.0) {
        const double w = result[3];
        // ¬озвращаем точку с w=1 после нормализации
        return glm::vec3(result[0] / w, result[1] / w, result[2] / w);
    }

    // ≈сли w == 1, возвращаем как есть
    // ≈сли w == 0, это направление (вектор), возвращаем без изменени€
    return glm::vec3(result[0], result[1], result[2]);
}