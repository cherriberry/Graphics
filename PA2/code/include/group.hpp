#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        objects.resize(num_objects, nullptr);

    }

    ~Group() override {

    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool isIntersected = false;
        for (Object3D *obj : objects) {
            if (obj != nullptr) {
                isIntersected |= obj->intersect(r, h, tmin);
            }
        }
        return isIntersected;
    }

    void addObject(int index, Object3D *obj) {
        if (index < 0) {
            std::cerr << "Error: Index out of bounds in addObject" << std::endl;
            return;
        }
        if (index >= objects.size()) {
            objects.resize(index + 1, nullptr);
        }
        objects[index] = obj;
    }

    void drawGL() override {
        Object3D::drawGL();
        for (Object3D *obj : objects) {
            if (obj != nullptr) {
                obj->drawGL();
            }
        }
    }

    int getGroupSize() {
        return static_cast<int>(objects.size());
    }

private:
    std::vector <Object3D*> objects;
};

#endif
	
