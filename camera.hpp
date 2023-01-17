#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <vector>

class Camera {
  public:
    Camera(){};
    Camera(const int &id, const glm::vec3 pos, const glm::vec3 pry, const float &fov,
           const float &w, const float &h, const float &near, const float &far)
        : id_(id), pos_(pos), pry_(pry) {
        pos_.z = -pos_.z;

        calc_front(5);

        mat_view_ = glm::lookAt(pos_, tar_, glm::vec3(0, 1, 0));
        mat_view_ = glm::rotate(glm::radians(pry_.x), glm::vec3(1, 0, 0)) * mat_view_;

        mat_proj_ = glm::perspective(glm::radians(fov), w / h, near, far);
    };

    std::vector<GLfloat> get_pose() const { return std::vector<GLfloat>{pos_.x, pos_.y, pos_.z}; };
    std::vector<GLfloat> get_frustum() const {
        // reference :
        // https://gamedev.stackexchange.com/questions/183196/calculating-directional-shadow-map-using-camera-frustum

        std::vector<glm::vec4> clip{glm::vec4(-1, -1, -1, 1), glm::vec4(1, -1, -1, 1),
                                    glm::vec4(-1, 1, -1, 1),  glm::vec4(1, 1, -1, 1),
                                    glm::vec4(-1, -1, 1, 1),  glm::vec4(1, -1, 1, 1),
                                    glm::vec4(-1, 1, 1, 1),   glm::vec4(1, 1, 1, 1)};

        glm::mat4 inv_view = glm::inverse(mat_view_);
        glm::mat4 inv_proj = glm::inverse(mat_proj_);

        std::vector<glm::vec4> pt_worlds;
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
        std::vector<GLfloat> frustum_data;
        for (int i = 0; i < 12; i++) {
            std::vector<GLfloat> line{pt_worlds[line_indices[i][0]].x,
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

        return frustum_data;
    }

  private:
    void calc_front(const double &distance) {
        // reference --> https://slideplayer.com/slide/16393785/
        double x = -distance * sin(glm::radians(pry_.z));
        double z = distance * cos(glm::radians(pry_.z));

        tar_ = glm::vec3(pos_.x + x, pos_.y, pos_.z + z);
    };

  private:
    int id_;

    glm::vec3 pos_;
    glm::vec3 tar_;
    glm::vec3 pry_;

    glm::mat4 mat_view_;
    glm::mat4 mat_proj_;
};

#endif