#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "Rendering/camera.h"
#include "Rendering/Vertex.h"
#include "Rendering/Texture.h"
#include "Rendering/mesh.h"
#include "Shaders/ShaderPrograms.h"
#include "Services/Line3D.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_stdlib.h"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include "Services/CoordinateTransformer.h"
std::vector<Line3D> lines = std::vector<Line3D>();
std::vector<int> selectedIndices = std::vector<int>();
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f);
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
bool isMouseHidden = true;
int lineCount = 0;
bool showAddLinePopup = false;
bool showMatrixInputPopup = false;
std::string currentDir;
std::string selectedFile;

void saveLinesToFile(std::vector<Line3D>& lines, const std::string& filename) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    size_t count = lines.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(size_t));

    for (int i = 0;i < lines.size();i++) {
        lines[i].Serialize(out);
    }
}

std::vector<Line3D> loadLinesFromFile(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }

    std::vector<Line3D> lines;
    size_t count;
    in.read(reinterpret_cast<char*>(&count), sizeof(size_t));
    lines.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        Line3D line;
        line.Deserialize(in);
        lines.push_back(line);
    }

    return lines;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    
    if (isMouseHidden) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset,true);
    }
}

void showLineList() {
    if (ImGui::BeginListBox("LinesList", ImVec2(-FLT_MIN, lines.size() * ImGui::GetTextLineHeightWithSpacing()))) {
        for (int i = 0; i < lines.size(); i++) {
            bool is_selected = (std::find(selectedIndices.begin(), selectedIndices.end(), i) != selectedIndices.end());

            if (ImGui::Selectable((lines[i].getName()).c_str(), is_selected)) {
                if (ImGui::GetIO().KeyCtrl) {
                    auto it = std::find(selectedIndices.begin(), selectedIndices.end(), i);
                    if (it != selectedIndices.end()) {
                        selectedIndices.erase(it);
                    }
                    else {
                        lines[i].Select(true);
                        selectedIndices.push_back(i);
                    }

                }
                else {
                    for (int j = 0;j < selectedIndices.size();j++) {
                        lines[selectedIndices[j]].Select(false);
                    }
                    selectedIndices.clear();
                    lines[i].Select(true);
                    selectedIndices.push_back(i);
                }
            }
        }
        ImGui::EndListBox();
    }
}
void showDeleteLineButton() {
    if (ImGui::Button("Delete Selected") && !selectedIndices.empty()) {
        std::sort(selectedIndices.begin(), selectedIndices.end());
        for (auto it = selectedIndices.rbegin(); it != selectedIndices.rend(); ++it) {
            lines.erase(lines.begin() + *it);
        }
        lineCount--;
        selectedIndices.clear();
    }
}
void showAddLineButton() {
    if (ImGui::Button("Add Line")) {
        showAddLinePopup = true;
        ImGui::OpenPopup("Add New Line");
    }
    if (ImGui::BeginPopupModal("Add New Line", &showAddLinePopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        static float startX, startY, startZ, endX, endY, endZ;
        ImGui::Text("Start Point:");
        ImGui::InputFloat("Start X", &startX);
        ImGui::InputFloat("Start Y", &startY);
        ImGui::InputFloat("Start Z", &startZ);

        ImGui::Text("End Point:");
        ImGui::InputFloat("End X", &endX);
        ImGui::InputFloat("End Y", &endY);
        ImGui::InputFloat("End Z", &endZ);


        if (ImGui::Button("Create")) {

            Line3D newLine = Line3D("Line " + std::to_string(lineCount), glm::vec3(startX, startY, startZ), glm::vec3(endX, endY, endZ), glm::vec3(1, 1, 1));
            lines.push_back(newLine);
            lineCount++;
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
void showApplyMatrixButton() {
    if (ImGui::Button("Apply Matrix")) {
        showMatrixInputPopup = true;
        ImGui::OpenPopup("Matrix Input");
    }
    if (ImGui::BeginPopupModal("Matrix Input", &showMatrixInputPopup, ImGuiWindowFlags_AlwaysAutoResize)) {
        static std::vector<std::vector<double>> matrix = {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}
        };
        ImGui::Text("Enter 4x4 transformation matrix:");
        ImGui::Separator();

        for (int row = 0; row < 4; ++row) {
            ImGui::PushID(row);
            for (int col = 0; col < 4; ++col) {
                ImGui::PushItemWidth(60.0f);
                ImGui::InputDouble(("##" + std::to_string(col)).c_str(), &matrix[row][col], 0.0, 0.0, "%.3f");
                ImGui::PopItemWidth();
                if (col < 3) ImGui::SameLine();
            }
            ImGui::PopID();
        }

        if (ImGui::Button("Apply")) {
            CoordinateTransformer transformer;
            transformer.setTransformMatrix(matrix);
            for (int idx : selectedIndices) {
                if (idx >= 0 && idx < lines.size()) {
                    lines[idx].ApplyMatrix(transformer);

                }
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
}
void showSaveLoadButton() {
    if (ImGui::Button("Save / Load")) {
        if (currentDir.empty() || !std::filesystem::exists(currentDir)) {
            currentDir = std::filesystem::current_path().string();
        }
    }

    if (!currentDir.empty()) {
        ImGui::BeginChild("DirectoryTree", ImVec2(0, 200), true);
        if (std::filesystem::path(currentDir).has_parent_path()) {
            if (ImGui::Selectable("../", false)) {
                currentDir = std::filesystem::path(currentDir).parent_path().string();
                selectedFile.clear();
            }
        }

        for (const auto& entry : std::filesystem::directory_iterator(currentDir)) {
            std::string name = entry.path().filename().string();
            bool isSelected = (selectedFile == entry.path().string());

            if (entry.is_directory()) {
                name += "/";
                if (ImGui::Selectable(name.c_str(), false)) {
                    currentDir = entry.path().string();
                    selectedFile.clear();
                }
            }
            else {
                if (ImGui::Selectable(name.c_str(), isSelected)) {
                    selectedFile = entry.path().string();
                }
            }
        }
        ImGui::EndChild();

        if (!selectedFile.empty()) {
            ImGui::Text("Selected: %s", std::filesystem::path(selectedFile).filename().string().c_str());

            if (ImGui::Button("Load")) {
                lines.clear();
                selectedIndices.clear();
                lines = loadLinesFromFile(selectedFile);
                ImGui::Text("Loaded successfully!");
                currentDir = "";

            }

            ImGui::SameLine();

            if (ImGui::Button("Save")) {
                saveLinesToFile(lines, selectedFile);
                ImGui::Text("Saved successfully!");
                currentDir = "";
            }
        }
        else {
            static std::string newFileName;
            ImGui::InputText("New file name", &newFileName);

            if (ImGui::Button("Create New")) {
                if (!newFileName.empty()) {
                    selectedFile = currentDir + "/" + newFileName;
                    std::ofstream out(selectedFile);
                    out.close();
                    newFileName.clear();

                }
            }
        }

    }
}
void showInterface() {
    ImGui::Begin("Menu");
    {
        showLineList();
        showDeleteLineButton();
        showAddLineButton();
        showApplyMatrixButton();
        showSaveLoadButton();

    }
    ImGui::End();
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(0, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(1, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(2, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(3, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        static double lastPressTime = 0;
        double currentTime = glfwGetTime();
        if (currentTime - lastPressTime > 0.2) { 
            if (isMouseHidden) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                firstMouse = true; 
            }
            else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            }
            isMouseHidden = !isMouseHidden;
            lastPressTime = currentTime;
        }
    }
}

unsigned int createShaderProgram(const char* vertexShaderSource, const char* fragmentShaderSource) {
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1920, 1080, "3D Engine", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    glEnable(GL_DEPTH_TEST);

    unsigned int shaderProgram = createShaderProgram(ShaderProg::vertexShaderSource, ShaderProg::fragmentShaderSource);
    unsigned int lineShaderProgram = createShaderProgram(ShaderProg::lineVertexShaderSource, ShaderProg::lineFragmentShaderSource);

    std::vector<Texture> textures;
    Texture diffuseTexture;
    diffuseTexture.id = 0; 
    diffuseTexture.type = "texture_diffuse";
    textures.push_back(diffuseTexture);

    Texture specularTexture;
    specularTexture.id = 0; 
    specularTexture.type = "texture_specular";
    textures.push_back(specularTexture);

    Line3D X_axis("X Axis", glm::vec3(0.0f), glm::vec3(5.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    Line3D Y_axis("Y Axis",glm::vec3(0.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    Line3D Z_axis("Z Axis",glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 1.0f));


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    bool showAxes = true;
    float axesLength = 5.0f;
    glm::vec3 backgroundColor = glm::vec3(0.1, 0.1, 0.1);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


       showInterface();
       

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(lineShaderProgram);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lineShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        for (int i = 0;i < lines.size();i++) {
            lines[i].Draw(lineShaderProgram);
        }
        X_axis.Draw(lineShaderProgram);
        Y_axis.Draw(lineShaderProgram);
        Z_axis.Draw(lineShaderProgram);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    return 0;
}