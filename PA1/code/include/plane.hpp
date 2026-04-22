#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D {
public:
    Plane() {

    }

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
        this->normal = normal;
        this->length = d;
        this->material = m;
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f ray_dir = r.getDirection();
        Vector3f ray_orig = r.getOrigin();
        float denom = Vector3f::dot(this->normal, ray_dir);
        if (std::fabs(denom) < 1e-6f)
            return false;
        
        float t = (length - Vector3f::dot(ray_orig, this->normal)) / denom;
        if( t < tmin || t > h.getT() || t <= 0 )
            return false;
        
        h.set(t, material, this->normal);
        return true;
    }

protected:
    Vector3f normal;
    float length;
    Material *material;


};

#endif //PLANE_H
		

