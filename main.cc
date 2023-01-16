#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>

#include "shader.hpp"

using namespace std;

GLuint vao, vbo;

const float width = 495;
const float height = 495;

void draw_grid_xz(Shader &shader, float size, float step);
void draw_frustum(glm::mat4 view, glm::mat4 proj);

int main() {
    // GLFW Init
    if (!glfwInit()) {
        cerr << "GLFW init fail!" << endl;
        exit(EXIT_FAILURE);
    }

    // GLFW Window Hint 설정
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;
    window = glfwCreateWindow(width, height, "Viewer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    // Viewport 만들기
    int framebuf_width, framebuf_height;
    glfwGetFramebufferSize(window, &framebuf_width, &framebuf_height);
    glViewport(0, 0, framebuf_width, framebuf_height);

    // GLEW Init
    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    /**********************************************************/
    // main
    /**********************************************************/
    Shader shader("../shaders/draw_point.glsl");

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        draw_grid_xz(shader, 10.f, 1.f);

        glm::vec3 pos(0, 10, 10);
        glm::vec3 tar(0, 0, 0);
        glm::mat4 view = glm::lookAt(pos, tar, glm::vec3(0, 1, 0));

        float aspect = width / height;
        float zNear = 0.5;
        float zFar = 1000;

        glm::mat4 proj = glm::perspective(glm::radians(45.f), aspect, zNear, zFar);
        glm::mat4 mode = glm::mat4(1.f);

        glm::mat4 mvp = proj * view * mode;

        shader.set_mat4("mvp", mvp);

        draw_frustum(view, proj);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);

    glfwTerminate();

    return EXIT_SUCCESS;
}

void draw_grid_xz(Shader &shader, float size, float step) {
    vector<GLfloat> grid_data;

    for (float i = step; i <= size; i += step) {
        vector<GLfloat> v1{-size, 0, i, 1.f, 1.f, 1.f,
                           size,  0, i, 1.f, 1.f, 1.f}; // lines parallel to X-axis
        vector<GLfloat> v2{-size, 0, -i, 1.f, 1.f, 1.f,
                           size,  0, -i, 1.f, 1.f, 1.f}; // lines parallel to X-axis

        vector<GLfloat> v3{i, 0, -size, 1.f, 1.f, 1.f,
                           i, 0, size,  1.f, 1.f, 1.f}; // lines parallel to Z-axis
        vector<GLfloat> v4{-i, 0, -size, 1.f, 1.f, 1.f,
                           -i, 0, size,  1.f, 1.f, 1.f}; // lines parallel to Z-axis

        grid_data.insert(grid_data.end(), v1.begin(), v1.end());
        grid_data.insert(grid_data.end(), v2.begin(), v2.end());
        grid_data.insert(grid_data.end(), v3.begin(), v3.end());
        grid_data.insert(grid_data.end(), v4.begin(), v4.end());
    }

    // x-axis
    vector<GLfloat> vx{0, 0, 0, 1.f, 0, 0, size,  0, 0, 1.f, 0,   0,
                       0, 0, 0, 1.f, 0, 0, -size, 0, 0, 1.f, 1.f, 1.f};
    grid_data.insert(grid_data.end(), vx.begin(), vx.end());

    // z-axis
    vector<GLfloat> vy{0, 0, 0, 0, 0, 1.f, 0, 0, size,  0,   0,   1.f,
                       0, 0, 0, 0, 0, 1.f, 0, 0, -size, 1.f, 1.f, 1.f};
    grid_data.insert(grid_data.end(), vy.begin(), vy.end());

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, grid_data.size() * sizeof(GLfloat), &grid_data.front(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, grid_data.size());
    glBindVertexArray(0);
}

void draw_frustum(glm::mat4 view, glm::mat4 proj) {
    // reference :
    // https://gamedev.stackexchange.com/questions/183196/calculating-directional-shadow-map-using-camera-frustum

    vector<glm::vec4> clip{glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
                           glm::vec4(-1, 1, -1, 1),  glm::vec4(1, 1, -1, 1),
                           glm::vec4(-1, -1, 1, 1),  glm::vec4(1, -1, 1, 1),
                           glm::vec4(-1, 1, 1, 1),   glm::vec4(1, 1, 1, 1)};

    glm::mat4 inv_view = glm::inverse(view);
    glm::mat4 inv_proj = glm::inverse(proj);

    vector<glm::vec4> pt_worlds;
    for (size_t i = 0; i < clip.size(); i++) {
        glm::vec4 pt_view = inv_proj * clip[i];
        clip[i] /= clip[i][3];
        glm::vec4 pt_world = inv_view * pt_view;
        pt_worlds.push_back(pt_world);
    }

    int line_indices[12][2] = {
        {0, 1}, {0, 2}, {0, 4}, {1, 3}, {1, 5}, {2, 3},
        {2, 6}, {3, 7}, {4, 5}, {4, 6}, {5, 7}, {6, 7},
    };
    vector<GLfloat> frustum_data;
    for (int i = 0; i < 12; i++) {
        vector<GLfloat> line{pt_worlds[line_indices[i][0]].x,
                             pt_worlds[line_indices[i][0]].y,
                             pt_worlds[line_indices[i][0]].z,
                             0,
                             1.f,
                             0,
                             pt_worlds[line_indices[i][1]].x,
                             pt_worlds[line_indices[i][1]].y,
                             pt_worlds[line_indices[i][1]].z,
                             0,
                             1.f,
                             0};
        frustum_data.insert(frustum_data.end(), line.begin(), line.end());
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(GL_ARRAY_BUFFER, frustum_data.size() * sizeof(GLfloat), &frustum_data.front(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_LINES, 0, frustum_data.size());
    glBindVertexArray(0);
}