#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>

constexpr float FOV = 45.f;
constexpr float SENSITIVITY = 0.1f;
constexpr float EPSILON = 0.00001f;
constexpr float SPEED = 0.05f;

constexpr int WIDTH = 600;
constexpr int HEIGHT = 600;

constexpr float ZNEAR = 0.1f;
constexpr float ZFAR = 1000.f;

class Controller {
  public:
    Controller() : zoom_(FOV) {
        pos_ = glm::vec3(0, 10, 10);
        tar_ = glm::vec3(0, 0, 0);

        forward_ = tar_ - pos_;
        forward_ = glm::normalize(forward_);
        left_ = glm::normalize(glm::cross(forward_, glm::vec3(0, 1, 0)));
        up_ = glm::normalize(glm::cross(left_, forward_));

        distance_ = std::sqrt(std::pow(pos_.x - tar_.x, 2) + std::pow(pos_.y - tar_.y, 2) +
                              std::pow(pos_.z - tar_.z, 2));

        glm::mat4 mat = glm::mat4(1.f);
        mat = glm::lookAt(pos_, tar_ + forward_, up_);

        // find yaw (around y-axis) first
        // NOTE: asin() returns -90~+90, so correct the angle range -180~+180
        // using z value of forward vector
        yaw_ = glm::degrees(asinf(mat[2][0]));
        if (mat[2][2] < 0) {
            if (yaw_ >= 0)
                yaw_ = 180.0f - yaw_;
            else
                yaw_ = -180.0f - yaw_;
        }

        // find roll (around z-axis) and pitch (around x-axis)
        // if forward vector is (1,0,0) or (-1,0,0), then m[0]=m[4]=m[9]=m[10]=0
        if (mat[0][0] > -EPSILON && mat[0][0] < EPSILON) {
            // roll = 0; //@@ assume roll=0
            pitch_ = glm::degrees(atan2f(mat[0][1], mat[1][1]));
        } else {
            // roll = glm::radians(atan2f(-mat[1][0], mat[0][0]));
            pitch_ = glm::degrees(atan2f(-mat[2][1], mat[2][2]));
        }
        yaw_ = -yaw_;
    };

    void handle_mouse_button(const float &xpos, const float &ypos) {
        last_x_ = static_cast<float>(xpos);
        last_y_ = static_cast<float>(ypos);

        last_world_ = get_world_pos(xpos, ypos);
    }

    void handle_mouse_rotate(const float &xpos, const float &ypos) {
        float xoffset = last_x_ - xpos;
        float yoffset = ypos - last_y_;

        last_x_ = xpos;
        last_y_ = ypos;

        xoffset *= SENSITIVITY;
        yoffset *= SENSITIVITY;

        yaw_ += xoffset;
        pitch_ += yoffset;

        pitch_ = std::min(pitch_, 89.f);
        pitch_ = std::max(pitch_, -89.f);

        forward_.x = std::cos(glm::radians(pitch_)) * std::sin(glm::radians(yaw_));
        forward_.y = std::sin(glm::radians(pitch_));
        forward_.z = std::cos(glm::radians(pitch_)) * std::cos(glm::radians(yaw_));

        forward_ = glm::normalize(forward_);
        left_ = glm::normalize(glm::cross(forward_, glm::vec3(0, 1, 0)));
        up_ = glm::normalize(glm::cross(left_, forward_));

        pos_ = tar_ + distance_ * forward_;
    }

    void handle_mouse_transpose(const float &xpos, const float &ypos) {
        glm::vec3 world_pos = get_world_pos(xpos, ypos);

        tar_.x += SPEED * (last_world_ - world_pos).x;
        tar_.z += SPEED * (last_world_ - world_pos).z;

        pos_.x += SPEED * (last_world_ - world_pos).x;
        pos_.z += SPEED * (last_world_ - world_pos).z;

        last_world_ = world_pos;
    }

    void handle_mouse_scroll(const float &yoffset) {
        zoom_ -= yoffset;
        zoom_ = std::min(zoom_, 1000.f);
        zoom_ = std::max(zoom_, 1.f);
    }

    glm::mat4 get_model() const { return glm::mat4(1.f); };
    glm::mat4 get_view() const { return glm::lookAt(pos_, tar_ + forward_, up_); };
    glm::mat4 get_projection() const {
        return glm::perspective(glm::radians(zoom_), WIDTH / (float)HEIGHT, ZNEAR, ZFAR);
    };

  private:
    glm::vec3 get_world_pos(const float &xpos, const float &ypos) {
        GLint viewport[4] = {0, 0, WIDTH, HEIGHT};

        glm::vec4 ndc;
        ndc.x = 2 * (xpos - (float)viewport[0]) / (float)viewport[2] - 1.f;
        ndc.y = 2 * (HEIGHT - ypos - (float)viewport[1]) / (float)viewport[3] - 1.f;
        ndc.z = 1.f;
        ndc.w = 1.f;

        glm::mat4 inv_mvp = glm::inverse(get_projection() * get_view() * get_model());
        glm::vec4 pos = inv_mvp * ndc;
        pos /= pos.w;

        return glm::vec3(pos);
    }

  private:
    glm::vec3 pos_;
    glm::vec3 tar_;

    float zoom_;

    float distance_;

    glm::vec3 forward_;
    glm::vec3 left_;
    glm::vec3 up_;

    float last_x_;
    float last_y_;

    glm::vec3 last_world_;

    float pitch_;
    float yaw_;
};
#endif
