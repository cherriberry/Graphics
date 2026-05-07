#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <glut.h>


class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = Vector3f::ZERO;
        radius = 1.0f;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material) {
        // 
        this->center = center;
        this->radius = radius;
        this->material = material;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f dir = r.getDirection();
        float dir_len = dir.length();
        if (dir_len < 1e-8f) {
            return false;
        }
        Vector3f dir_unit = dir / dir_len;

        Vector3f l = center - r.getOrigin();
        float t_poly = Vector3f::dot(l, dir_unit);
        bool is_origin_outside = Vector3f::dot(l, l) > radius * radius;
        if (t_poly < 0 && is_origin_outside)
            return false;
        
        float center_to_ray_dist = sqrt(Vector3f::dot(l, l) - t_poly * t_poly);
        if (center_to_ray_dist > radius)
            return false;
        
        float t_to_sphere = sqrt(radius * radius - center_to_ray_dist * center_to_ray_dist);
        if (is_origin_outside) {
            float t = (t_poly - t_to_sphere) / dir_len;
            if (t < tmin || t > h.getT())
                return false;
            h.set(t, material, (r.pointAtParameter(t) - center).normalized());
        } else {
            float t = (t_poly + t_to_sphere) / dir_len;
            if (t < tmin || t > h.getT())
                return false;
            h.set(t, material, (r.pointAtParameter(t) - center).normalized());
        }
        return true;
    }

    void drawGL() override {
        Object3D::drawGL();
        glMatrixMode(GL_MODELVIEW); glPushMatrix();
        glTranslatef(center.x(), center.y(), center.z());
        glutSolidSphere(radius, 80, 80);
        glPopMatrix();
    }

protected:
    Vector3f center;
    float radius;
    Material *material;

};


#endif
