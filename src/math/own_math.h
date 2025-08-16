


#ifndef MATH_H
#define MATH_H
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
struct Vector3 {
    float x, y, z;
};
float dotProduct(const Vector3 a, const Vector3 b) ;




Vector3 normalize(const Vector3& v);


 Vector3 cross(const Vector3& a, const Vector3& b);
 float dot(const Vector3& a, const Vector3& b);
float vlen(const Vector3 v);
Vector3 vsub(Vector3 a, Vector3 b);
 float deg2rad(float d);
 Vector3 vnorm(const Vector3 v);
 float vdot(const Vector3&a,const Vector3&b);
 Vector3 vcross(const Vector3&a,const Vector3&b);

 void build_basis(const Vector3 forward_in, const Vector3 toP, Vector3 fwd, Vector3 right, Vector3 up);

void build_basis_auto(const Vector3& forward_in, const Vector3& toP, Vector3& F, Vector3& R, Vector3& U);


#endif // MATH_H
