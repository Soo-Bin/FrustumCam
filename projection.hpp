#ifndef __PROJECTION_HPP__
#define __PROJECTION_HPP__

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

#include <vector>

#include <iostream>
using namespace std;

struct Object {
    int id;
    glm::vec3 pt;

    Object(int _id, glm::vec3 _pt) : id(_id), pt(_pt) { pt.z = -pt.z; };
};

namespace projection {

using namespace std;

inline void run(vector<Object> objects, glm::mat4 mvp, int w, int h) {
    GLint viewport[4] = {0, 0, w, h};

    for (const auto obj : objects) {
        glm::vec4 pos(obj.pt, 1.f);
        // cout << "projection: " << glm::to_string(pos) << endl;
        glm::vec4 ndc = mvp * pos;
        ndc /= ndc.w;

        GLdouble winX, winY, winZ; // 2D point
        winX = viewport[2] * (ndc.x + 1) / 2 + viewport[0];
        winY = (ndc.z + 1) / 2;
        winZ = viewport[3] * (ndc.y + 1) / 2 + viewport[1];

        // cout << "projection x: " << winX << ", y: " << winY << ", z: " << winZ << endl;
    }
};

inline vector<GLfloat> introjection(glm::vec3 px, glm::mat4 mvp, int w, int h, float far,
                                    float near) {
    GLint viewport[4] = {0, 0, w, h};

    float px_x = px.x;
    float px_y = px.y;
    float px_z = px.z;

#if 0
    float a = 2.f * far * near / (near - far);
    float b = (far + near) / (near - far);

    GLfloat px_depth, zFar, zNear;
    // zFar = far;
    // zNear = near;
    zFar = 0.5 * a * (1.0 - ((b - 1.0) / (b + 1.0)));
    zNear = zFar * (b + 1.0) / (b - 1.0);
    glReadPixels((int)px_x, (int)px_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &px_depth);
    cout << "1depth: " << px_depth << endl;

    px_depth = (2.f * px_depth) - 1.f;
    px_depth = (2.f * zNear * zFar) / (zFar + zNear - (px_depth * (zFar - zNear)));
    cout << "2depth: " << px_depth << endl;
    px_y = px_depth;
#elif 0
    GLfloat depthrange[2];
    glGetFloatv(GL_DEPTH_RANGE, depthrange);
    GLfloat range_zNear = depthrange[0];
    GLfloat range_zFar = depthrange[1];

    GLfloat px_depth;
    glReadPixels((int)px_x, (int)px_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &px_depth);

    cout << "range: " << range_zNear << ", far: " << range_zFar << ", depth: " << px_depth << endl;

    float range =
        fabs(fabs((far * near * (range_zFar - range_zNear)) / (far - near)) /
             (px_depth - (((far + near) * (range_zFar - range_zNear)) / (2.f * (far - near))) -
              ((range_zFar + range_zNear) / 2.f)));
    int x, y, bits;
    glGetIntegerv(GL_DEPTH_BITS, &bits);
    cout << "numbits: " << bits << ", depth: " << px_depth << " (range=" << range
         << " units), FarZ: " << range_zFar << ", NearZ: " << range_zNear << ", far: " << far
         << ", near: " << near << endl;
#else

#endif
    // double px_x = 1503.08;
    // double px_y = 0.995709;
    // double px_z = 1105.21;

    glm::vec4 ndc;
    ndc.x = 2 * (px_x - (float)viewport[0]) / (float)viewport[2] - 1.f;
    ndc.y = 2 * (px_z - (float)viewport[1]) / (float)viewport[3] - 1.f;
    ndc.z = 2 * px_y - 1.f;
    ndc.w = 1.f;

    glm::mat4 inv_mvp = glm::inverse(mvp);
    glm::vec4 pos = inv_mvp * ndc;
    pos /= pos.w;

    // cout << "introjection: " << glm::to_string(pos) << endl;
    // cout << "---" << endl;

    return vector<GLfloat>{pos.x, pos.y, pos.z};
};
} // namespace projection
#endif