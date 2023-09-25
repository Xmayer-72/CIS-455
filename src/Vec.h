#pragma once

template<typename T>

class vec2
{
public:
    T x;
    T y;
};

using vec2f = vec2<float>;
using vec2i = vec2<int>;

template<typename T>

class vec3
{
public:
    T x;
    T y;
    T z;

    friend vec3<T> operator+( const vec3<T>& a, const vec3<T>& b){
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    friend vec3<T> operator*( const vec3<T>& a, const vec3<T>& b){
        return {a.x * b.x, a.y * b.y, a.z * b.z};
    }

    friend vec3<T> operator*( T a, const vec3<T>& b){
        return {a * b.x, a * b.y, a * b.z};
    }

    friend vec3<T> operator-( const vec3<T>& a){
        return{-a.x, -a.y, -a.z};
    }

    friend vec3<T> operator-( const vec3<T>& a, const vec3<T>& b){
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }
};

using vec3f = vec3<float>;
using vec3i = vec3<int>;

template<typename T>

class vec4
{
public:
    T x;
    T y;
    T z;
    T w;
};

using vec4f = vec4<float>;
using vec4i = vec4<int>;