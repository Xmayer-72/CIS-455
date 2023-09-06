#pragma once

template<typename T>

class Vec2
{
public:
    T x;
    T y;
};

using vec2f = Vec2<float>;
using vec2i = Vec2<int>;

template<typename T>

class Vec3
{
public:
    T x;
    T y;
    T z;

    friend Vec3<t> operator+( const Vec3<T>& a, const Vec3<t> b){
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }
};

using vec3f = Vec3<float>;
using vec3i = Vec3<int>;