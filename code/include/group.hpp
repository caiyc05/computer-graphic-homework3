#ifndef GROUP_H
#define GROUP_H


#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <vector>


// TODO: Implement Group - add data structure to store a list of Object*
class Group : public Object3D {

public:

    Group() {

    }

    explicit Group (int num_objects) {
        objects.reserve(num_objects);
    }

    ~Group() override {
        for(Object3D* object:objects){
            delete object;
        }
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        bool intersect = false;
        for(auto& object:objects){
            if(object->intersect(r,h,tmin)){
                intersect = true;
            }
        }
        return intersect;
    }

    void drawGL() override {
        for (auto *object : objects) {
            if (object != nullptr) {
                object->drawGL();
            }
        }
    }

    void addObject(int index, Object3D *obj) {
        if(index >= 0 && index<objects.size()){
            objects.insert(objects.begin()+index,obj);
        }
        else{
            objects.push_back(obj);
        }
    }

    int getGroupSize() {
        return objects.size();
    }   

private:
    std::vector<Object3D*> objects;

};

#endif
	
