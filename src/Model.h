#pragma once

#include <cmath>
#include <vector>

#include "Vec.h"
#include "Sphere.h"
#include "Triangle.h"

class Model
{
private:
    Sphere compute_bounding_sphere() const
    {
        if( verticies.empty() )
            return { { 0, 0, 0 }, 0 } ;

        vec3f mins { verticies[ 0 ].x, verticies[ 0 ].y, verticies[ 0 ].z } ;
        vec3f maxs { verticies[ 0 ].x, verticies[ 0 ].y, verticies[ 0 ].z } ;

        for( size_t i = 1 ; i < verticies.size() ; ++i  )
        {
            if( verticies[ i ].x < mins.x ) mins.x = verticies[ i ].x ;
            if( verticies[ i ].y < mins.y ) mins.y = verticies[ i ].y ;
            if( verticies[ i ].z < mins.z ) mins.z = verticies[ i ].z ;
            if( verticies[ i ].x > maxs.x ) maxs.x = verticies[ i ].x ;
            if( verticies[ i ].y > maxs.y ) maxs.y = verticies[ i ].y ;
            if( verticies[ i ].z > maxs.z ) maxs.z = verticies[ i ].z ;
        }

        vec3f centroid { ( mins.x + maxs.x ) / 2,
                         ( mins.y + maxs.y ) / 2 ,
                         ( mins.z + maxs.z ) / 2 } ;

        float radius_squared = 0 ;

        vec3f corner3 { mins.x, mins.y, maxs.z } ;
        vec3f corner4 { mins.x, maxs.y, maxs.z } ;
        vec3f corner5 { mins.x, maxs.y, mins.z } ;
        vec3f corner6 { maxs.x, mins.y, mins.z } ;
        vec3f corner7 { maxs.x, mins.y, maxs.z } ;
        vec3f corner8 { maxs.x, maxs.y, mins.z } ;

        radius_squared = get_max_distance_squared( centroid,    mins, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid,    maxs, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner3, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner4, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner5, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner6, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner7, radius_squared ) ;
        radius_squared = get_max_distance_squared( centroid, corner8, radius_squared ) ;

        return { centroid, std::sqrt( radius_squared ) } ;
    }

    static float get_max_distance_squared(
        const vec3f& centroid, const vec3f& corner, float max_radius_squred_so_far )
    {
        auto dist_squared = ( corner.x - centroid.x ) * ( corner.x - centroid.x )
                          + ( corner.y - centroid.y ) * ( corner.y - centroid.y )
                          + ( corner.z - centroid.z ) * ( corner.z - centroid.z ) ;

        return std::max( dist_squared, max_radius_squred_so_far ) ;
    }

public:
    const std::vector<vec3f>    verticies;
    const std::vector<Triangle> triangles;
    const Sphere                bounding_sphere ;

    Model(std::vector<vec3f> verticies, std::vector<Triangle> triangles):
        verticies(std::move(verticies)),
        triangles(std::move(triangles)),
        bounding_sphere(compute_bounding_sphere()){}
};