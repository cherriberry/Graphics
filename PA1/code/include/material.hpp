#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.
class Material {
public:

    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) :
            diffuseColor(d_color), specularColor(s_color), shininess(s) {

    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const {
        return diffuseColor;
    }


    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor) {
        Vector3f shaded = Vector3f::ZERO;

        Vector3f N = hit.getNormal().normalized();
        Vector3f L = dirToLight.normalized();

        float dot_n_l = Vector3f::dot(N, L);
        float diffuse = std::max(0.0f, dot_n_l);
        shaded += diffuseColor * diffuse * lightColor; 

        float specular = 0.0f;
        if (dot_n_l > 1e-6f) {
            Vector3f R = (2.0f * dot_n_l * N - L).normalized();
            Vector3f d_r_w = -ray.getDirection().normalized();
            specular = std::max(0.0f, Vector3f::dot(d_r_w, R));
        }
        shaded += specularColor * std::pow(specular, shininess) * lightColor;
        return shaded;
    }

protected:
    Vector3f diffuseColor;//kd
    Vector3f specularColor;//ks
    float shininess;//s
};


#endif // MATERIAL_H
