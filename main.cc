#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <json/json.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include "camera.hpp"
#include "controller.hpp"
#include "shader.hpp"

using namespace std;
using json = nlohmann::json;

GLuint vao[2], vbo[2];
vector<GLfloat> line, point;

Controller control;

const float width = 600;
const float height = 600;

void draw_grid_xz(Shader &shader, float size, float step);
void draw_camera(Camera cam, glm::vec3 cam_color);

void bind_line_opengl();
void bind_point_opengl();

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        control.handle_mouse_button(xpos, ypos);
    }
}

void mouse_cursor_callback(GLFWwindow *window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        control.handle_mouse_movement(xpos, ypos);
}

void mouse_scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    control.handle_mouse_scroll(yoffset);
}

vector<Camera> read_config(const std::string file) {
    ifstream file_handler(file);
    if (!file_handler.is_open()) {
        cerr << "reading config json fail!" << endl;
        exit(EXIT_FAILURE);
    }

    string json_data;
    std::ostringstream json_oss;
    json_oss << file_handler.rdbuf();
    json_data = json_oss.str();
    file_handler.close();

    bool is_first = true;
    glm::vec3 offset;
    vector<Camera> res;
    for (auto &j : json::parse(json_data)) {
        if (is_first) {
            offset = glm::vec3(j["xyz"][0], j["xyz"][2], j["xyz"][1]);
            is_first = false;
        }
        glm::vec3 xyz(j["xyz"][0], j["xyz"][2], j["xyz"][1]);
        glm::vec3 pry(j["pry"][0], j["pry"][1], j["pry"][2]);
        res.push_back(Camera(j["cam-id"], xyz - offset, pry, j["fov"], j["width"], j["height"],
                             j["near"], j["far"]));
    }
    return res;
}

int main() {
    vector<Camera> cams = read_config("../config/object.json");
    if (cams.empty()) {
        cerr << "no camera object!" << endl;
        exit(EXIT_FAILURE);
    }

    /**********************************************************/
    // OpenGL initialize
    /**********************************************************/
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

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
    // run
    /**********************************************************/
    Shader shader("../shaders/draw_point.glsl");

    glGenVertexArrays(2, vao);
    glGenBuffers(2, vbo);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        draw_grid_xz(shader, 10.f, 1.f);

        glm::mat4 view = control.get_view();

        float aspect = width / height;
        float zNear = 0.5;
        float zFar = 1000;

        glm::mat4 proj = glm::perspective(glm::radians(control.zoom_), aspect, zNear, zFar);
        glm::mat4 mode = glm::mat4(1.f);

        glm::mat4 mvp = proj * view * mode;

        shader.set_mat4("mvp", mvp);

        for (const auto &cam : cams)
            draw_camera(cam, glm::vec3(1, 0.647059, 0));

        bind_line_opengl();
        bind_point_opengl();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, vao);
    glDeleteBuffers(1, vbo);

    glfwTerminate();

    return EXIT_SUCCESS;
}

void bind_line_opengl() {
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    glBufferData(GL_ARRAY_BUFFER, line.size() * sizeof(GLfloat), &line.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(vao[0]);
    glDrawArrays(GL_LINES, 0, line.size() / 6);
    glBindVertexArray(0);
}

void bind_point_opengl() {
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    glBufferData(GL_ARRAY_BUFFER, point.size() * sizeof(GLfloat), &point.front(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glPointSize(15);
    glDrawArrays(GL_POINTS, 0, point.size() / 6);
    glBindVertexArray(0);
}

void draw_grid_xz(Shader &shader, float size, float step) {
    for (float i = step; i <= size; i += step) {
        vector<GLfloat> v1{-size, 0, i, 1.f, 1.f, 1.f,
                           size,  0, i, 1.f, 1.f, 1.f}; // lines parallel to X-axis
        vector<GLfloat> v2{-size, 0, -i, 1.f, 1.f, 1.f,
                           size,  0, -i, 1.f, 1.f, 1.f}; // lines parallel to X-axis

        vector<GLfloat> v3{i, 0, -size, 1.f, 1.f, 1.f,
                           i, 0, size,  1.f, 1.f, 1.f}; // lines parallel to Z-axis
        vector<GLfloat> v4{-i, 0, -size, 1.f, 1.f, 1.f,
                           -i, 0, size,  1.f, 1.f, 1.f}; // lines parallel to Z-axis

        line.insert(line.end(), v1.begin(), v1.end());
        line.insert(line.end(), v2.begin(), v2.end());
        line.insert(line.end(), v3.begin(), v3.end());
        line.insert(line.end(), v4.begin(), v4.end());
    }

    // x-axis
    vector<GLfloat> vx{0, 0, 0, 1.f, 0, 0, size,  0, 0, 1.f, 0,   0,
                       0, 0, 0, 1.f, 0, 0, -size, 0, 0, 1.f, 1.f, 1.f};
    line.insert(line.end(), vx.begin(), vx.end());

    // z-axis
    vector<GLfloat> vy{0, 0, 0, 0, 0, 1.f, 0, 0, size,  0,   0,   1.f,
                       0, 0, 0, 0, 0, 1.f, 0, 0, -size, 1.f, 1.f, 1.f};
    line.insert(line.end(), vy.begin(), vy.end());
}

void draw_camera(Camera cam, glm::vec3 cam_color) {
    vector<GLfloat> pose = cam.get_pose();
    pose.push_back(cam_color.x);
    pose.push_back(cam_color.y);
    pose.push_back(cam_color.z);
    point.insert(point.end(), pose.begin(), pose.end());

    vector<GLfloat> frustum = cam.get_frustum();
    line.insert(line.end(), frustum.begin(), frustum.end());
}
