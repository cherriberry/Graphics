#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera {
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH) {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up).normalized();
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// TODO: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera {
private:
    float angle_y, angle_x; // field of view in radian
    float fx, fy; // how much one pixel represents.
    //
public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
            const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH) {
        // angle is in radian.
        this->angle_y = angle;
        this->angle_x = angle * imgW / imgH;
    }

    Ray generateRay(const Vector2f &point) override {
        // 
        Vector3f ray_dir;
        fx = tan(angle_x / 2) * 2 / width;
        fy = tan(angle_y / 2) * 2 / height;
        ray_dir = direction + horizontal * (point.x() - width / 2) * fx + up * (point.y() - height / 2) * fy;
        return Ray(center, ray_dir.normalized());
    }
};

#endif //CAMERA_H
