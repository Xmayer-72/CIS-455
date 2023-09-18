#pragma once

#include <memory>
#include <strings.h>
#include <vector>
#include <fstream>

#include "Vec.h"
#include "Color.h"
#include "Model.h"

// ReSharper disable once CppInconsistentNaming
class A3DBModel
{
public:
    static std::unique_ptr<Model> load( const std::string& file_name )
    {
        std::vec3f<vec3f> vertices        ;
        std::vec3f<vec3i> indices         ;
        std::vec3f<Color> triangle_colors ;

        std::ifstream in_file( file_name ) ;

        if( !in_file.good() )
            return nullptr ;

        while( !in_file.eof() )
        {
            std::string object ;
            in_file >> object ;

            if( ! in_file.eof() && ! in_file.good() )
                return nullptr ;

            if( ! strcmp( object.c_str(), "vertex" ) )
            {
                float x, y, z ;
                in_file >> x >> y >> z ;

                if( !in_file.good() )
                    return nullptr ;

                vertices.push_back( { x, y, z } ) ;
            }

            if( ! strcmp( object.c_str(), "triangle" ) )
            {
                int x, y, z, r, g, b ;
                in_file >> x >> y >> z >> r >> g >> b ;

                if( !in_file.good() )
                    return nullptr ;

                indices.push_back( { x, y, z } ) ;

                triangle_colors.push_back( Color::custom(
                    static_cast<uint8_t>( r ),
                    static_cast<uint8_t>( g ),
                    static_cast<uint8_t>( b ) ) ) ;

            }
        }

        std::vec3f<Triangle> triangles ;
        for( size_t i = 0 ; i < indices.size() ; ++i )
            triangles.push_back( { indices[ i ], triangle_colors[ i ] } ) ;

        return std::make_unique<Model>( Model { vertices, triangles } ) ;
    }
} ;
