#pragma once

template<typename T>

class Vec2
{
public:
    T x;
    T y;
};

using Vec2i = Vec2<int>;

template<typename T>

class Vec3
{
public:
    T x;
    T y;
    T z;
};

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;