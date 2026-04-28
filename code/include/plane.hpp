#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// TODO (PA2): Copy from PA1

class Plane : public Object3D {
public:
    Plane() {
        norm = Vector3f(0,0,1);
        d = 0;
        material = nullptr;
    }

    Plane(const Vector3f &normal, float d, Material *m) :
    Object3D(m),norm(normal),d(d) {

    }

    ~Plane() override = default;

     bool intersect(const Ray &r, Hit &h, float tmin) override {
        Vector3f dir = r.getDirection();
        Vector3f origin = r.getOrigin();
        
        if(fabs(Vector3f::dot(dir,norm)) < 1e-6){
            return false;
        }

        float t = (d - Vector3f::dot(norm,origin)) / Vector3f::dot(dir,norm);

        if(tmin > t || t < 0){
            return false;
        }

        if(h.getT() <= t){
            return false;
        }
        
        h.set(t, material, norm);
        
        return true;
    }

    void drawGL() override {
        Object3D::drawGL();
        Vector3f xAxis = Vector3f::RIGHT;
        Vector3f yAxis = Vector3f::cross(norm, xAxis);
        xAxis = Vector3f::cross(yAxis, norm);
        const float planeSize = 10.0;
        glBegin(GL_TRIANGLES);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glVertex3fv(d * norm + planeSize * xAxis - planeSize * yAxis);
        glNormal3fv(norm);
        glVertex3fv(d * norm + planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis + planeSize * yAxis);
        glVertex3fv(d * norm - planeSize * xAxis - planeSize * yAxis);
        glEnd();
    }

protected:
    Vector3f norm;
    float d;

};

#endif //PLANE_H
		

