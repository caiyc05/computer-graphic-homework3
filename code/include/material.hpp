#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <glut.h>

// TODO (PA2): Copy from PA1.
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
        // 
        Vector3f N = hit.getNormal().normalized();
        //L:从相交处 P 指向第 i 个光源的单位向量
        Vector3f L = dirToLight.normalized();
        //V = -dRw,且也是单位向量
        Vector3f V = -1*ray.getDirection().normalized(); 
        Vector3f R = 2*Vector3f::dot(N,L)*N - L;
        //确保R为方向向量
        R = R.normalized();
        //漫反射
        float diffusefactor = std::max(0.0f,Vector3f::dot(L,N));
        //镜面反射
        float specularfactor = std::max(0.0f,Vector3f::dot(V,R));
        //光泽度s次幂
        specularfactor = pow(specularfactor,shininess);

        Vector3f diffuse = lightColor * diffuseColor * diffusefactor;
        Vector3f specular = lightColor * specularColor * specularfactor;

        shaded += diffuse + specular;
        return shaded;
    }

    // For OpenGL, this is fully implemented
    void Use() {
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Vector4f(diffuseColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Vector4f(specularColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, Vector2f(shininess * 4.0, 1.0f));
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
};


#endif // MATERIAL_H
