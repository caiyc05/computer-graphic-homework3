#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// TODO (PA2): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint {
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D {
protected:
    std::vector<Vector3f> controls;
public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override {
        return false;
    }

    std::vector<Vector3f> &getControls() {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint>& data) = 0;

    void drawGL() override {
        Object3D::drawGL();
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glDisable(GL_LIGHTING);
        glColor3f(1, 1, 0);
        glBegin(GL_LINE_STRIP);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        glPointSize(4);
        glBegin(GL_POINTS);
        for (auto & control : controls) { glVertex3fv(control); }
        glEnd();
        std::vector<CurvePoint> sampledPoints;
        discretize(30, sampledPoints);
        glColor3f(1, 1, 1);
        glBegin(GL_LINE_STRIP);
        for (auto & cp : sampledPoints) { glVertex3fv(cp.V); }
        glEnd();
        glPopAttrib();
    }
};

class BezierCurve : public Curve {
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4 || points.size() % 3 != 1) {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        int segmentNum = (controls.size() - 1)/3;
        for(int i=0;i<segmentNum;i++){

            //三次bezier：4个控制点
            const Vector3f& p0 = controls[3*i];
            const Vector3f& p1 = controls[3*i+1];
            const Vector3f& p2 = controls[3*i+2];
            const Vector3f& p3 = controls[3*i+3];

            //进行resolution次采样
            for(int j=0;j<resolution;j++){
                //不放认为每次采样的t均匀分布
                float t = static_cast<float>(j) / resolution;
                
                //计算点
                float b0 = std::pow(1-t,3);
                float b1 = float(3)*std::pow(1-t,2)*t;
                float b2 = float(3)*(1-t)*std::pow(t,2);
                float b3 = std::pow(t,3);

                Vector3f V = b0*p0+b1*p1+b2*p2+b3*p3;

                //计算倒数
                float b0_derive = float(-3)*std::pow(1-t,2);
                float b1_derive = float(3)*(std::pow(1-t,2)-float(2)*(1-t)*t);
                float b2_derive = float(3)*(float(2)*(1-t)*t - std::pow(t,2));
                float b3_derive = float(3)*std::pow(t,2);

                Vector3f T = p0*b0_derive + p1*b1_derive + p2*b2_derive + p3*b3_derive;

                data.push_back({V,T.normalized()});
            }

        }

    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }


    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        // TODO (PA2): fill in data vector
        //使用3次B样条k=3
        int k = 3;
        int n = controls.size()-1;
        //先得到B样条的结点
        vector<float>t_list;
        for(int i=0;i<=n+k+1;i++){
            t_list.push_back(static_cast<float>(i)/(n+k+1));
        }

        //合理的计算范围[t_k,t_{n+1}]
        //对每一个区间[t_i,t_{i+1}]进行采样
        for(int i=k;i<=n;i++){

            //获得这个区间中涉及的b样条基函数B,k+1个
            //计算B_{i-k,k}(t),.....B_{i,k}(t)即可
            float start = t_list[i];
            float end = t_list[i+1];
            for(int j=0;j<resolution;j++){
                float t = start + (end-start) / resolution*j;

                float* b_basis = new float[n+k+1];
                //初始化B{i,0}
                for(int u=0;u<=n+k;u++){
                    b_basis[u]=(t>=t_list[u] && t<t_list[u+1])?1.0f:0.0f;
                }
                float* b_derive = new float[n+k+1];

                //获取基函数以及对应的导数
                for(int p = 1;p <= k;p++){
                    float* curr = new float[n+k+1-p];
                    float* derive = new float[n+k+1-p];
                    for(int u=0;u<=n+k-p;u++){
                        //递推公式分为左右两项

                        //compute B_{u,p}
                        float left=0;
                        float right=0;
                        if(t_list[u+p]!=t_list[u]){
                            left = (t-t_list[u])/(t_list[u+p]-t_list[u])*b_basis[u];
                        }
                        if(t_list[u+p+1]!=t_list[u+1]){
                            right = (t_list[u+p+1]-t)/(t_list[u+p+1]-t_list[u+1])*b_basis[u+1];
                        }
                        curr[u] = left+right;

                        //compute_b_derive
                        float left2 =0;
                        float right2 = 0;
                        if(t_list[u+p]!=t_list[u]){
                            left2= p*b_basis[u]/(t_list[u+p]-t_list[u]);
                        }
                        if(t_list[u+p+1]!=t_list[u+1]){
                            right2 = p*b_basis[u+1]/(t_list[u+p+1]-t_list[u+1]);
                        }
                        derive[u] = left2 - right2;
                    }
                    //计算完这一阶的基函数之后更新b_basis
                    delete[]b_basis;
                    b_basis = curr;
                    delete[]b_derive;
                    b_derive = derive;
                }

                //运行完上面的循环就已经完成里寻找

                //计算结果
                Vector3f ft(0,0,0);
                Vector3f ft_derive(0,0,0);
                for(int i=0;i<=n;i++){
                    ft += controls[i]*b_basis[i];
                    ft_derive += controls[i]*b_derive[i];
                }
                
                data.push_back({ft,ft_derive.normalized()});

            }
        }

    }

protected:

};

#endif // CURVE_HPP
