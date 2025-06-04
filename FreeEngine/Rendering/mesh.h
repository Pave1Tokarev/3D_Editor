#pragma once
#pragma once
#include <glad/glad.h>  
#include <iostream>
#include <vector>
#include <string>
#include "Vertex.h"
#include "Texture.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

    void Draw(unsigned int shaderProgram);

private:
    unsigned int VAO, VBO, EBO;

    void setupMesh();
};