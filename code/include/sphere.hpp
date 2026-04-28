#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <glut.h>

// TODO (PA2): Copy from PA1

class Sphere : public Object3D {
public:
    Sphere() {
        // unit ball at the center
        center = Vector3f(0.0,0.0,0.0);
        radius = 1.0;
        material = nullptr;
    }

    Sphere(const Vector3f &center, float radius, Material *material) : 
    Object3D(material),center(center),radius(radius) {
        // 
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        // 球心到射线起点
        Vector3f oc = center - r.getOrigin();    
        // 获取方向向量      
        Vector3f dir = r.getDirection();   
        
        // 计算球心到射线上的投影长度
        float projectionLength = Vector3f::dot(oc, dir) / dir.squaredLength();
        
        // 计算球心到射线的垂直距离
        Vector3f projectPoint = r.pointAtParameter(projectionLength);
        float distanceSquared = (projectPoint - center).squaredLength();
        
        // 垂直距离大于半径，不相交
        if (distanceSquared > radius * radius) {
            return false;
        }
        
        // 计算交点距离
        float halfChord = sqrt(radius * radius - distanceSquared);
        
        // 分类讨论：
        float t;
        if(oc.length() > radius){
            //origin 在球外。此时projectionLength一定是正的
            t = projectionLength - halfChord;
        }
        else if(oc.length() < radius){
            //origin在球内
            t = projectionLength + halfChord;
        }
        else{
            //说明origin正好在球面上
            t = 0;
        }
        //判断合理性
        if(tmin > t){
            return false;
        }
        //不是离相机最近的
        if(t >= h.getT()){
            return false;
        }
        
        // 计算法线
        Vector3f intersectionPoint = r.getOrigin() + t*dir;
        Vector3f normal = (intersectionPoint - center).normalized();  // 归一化
        
        
        h.set(t, material, normal);
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

};


#endif
