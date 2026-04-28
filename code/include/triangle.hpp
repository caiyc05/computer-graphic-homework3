#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>

using namespace std;

// TODO (PA2): Copy from PA1
class Triangle: public Object3D
{

public:
	Triangle() = delete;
        ///@param a b c are three vertex positions of the triangle

	Triangle( const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) : Object3D(m) {
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		Vector3f ba = a-b;
		Vector3f ca = a-c;
		normal = Vector3f::cross(ba,ca).normalized();
	}

	bool intersect(const Ray& ray, Hit& hit, float tmin) override {
		const Vector3f& A = vertices[0];
		const Vector3f& B = vertices[1];
		const Vector3f& C = vertices[2];

		const Vector3f& origin = ray.getOrigin();
		const Vector3f& dir = ray.getDirection();

		// 平面求交 
		float dot_n_d = Vector3f::dot(normal,dir);
		// 射线与平面平行，不相交:小于一个极小的数就认为平行
		if (fabs(dot_n_d )< 1e-8) return false;

		//重心法判断是否在三角形内部
		//判断是否在三角形中
		// R0:origin
		// Rd:direction
		// P0:A
		// P1:B
		// P2:C
		Vector3f E1 = A - B;
		Vector3f E2 = A - C;
		Vector3f S = A - origin; 
		
		Matrix3f matrix1 = Matrix3f(dir,E1,E2);
		Matrix3f matrix2 = Matrix3f(S,E1,E2);
		Matrix3f matrix3 = Matrix3f(dir,S,E2);
		Matrix3f matrix4 = Matrix3f(dir,E1,S);

		float determinant1 = matrix1.determinant();
		float determinant2 = matrix2.determinant();
		float determinant3 = matrix3.determinant();
		float determinant4 = matrix4.determinant();

		if(fabs(determinant1) < 1e-8){
			return false;
		}
		float t = determinant2 / determinant1;
		float alpha = determinant3 / determinant1;
		float beta = determinant4 / determinant1;

		//需要满足 t>0,beta,alpha,beta,alpha在0-1之间
		if(t <= 0.0f) return false;
		if(alpha < 0.0f || alpha > 1.0f) return false;
		if(beta < 0.0f || beta > 1.0f) return false;
		if(beta + alpha > 1.0f) return false;

		if(t<tmin || t > hit.getT()) return false;
		// 更新碰撞信息
		hit.set(t, material, normal);

		return true;
	}
	Vector3f normal;
	Vector3f vertices[3];

    void drawGL() override {
        Object3D::drawGL();
        glBegin(GL_TRIANGLES);
        glNormal3fv(normal);
        glVertex3fv(vertices[0]); glVertex3fv(vertices[1]); glVertex3fv(vertices[2]);
        glEnd();
    }

protected:
};

#endif //TRIANGLE_H
