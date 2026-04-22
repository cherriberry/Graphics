#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include "plane.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// TODO: implement this class and add more fields as necessary,
class Triangle: public Object3D {

public:
	Vector3f normal; // 法向量
	Triangle() = delete;

    // a b c are three vertex positions of the triangle
	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		this->a = a;
		this->b = b;
		this->c = c;
		this->material = m;
		this->normal = Vector3f::cross(b - a, c - a).normalized();
		plane = Plane(normal, Vector3f::dot(normal, a), m);
	}

	bool intersect( const Ray& ray,  Hit& hit , float tmin) override {
		Hit planeHit(hit);
		bool is_intersected = plane.intersect(ray, planeHit, tmin);
		if (!is_intersected)
			return false;
		Vector3f hit_point = ray.pointAtParameter(planeHit.getT());
		//检查点是否在三角形内：用矩阵乘法求解重心坐标参数
		Vector3f R0 = ray.getOrigin(), Rd = ray.getDirection();
		Vector3f E0 = a - b, E1 = a - c, S = a - R0;
		Matrix3f A(S, E0, E1,  false);
		Matrix3f B(Rd, S, E1, false);
		Matrix3f C(Rd, E0, S, false);
		Matrix3f O(Rd, E0, E1, false);
		float denom = O.determinant();
		if (std::fabs(denom) < 1e-6f)
			return false;
		float t = A.determinant() / denom;
		float beta = B.determinant() / denom;
		float gamma = C.determinant() / denom;
		if (beta < -1e-6f || gamma < -1e-6f || beta + gamma > 1.0f + 1e-6f || t < tmin || t > planeHit.getT() + 1e-6f)
			return false;
		hit.set(t, material, normal);
		return true;
	}

protected:
	Vector3f a, b, c;// 顶点坐标
	Material* material;
	Plane plane; // 三角形所在的平面
};

#endif //TRIANGLE_H
