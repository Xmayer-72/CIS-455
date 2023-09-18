#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include "Mat.h"
#include "Plane.h"
#include "CanvasBase.h"
#include "ModelInstance.h"

class Canvas final : public CanvasBase
{
    struct correspondingCoordinateLists{
        std::vector<float> left;
        std::vector<float> right;
    };

    static constexpr float viewport_size = 1;
    static constexpr float projection_z = 1;
    static const     Plane clipping_planes[ 5 ];

public:
    Canvas (const char* window_title, size_t width, size_t height) 
        : CanvasBase(window_title, height, width),
        _camera_pos({0, 0, 0}),
        _camera_orient(Mat::get_identity_matrix()),
        _camera_transform(Mat::get_identity_matrix()){
    }

    void set_camera_pos(const vec3f& position){
        _camera_pos = position;
        compute_camera_transform();
    }

    void set_camera_orient(const Mat& orientation){
        _camera_orient = orientation;
        compute_camera_transform();
    }

    void draw_simple_model(const ModelInstance& instance) const{
        auto overall_transform = _camera_transform * instance.get_transformation();

        std::vector<vec2i> projected_vertexies(instance.model.verticies.size());
        
        for(auto& triangle : instance.model.triangles){

            for(int i; i < instance.model.verticies.size(); ++i){
                projected_vertexies[i] = project_vortex(overall_transform * instance.model.verticies[i]);
            }

            for (auto& triangle : instance.model.triangles){
                draw_triangle_2d_outline(
                    projected_vertexies[triangle.vertex_indexes.x],
                    projected_vertexies[triangle.vertex_indexes.y],
                    projected_vertexies[triangle.vertex_indexes.z],
                    triangle.color
                );
            }
        }
    }

private:
    vec3f _camera_pos;
    Mat _camera_orient;
    Mat _camera_transform;



    void compose_camera_transform()
    {
        _camera_transform = _camera_orient.transpose()
            * Mat::get_translation_matrix( -_camera_pos ) ;
    }

    std::unique_ptr<Model> clip_model( const ModelInstance& instance, const Mat& transform ) const
    {
        //----------------------------------------------------------------------------------------
        // Phase 1: Reject the model if it is clipped entirely
        //----------------------------------------------------------------------------------------

        // Get the transformed center and radius of the model's bounding sphere
        // TODO: that ^

        // Discard instance if it is entirely outside of the viewing frustrum
        // TODO: that ^


        //----------------------------------------------------------------------------------------
        // Phase 2: Clip individual triangls in the model
        //----------------------------------------------------------------------------------------

        // Transform vertices
        // TODO: that ^

        // Clip each of the triangles (with transformed vertices) against each successive plane

        // Step 1.) Copy model triangles to vectors we will call "unclipped"
        // TODO: that ^

        // Step 2.) Go through each of the clipping planes
        // TODO: that ^
            // Step 3.) Create empty vectors to hold the triangles after they are clipped
            // TODO: that ^

            // Step 4.) Go through each of the triangles (for the current clipping plane)
            // TODO: that ^
                // Step 5.) Add the clipped triangles to the clipped triangle vectors
                // TODO: that ^

            // Step 6.) The vectors now have triangles clipped relative to the current clipping plane.
            //          Copy them to the unclipped vectors because they have not yet been clipped relative
            //            to the next clipping plane.
            // TODO: that ^

        // Step 7.) There was not a next clipping plane, so the triangles that are in the "unclipped" vectors
        //            are actually fully clipped.  So, pass back a new model made up of the clipped triangles.
        // TODO: that ^
    }

    void clip_triangle( const Plane& plane, const Triangle& triangle,
        std::vector<vec3f>& vertices, std::vector<Triangle>& triangles ) const
    {
        auto dist_from_plane_v1 = compute_dot_product( plane.normal,
                                    vertices[ triangle.vertex_indexes.x ] ) + plane.distance ;
        auto dist_from_plane_v2 = compute_dot_product( plane.normal,
                                    vertices[ triangle.vertex_indexes.y ] ) + plane.distance ;
        auto dist_from_plane_v3 = compute_dot_product( plane.normal,
                                    vertices[ triangle.vertex_indexes.z ] ) + plane.distance ;

        auto count_in_plane = ( dist_from_plane_v1 > 0 ? 1 : 0 )
            + ( dist_from_plane_v2 > 0 ? 1 : 0 )
            + ( dist_from_plane_v3 > 0 ? 1 : 0 ) ;

        // The triangle is fully in front of the plane.
        if( count_in_plane == 3 )
        {
            triangles.push_back( triangle ) ;
        }
        else if( count_in_plane == 1 )
        {   // The triangle has one vertex in. Add one clipped triangle.

            // Set A to the vertex inside the frustrum and idx_a to it's index
            // Set B and C to the other two vertices
            auto a = vertices[ triangle.vertex_indexes.x ] ;
            auto b = vertices[ triangle.vertex_indexes.y ] ;
            auto c = vertices[ triangle.vertex_indexes.z ] ;
            auto idx_a = triangle.vertex_indexes.x ;
            if( dist_from_plane_v2 > 0 )
            {
                a = vertices[ triangle.vertex_indexes.y ] ;
                b = vertices[ triangle.vertex_indexes.z ] ;
                c = vertices[ triangle.vertex_indexes.x ] ;
                idx_a = triangle.vertex_indexes.y ;
            }
            else if( dist_from_plane_v3 > 0 )
            {
                a = vertices[ triangle.vertex_indexes.z ] ;
                b = vertices[ triangle.vertex_indexes.x ] ;
                c = vertices[ triangle.vertex_indexes.y ] ;
                idx_a = triangle.vertex_indexes.z ;
            }

            // Create new vertices where AB and AC intersect the clipping plane
            auto new_b = compute_intersection( a, b, plane ) ;
            auto new_c = compute_intersection( a, c, plane ) ;

            // Add the new vertices to the vertices list and get their indexes
            vertices.push_back( new_b ) ;
            vertices.push_back( new_c ) ;
            auto idx_b = static_cast<int>( vertices.size() ) - 2 ;
            auto idx_c = static_cast<int>( vertices.size() ) - 1 ;

            // Add the new triangle made up of A, the new B, and the new C (and its color)
            triangles.push_back( { { idx_a, idx_b, idx_c }, triangle.color } ) ;
        }
        else if( count_in_plane == 2 )
        {   // The triangle has two vertices in. Add two clipped triangles.

            // Set C to the vertex outside the frustrum
            // Set A and B to the other two vertices and idx_a and idx_b to their indices
            auto a = vertices[ triangle.vertex_indexes.x ] ;
            auto b = vertices[ triangle.vertex_indexes.y ] ;
            auto c = vertices[ triangle.vertex_indexes.z ] ;
            auto idx_a = triangle.vertex_indexes.x ;
            auto idx_b = triangle.vertex_indexes.y ;
            if( dist_from_plane_v1 <= 0 )
            {
                a = vertices[ triangle.vertex_indexes.y ] ;
                b = vertices[ triangle.vertex_indexes.z ] ;
                c = vertices[ triangle.vertex_indexes.x ] ;
                idx_a = triangle.vertex_indexes.y ;
                idx_b = triangle.vertex_indexes.z ;
            }
            else if( dist_from_plane_v2 <= 0 )
            {
                a = vertices[ triangle.vertex_indexes.z ] ;
                b = vertices[ triangle.vertex_indexes.x ] ;
                c = vertices[ triangle.vertex_indexes.y ] ;
                idx_a = triangle.vertex_indexes.z ;
                idx_b = triangle.vertex_indexes.x ;
            }

            // Create new vertices where AC and BC intersect the clipping plane
            auto new_a = compute_intersection( a, c, plane ) ;
            auto new_b = compute_intersection( b, c, plane ) ;

            // Add the new vertices to the vertices list and get their indexes
            vertices.push_back( new_a ) ;
            vertices.push_back( new_b ) ;
            auto idx_new_a = static_cast<int>( vertices.size() ) - 2 ;
            auto idx_new_b = static_cast<int>( vertices.size() ) - 1 ;

            // Add the new triangle made up of A, B and the new A (and its color)
            triangles.push_back( { { idx_a, idx_b, idx_new_a }, triangle.color } ) ;

            // Add the new triangle made up of the new A, B and the new B (and its color)
            triangles.push_back( { { idx_new_a, idx_b, idx_new_b }, triangle.color } ) ;
        }
    }

    void draw_line_2d(vec2i pt1, vec2i pt2, const Color& color) const {
        auto dx = pt2.x - pt1.x;
        auto dy = pt2.y - pt1.y;

        if (std::abs(dx) > std::abs(dy)){
            if(dx<0){
                std::swap(pt1, pt2);
            }
            auto ys = interpolate(pt1.x, static_cast<float>(pt1.y), pt2.x, static_cast<float>(pt2.y));
            for (int x = pt1.x; x < pt2.x; ++x)
            {
                put_pixel({x, static_cast<int>(ys[x - pt1.x])}, color);
            }
            
        }
        else{
            if(dy<0){
                std::swap(pt1, pt2);
            }
            auto xs = interpolate(pt1.y, static_cast<float>(pt1.x), pt2.y, static_cast<float>(pt2.x));
            for (int y = pt1.y; y < pt2.y; ++y)
            {
                put_pixel({static_cast<int>(xs[y - pt1.y]), y}, color);
            }
        }
    }

    void draw_triangle_2d_outline(vec2i pt1, vec2i pt2, vec2i pt3, const Color& color) const {
        draw_line_2d(pt1,pt2,color);
        draw_line_2d(pt3,pt2,color);
        draw_line_2d(pt3,pt1,color);
    }

    void draw_triangle_2d(vec2i pt1, vec2i pt2, vec2i pt3, const Color& color) const {
        //Sort points by height
        if (pt2.y < pt1.y)
        {
            std::swap(pt2, pt1);
        }

        if (pt3.y < pt1.y)
        {
            std::swap(pt3, pt1);
        }

        if (pt3.y < pt2.y)
        {
            std::swap(pt2, pt3);
        }
        
        //Create interpolations
        auto x_coords_per_y = interpolate_between_edges(
            pt1.y, static_cast<float>(pt1.x),
            pt2.y, static_cast<float>(pt2.x),
            pt3.y, static_cast<float>(pt3.x)
        );

        //working through lists
        for (auto y = pt1.y; y <= pt3.y; ++y)
        {
            auto idx_into_lists = y - pt1.y;

            auto x_start = static_cast<int>(x_coords_per_y.left[idx_into_lists]);
            auto x_end = static_cast<int>(x_coords_per_y.right[idx_into_lists]);
            
            for(int x = x_start; x <= x_end; ++x){
                put_pixel({x,y}, color);
            }
        }
    }

    void draw_line_3d(const vec3f pt1, const vec3f pt2, const Color& color){
        auto pv1 = project_vortex(pt1);
        auto pv2 = project_vortex(pt2);
        draw_line_2d(pv1, pv2, color);
    }
    
    void draw_triangle_3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, Color color) const{
        auto pv1 = project_vortex(v1);
        auto pv2 = project_vortex(v2);
        auto pv3 = project_vortex(v3);

        draw_line_2d(pv1, pv2, color);
        draw_line_2d(pv2, pv3, color);
        draw_line_2d(pv1, pv3, color);
    }

    static std::vector<float> interpolate(int i0, float d0, int i1, float d1){
        if (i0 == i1)
        {
            return std::vector <float> {d0};
        }
        else{
            auto values = std::vector<float>();

            auto a = (d1-d0)/static_cast<float>(i1-i0);
            auto d = d0;

            for (int i = i0; i < i1; ++i)
            {
                values.push_back(d);
                d += a;
            }
            
            return values;
        }
    }

    static correspondingCoordinateLists interpolate_between_edges(int y0, float v0, int y1, float v1, int y2, float v2){
        auto x01 = interpolate(y0, v0, y1, v1);
        auto x12 = interpolate(y1, v1, y2, v2);
        auto x02 = interpolate(y0, v0, y2, v2);

        // x01.pop_back();
        // x01.insert(x01.end(), x12.begin(), x12.end());
        auto x012 = x01;
        x012.pop_back();
        x012.insert(x012.end(), x12.begin(), x12.end());

        auto m = x02.size() / 2;
        auto left = x012;//
        auto right = x02;

        if (left[m] > right[m])
        {
            std::swap(left, right);
        }

        return {left, right};
    }

    vec2i viewport_to_canvas(const vec2f& pt) const{
        return {
            static_cast<int>((pt.x * static_cast<float>(_width)) / viewport_size),
            static_cast<int>((pt.y * static_cast<float>(_height)) / viewport_size)};
    }
    
    vec2i project_vortex(const vec3f& v) const{
        return viewport_to_canvas({
            (v.x * projection_z) / v.z,
            (v.y * projection_z) / v.z});
    }
    
    vec2i project_vortex(const vec4f& v) const{
        return viewport_to_canvas({
            (v.x * projection_z) / v.z,
            (v.y * projection_z) / v.z});
    }

    void compute_camera_transform(){
        _camera_transform = _camera_orient.transpose()
                            * Mat::get_translation_matrix( -_camera_pos);
    }
};
