#ifndef double3_H
#define double3_H

#include <math.h>

struct double3 {
  double x;
  double y;
  double z;
};


inline int max(int a, int b)
{
  return a > b ? a : b;
}

inline int min(int a, int b)
{
  return a < b ? a : b;
}

// additional constructors
inline double3 make_double3(double x, double y, double z) {
  double3 temp = {x, y, z};
  return temp;
}

inline double3 make_double3(double s) {
    return make_double3(s, s, s);
}

// negate
inline double3 operator-(double3 &a)
{
    return make_double3(-a.x, -a.y, -a.z);
}

// addition
inline double3 operator+(double3 a, double3 b)
{
    return make_double3(a.x + b.x, a.y + b.y, a.z + b.z);
}
inline double3 operator+(double3 a, double b)
{
    return make_double3(a.x + b, a.y + b, a.z + b);
}
inline void operator+=(double3 &a, double3 b)
{
    a.x += b.x; a.y += b.y; a.z += b.z;
}

// subtract
inline double3 operator-(double3 a, double3 b)
{
    return make_double3(a.x - b.x, a.y - b.y, a.z - b.z);
}
inline double3 operator-(double3 a, double b)
{
    return make_double3(a.x - b, a.y - b, a.z - b);
}
inline void operator-=(double3 &a, double3 b)
{
    a.x -= b.x; a.y -= b.y; a.z -= b.z;
}

// multiply
inline double3 operator*(double3 a, double3 b)
{
    return make_double3(a.x * b.x, a.y * b.y, a.z * b.z);
}
inline double3 operator*(double3 a, double s)
{
    return make_double3(a.x * s, a.y * s, a.z * s);
}
inline double3 operator*(double s, double3 a)
{
    return make_double3(a.x * s, a.y * s, a.z * s);
}
inline void operator*=(double3 &a, double s)
{
    a.x *= s; a.y *= s; a.z *= s;
}

// divide
inline double3 operator/(double3 a, double3 b)
{
    return make_double3(a.x / b.x, a.y / b.y, a.z / b.z);
}
inline double3 operator/(double3 a, double s)
{
    double inv = 1.0f / s;
    return a * inv;
}
inline double3 operator/(double s, double3 a)
{
    double inv = 1.0f / s;
    return a * inv;
}
inline void operator/=(double3 &a, double s)
{
    double inv = 1.0f / s;
    a *= inv;
}

// lerp
inline double3 lerp(double3 a, double3 b, double t)
{
    return a + t*(b-a);
}

inline double clamp(double x, double y, double z) {
  return fminf(fminf(x, y), z);
}

// clamp
inline double3 clamp(double3 v, double a, double b)
{
    return make_double3(clamp(v.x, a, b), clamp(v.y, a, b), clamp(v.z, a, b));
}

inline double3 clamp(double3 v, double3 a, double3 b)
{
    return make_double3(clamp(v.x, a.x, b.x), clamp(v.y, a.y, b.y), clamp(v.z, a.z, b.z));
}

// dot product
inline double dot(double3 a, double3 b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// cross product
inline double3 cross(double3 a, double3 b)
{
    return make_double3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

// length
inline double length(double3 v)
{
    return sqrtf(dot(v, v));
}

// normalize
inline double3 normalize(double3 v)
{
    double invLen = 1.0f / sqrtf(dot(v, v));
    return v * invLen;
}

// floor
inline double3 floor(const double3 v)
{
    return make_double3(floor(v.x), floor(v.y), floor(v.z));
}

// reflect
inline double3 reflect(double3 i, double3 n)
{
        return i - 2.0f * n * dot(n, i);
}
#endif
