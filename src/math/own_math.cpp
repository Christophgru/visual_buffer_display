#include "own_math.h"
#include <cmath>


 float dotProduct(const Vector3 a, const Vector3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

 Vector3 normalize(const Vector3& v){
    float L = std::sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    return { v.x/L, v.y/L, v.z/L };
}

 float deg2rad(float d) {
    return d * M_PI / 180.0f;
}

Vector3 cross(const Vector3 &a, const Vector3 &b)
{
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

float dot(const Vector3 &a, const Vector3 &b)
{
    return vdot(a, b);
}
Vector3 vsub(Vector3 a, Vector3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }

float vlen(const Vector3 v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }
Vector3 vnorm(const Vector3 v)
{
    float L = vlen(v);
    return (L > 1e-8f) ? Vector3{v.x / L, v.y / L, v.z / L} : Vector3{0, 0, 0};
}

 float vdot(const Vector3&a,const Vector3&b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

 Vector3 vcross(const Vector3&a,const Vector3&b) {
    return { a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x };
}

 void build_basis(const Vector3 forward_in, const Vector3 toP, Vector3 fwd, Vector3 right, Vector3 up)
{
    Vector3 worldUp = {0.f, 0.f, 1.f};                 // change if your world-up differs
    fwd = vnorm(forward_in);
    if (vdot(fwd, worldUp) > 0.999f) worldUp = {0.f, 1.f, 0.f}; // avoid degeneracy

    // If the camera convention is actually "backward" (common), dot(toP,fwd) will be negative
    if (vdot(toP, fwd) < 0.0f) {
        fwd = { -fwd.x, -fwd.y, -fwd.z };         // auto-flip forward once
    }

    right = vnorm(vcross(fwd, worldUp));
    up    = vcross(right, fwd);                   // already normalized
}

void build_basis_auto(const Vector3& forward_in, const Vector3& toP, Vector3& F, Vector3& R, Vector3& U)
{
    Vector3 worldUp{0.f,0.f,1.f};
    F = vnorm(forward_in);
    if (std::fabs(vdot(F, worldUp)) > 0.999f) worldUp = {0.f,1.f,0.f};
    // If the camera uses -Z forward style, dot(toP,F) will be negative for points in front → flip once
    if (vdot(toP, F) < 0.0f) F = { -F.x, -F.y, -F.z };
    R = vnorm(vcross(F, worldUp));
    U = vcross(R, F); // already unit
}