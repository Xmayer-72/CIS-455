#pragma once

#include <SDL2/SDL.h>
#include "Vec.h"
#include "Plane.h"

inline bool should_keep_rendering()
{
    SDL_Event event;

    SDL_PollEvent( &event);
    return event.type != SDL_QUIT;
}

inline float compute_dot_product( const vec3f& v1, const vec3f& v2 )
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z ;
}

inline float compute_dot_product( const vec3f& v1, const vec4f& v2 )
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z ;
}

inline vec3f compute_intersection( const vec3f& v1, const vec3f& v2, const Plane& p )
{
    auto t_num = -p.distance - compute_dot_product( p.normal, v1 ) ;
    auto t_den = compute_dot_product( p.normal, v2 - v1 ) ;
    auto t = t_num / t_den ;
    return v1 + t * ( v2 - v1 ) ;
}

constexpr float pi = 3.14159265359;

constexpr float square_root_of_two = 1.41421356237;