#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>
#include <cmath>
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
        for(int i = 0; i < controls.size() - 1; i += 3) {
            CurvePoint P0, P1, P2, P3;   
            P0.V = controls[i];
            P1.V = controls[i + 1];
            P2.V = controls[i + 2];
            P3.V = controls[i + 3];             
            for(int j = 0; j <= resolution; j++) { //实际只计算三次Bezier曲线，所有B很好算
                float t = (float)j / resolution;
                float B_0_3 = std::pow(1 - t, 3) * std::pow(t, 0) * 1;
                float B_1_3 = std::pow(1 - t, 2) * std::pow(t, 1) * 3;
                float B_2_3 = std::pow(1 - t, 1) * std::pow(t, 2) * 3;
                float B_3_3 = std::pow(1 - t, 0) * std::pow(t, 3) * 1;
                CurvePoint cp;
                cp.V = P0.V * B_0_3 + P1.V * B_1_3 + P2.V * B_2_3 + P3.V * B_3_3;
                cp.T = 3*((P1.V - P0.V) * std::pow(1 - t, 2) + (P2.V - P1.V) * std::pow(1 - t, 1) * t * 2 + (P3.V - P2.V) * std::pow(t, 2));
                if(cp.T.length() > 1e-6) 
                    cp.T.normalize();
                data.push_back(cp);
            }
        }
    }

protected:

};

class BsplineCurve : public Curve {
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points) {
        if (points.size() < 4) {
            printf("Number of control points of BsplineCurve must be more than 4!\n");
            exit(0);
        }
    }


    void discretize(int resolution, std::vector<CurvePoint>& data) override {
        data.clear();
        int n = (int)controls.size() - 1; // n+1个控制点
        int k = 3; 
        if (n < k) return;
        int max_sum = n + k + 1;
        basisFunctions.resize(n + 1);

        for (int i = k; i <= n; i++) {
            float t_i = (float)i / max_sum;
            float t_i1 = (float)(i + 1) / max_sum;

            for (int j = 0; j <= resolution; j++) {
                float t = t_i + (t_i1 - t_i) * ((float)j / resolution);

                //B_r,0
                for (int r = 0; r <= n; r++) {
                    basisFunctions[r].resize(k + 1, 0.0);

                    float t_i = (float)r / max_sum;
                    float t_i1 = (float)(r + 1) / max_sum;
                    if ((t >= t_i && t < t_i1) || (r == n && t == t_i1)) 
                        basisFunctions[r][0] = 1.0;
                    else
                        basisFunctions[r][0] = 0.0;
                }

                //B_r,p
                for (int p = 1; p <= k; p++) {
                    //B_q,p
                    for (int q = 0; q <= n; q++) {
                        float t_i = (float)q / max_sum;
                        float t_ip = (float)(q + p) / max_sum;
                        float t_ip1 = (float)(q + p + 1) / max_sum;
                        float t_i1 = (float)(q + 1) / max_sum;
                        float left = 0.0;
                        float right = 0.0;
                        if ((t_ip - t_i) > 0.0) 
                            left = (t - t_i) / (t_ip - t_i) * basisFunctions[q][p - 1];
                        if ((t_ip1 - t_i1) > 0.0 && q + 1 <= n) 
                            right = (t_ip1 - t) / (t_ip1 - t_i1) * basisFunctions[q + 1][p - 1];
                        basisFunctions[q][p] = left + right;
                    }
                }

                Vector3f V(0, 0, 0);
                Vector3f dV(0, 0, 0);
                for (int q = 0; q <= n; q++) {
                    float B_ik = basisFunctions[q][k];
                    V += controls[q] * B_ik;
                    float t_i = (float)q / max_sum;
                    float t_ik = (float)(q + k) / max_sum;
                    float t_ik1 = (float)(q + k + 1) / max_sum;
                    float t_i1 = (float)(q + 1) / max_sum;
                    float dB = 0.0;
                    if ((t_ik - t_i) > 0.0) 
                        dB += k * (basisFunctions[q][k - 1] / (t_ik - t_i));
                    if ((t_ik1 - t_i1) > 0.0 && q + 1 <= n) 
                        dB -= k * (basisFunctions[q + 1][k - 1] / (t_ik1 - t_i1));
                    dV += controls[q] * dB;
                }

                CurvePoint cp;
                cp.V = V;
                if (dV.length() > 1e-6) cp.T = dV.normalized(); else cp.T = Vector3f(0, 0, 0);
                data.push_back(cp);
            }
        }



    }

protected:
    std::vector<std::vector<float>> basisFunctions; 

};

#endif // CURVE_HPP
