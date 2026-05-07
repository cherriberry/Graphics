#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>



class Plane : public Object3D {
public:
 Plane() {

    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this->norm = normal;
        this->d = d;
        this->material = m;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f ray_dir = r.getDirection();
        Vector3f ray_orig = r.getOrigin();
        float denom = Vector3f::dot(this->norm, ray_dir);
        if (std::fabs(denom) < 1e-6f)
            return false;
        
        float t = (d - Vector3f::dot(ray_orig, this->norm)) / denom;
        if( t < tmin || t > h.getT() || t <= 0 )
            return false;
        
        h.set(t, material, this->norm);
        return true;
    }

    void drawGL() override {
        Object3D::drawGL();
        Vector3f n = norm;
        Vector3f xAxis = Vector3f::RIGHT;
        Vector3f yAxis = Vector3f::cross(n, xAxis);
        xAxis = Vector3f::cross(yAxis, n);
        const float planeSize = 10.0;
        glBegin(GL_TRIANGLES);
        glNormal3fv(n);
        glVertex3fv(d * n + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * n - planeSize * xAxis - planeSize * yAxis);
        glVertex3fv(d * n + planeSize * xAxis - planeSize * yAxis);
        glNormal3fv(n);
        glVertex3fv(d * n + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * n - planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * n - planeSize * xAxis - planeSize * yAxis);
        glEnd();
    }

protected:
    Vector3f norm;
    float d;
    Material *material;

};

#endif //PLANE_H
		

