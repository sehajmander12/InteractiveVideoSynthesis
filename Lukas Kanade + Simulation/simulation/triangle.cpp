#include "triangle.h"


#include <Eigen/Core>
#include "math.h"
 
Triangle::Triangle(FeaturePoint* t1, FeaturePoint* t2, FeaturePoint* t3) : t1(t1), t2(t2), t3(t3) {}

bool Triangle::inBoundingBox(float x, float y) {
    getBoundingBox();
    return x <= maxX && x >= minX && y <= maxY && y >= minY;
}

vector<float> Triangle::barycentric(float x, float y) {
    vector<float> coords(3, 0.f);
    Eigen::Vector2f v0(t2->x - t1->x, t2->y - t1->y);
    Eigen::Vector2f v1(t3->x - t1->x, t3->y - t1->y);

    Eigen::Vector2f vp(x - t1->x, y - t1->y);

    float d00 = v0.squaredNorm();
    float d01 = v0.dot(v1);
    float d11 = v1.squaredNorm();
    float d20 = vp.dot(v0);
    float d21 = vp.dot(v1);

    float denom = d00 * d11 - d01 * d01;

    coords[1] = (d11 * d20 - d01 * d21) / denom;
    coords[2] = (d00 * d21 - d01 * d20) / denom;
    coords[0] = 1.f - coords[1] - coords[2];

    return coords;
}

bool Triangle::contains(vector<float> barycentric) {
    float sum = 0.f;
    assert(barycentric.size() == 3);
    for (unsigned int i = 0; i < barycentric.size(); ++i) {
        if (barycentric[i] < 0.f || barycentric[i] > 1.f) {
            return false;
        }
        sum += barycentric[i];
    }

    return abs(sum - 1.f) < 0.001f;
}

bool Triangle::contains(float x, float y) {
    return contains(barycentric(x, y));
}

bool Triangle::hasVertex(FeaturePoint fp) {
    if (t1 && (t1->id == fp.id)) return true;
    if (t2 && (t2->id == fp.id)) return true;
    if (t3 && (t3->id == fp.id)) return true;
    return false;
}

bool Triangle::isFixed() {
    return t1->getFixed() && t2->getFixed() && t3->getFixed();
}

void Triangle::getBoundingBox() {
    maxX = max(t1->x, max(t2->x, t3->x));
    maxY = max(t1->y, max(t2->y, t3->y));
    minX = min(t1->x, min(t2->x, t3->x));
    minY = min(t1->y, min(t2->y, t3->y));
}

std::ostream & operator<<(std::ostream & out, Triangle & v) {
    out << *v.t1 << endl;
    out << *v.t2 << endl;
    out << *v.t3 << endl;
    return out;
}
