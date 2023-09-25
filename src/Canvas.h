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
            _depth_buffer = std::vector<float>(_width * _height, 0.0f);
    }

    void clear() override
    {
        CanvasBase::clear();

        _depth_buffer = std::vector<float>(_width * _height, 0.0f);
    }

    void set_camera_pos(const vec3f& position){
        _camera_pos = position;
        compute_camera_transform();
    }

    void set_camera_orient(const Mat& orientation){
        _camera_orient = orientation;
        compute_camera_transform();
    }

    void draw_simple_model(const ModelInstance& instance) {
        auto overall_transform = _camera_transform * instance.get_transformation() ;

        auto clipped_model = clip_model( instance, overall_transform ) ;

        if( clipped_model == nullptr )
            return ;

        std::vector<vec2i> projected_verticies( clipped_model->verticies.size() ) ;
        for( size_t i = 0 ; i < clipped_model->verticies.size() ; ++i )
            projected_verticies[ i ] = project_vertex( clipped_model->verticies[ i ] ) ;

        for( auto& triangle : clipped_model->triangles )
        {
            draw_triangle_2d(
                projected_verticies[ triangle.vertex_indexes.x ],
                projected_verticies[ triangle.vertex_indexes.y ],
                projected_verticies[ triangle.vertex_indexes.z ],
                clipped_model->verticies[triangle.vertex_indexes.x].z,
                clipped_model->verticies[triangle.vertex_indexes.y].z,
                clipped_model->verticies[triangle.vertex_indexes.z].z,
                triangle.color ) ;
        }
    }

private:
    vec3f               _camera_pos         ;
    Mat                 _camera_orient      ;
    Mat                 _camera_transform   ;
    std::vector<float>  _depth_buffer{}     ;

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
        auto transformed_center = transform * instance.model.bounding_sphere.center;
        auto transformed_radius = instance.get_scale() * instance.model.bounding_sphere.radius;

        // Discard instance if it is entirely outside of the viewing frustrum
        for (auto& clipping_plane : clipping_planes)
        {
            auto distance = compute_dot_product(clipping_plane.normal, transformed_center)
                + clipping_plane.distance;

            if (distance < -transformed_radius)
            {
                return nullptr;
            }
        }

        //----------------------------------------------------------------------------------------
        // Phase 2: Clip individual triangls in the model
        //----------------------------------------------------------------------------------------

        // Transform verticies
        std::vector<vec3f> verticies(instance.model.verticies.size());

        for (size_t i = 0; i < instance.model.verticies.size(); ++i)
        {
            auto tv = transform * instance.model.verticies[i];
            verticies[i] = {tv.x, tv.y, tv.z};
        }        

        // Clip each of the triangles (with transformed verticies) against each successive plane

        // Step 1.) Copy model triangles to vectors we will call "unclipped"
        std::vector<Triangle> unclipped_triangles{instance.model.triangles};

        // Step 2.) Go through each of the clipping planes
        for(auto& clipping_plane : clipping_planes){
            // Step 3.) Create empty vectors to hold the triangles after they are clipped
            std::vector<Triangle> clipped_trianges;

            // Step 4.) Go through each of the triangles (for the current clipping plane)
            for(auto& unclipped_triangle : unclipped_triangles){
                // Step 5.) Add the clipped triangles to the clipped triangle vectors
                clip_triangle(clipping_plane, unclipped_triangle, verticies, clipped_trianges);
            }

            // Step 6.) The vectors now have triangles clipped relative to the current clipping plane.
            //          Copy them to the unclipped vectors because they have not yet been clipped relative
            //            to the next clipping plane.
            unclipped_triangles = std::vector<Triangle>{clipped_trianges};
        }

        // Step 7.) There was not a next clipping plane, so the triangles that are in the "unclipped" vectors
        //            are actually fully clipped.  So, pass back a new model made up of the clipped triangles.
        return std::make_unique<Model>(Model{verticies, unclipped_triangles});
    }

    void clip_triangle( const Plane& plane, const Triangle& triangle,
        std::vector<vec3f>& verticies, std::vector<Triangle>& triangles ) const
    {
        auto dist_from_plane_v1 = compute_dot_product( plane.normal,
                                    verticies[ triangle.vertex_indexes.x ] ) + plane.distance ;
        auto dist_from_plane_v2 = compute_dot_product( plane.normal,
                                    verticies[ triangle.vertex_indexes.y ] ) + plane.distance ;
        auto dist_from_plane_v3 = compute_dot_product( plane.normal,
                                    verticies[ triangle.vertex_indexes.z ] ) + plane.distance ;

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
            // Set B and C to the other two verticies
            auto a = verticies[ triangle.vertex_indexes.x ] ;
            auto b = verticies[ triangle.vertex_indexes.y ] ;
            auto c = verticies[ triangle.vertex_indexes.z ] ;
            auto idx_a = triangle.vertex_indexes.x ;
            if( dist_from_plane_v2 > 0 )
            {
                a = verticies[ triangle.vertex_indexes.y ] ;
                b = verticies[ triangle.vertex_indexes.z ] ;
                c = verticies[ triangle.vertex_indexes.x ] ;
                idx_a = triangle.vertex_indexes.y ;
            }
            else if( dist_from_plane_v3 > 0 )
            {
                a = verticies[ triangle.vertex_indexes.z ] ;
                b = verticies[ triangle.vertex_indexes.x ] ;
                c = verticies[ triangle.vertex_indexes.y ] ;
                idx_a = triangle.vertex_indexes.z ;
            }

            // Create new verticies where AB and AC intersect the clipping plane
            auto new_b = compute_intersection( a, b, plane ) ;
            auto new_c = compute_intersection( a, c, plane ) ;

            // Add the new verticies to the verticies list and get their indexes
            verticies.push_back( new_b ) ;
            verticies.push_back( new_c ) ;
            auto idx_b = static_cast<int>( verticies.size() ) - 2 ;
            auto idx_c = static_cast<int>( verticies.size() ) - 1 ;

            // Add the new triangle made up of A, the new B, and the new C (and its color)
            triangles.push_back( { { idx_a, idx_b, idx_c }, triangle.color } ) ;
        }
        else if( count_in_plane == 2 )
        {   // The triangle has two verticies in. Add two clipped triangles.

            // Set C to the vertex outside the frustrum
            // Set A and B to the other two verticies and idx_a and idx_b to their indices
            auto a = verticies[ triangle.vertex_indexes.x ] ;
            auto b = verticies[ triangle.vertex_indexes.y ] ;
            auto c = verticies[ triangle.vertex_indexes.z ] ;
            auto idx_a = triangle.vertex_indexes.x ;
            auto idx_b = triangle.vertex_indexes.y ;
            if( dist_from_plane_v1 <= 0 )
            {
                a = verticies[ triangle.vertex_indexes.y ] ;
                b = verticies[ triangle.vertex_indexes.z ] ;
                c = verticies[ triangle.vertex_indexes.x ] ;
                idx_a = triangle.vertex_indexes.y ;
                idx_b = triangle.vertex_indexes.z ;
            }
            else if( dist_from_plane_v2 <= 0 )
            {
                a = verticies[ triangle.vertex_indexes.z ] ;
                b = verticies[ triangle.vertex_indexes.x ] ;
                c = verticies[ triangle.vertex_indexes.y ] ;
                idx_a = triangle.vertex_indexes.z ;
                idx_b = triangle.vertex_indexes.x ;
            }

            // Create new verticies where AC and BC intersect the clipping plane
            auto new_a = compute_intersection( a, c, plane ) ;
            auto new_b = compute_intersection( b, c, plane ) ;

            // Add the new verticies to the verticies list and get their indexes
            verticies.push_back( new_a ) ;
            verticies.push_back( new_b ) ;
            auto idx_new_a = static_cast<int>( verticies.size() ) - 2 ;
            auto idx_new_b = static_cast<int>( verticies.size() ) - 1 ;

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

    void draw_triangle_2d(vec2i pt1, vec2i pt2, vec2i pt3, float pt1_z, float pt2_z, float pt3_z, const Color& color) {
        //Sort points by height
        if (pt2.y < pt1.y)
        {
            std::swap(pt2, pt1);
            std::swap(pt2_z, pt1_z);
        }

        if (pt3.y < pt1.y)
        {
            std::swap(pt3, pt1);
            std::swap(pt3_z, pt1_z);
        }

        if (pt3.y < pt2.y)
        {
            std::swap(pt2, pt3);
            std::swap(pt2_z, pt3_z);
        }
        
        //Create interpolations
        auto x_coords_per_y = interpolate_between_edges(
            pt1.y, static_cast<float>(pt1.x),
            pt2.y, static_cast<float>(pt2.x),
            pt3.y, static_cast<float>(pt3.x)
        );

        // Interpolate inverse Z Coord
        auto inv_z_coords_per_y = interpolate_between_edges(
            pt1.y, 1.0f/pt1_z,
            pt2.y, 1.0f/pt2_z,
            pt3.y, 1.0f/pt3_z
        );

        //working through lists
        for (auto y = pt1.y; y <= pt3.y; ++y)
        {
            auto idx_into_lists = y - pt1.y;

            auto x_start = static_cast<int>(x_coords_per_y.left[idx_into_lists]);
            auto x_end = static_cast<int>(x_coords_per_y.right[idx_into_lists]);

            auto z_left = inv_z_coords_per_y.left[idx_into_lists];
            auto z_right = inv_z_coords_per_y.right[idx_into_lists];
            
            auto zscan = interpolate(
                x_start, z_left, x_end, z_right
            );

            for(int x = x_start; x <= x_end; ++x){
                if(check_and_update_depth_buffer(x, y, zscan[x - x_start])){
                    put_pixel({x,y}, color);
                }
            }
        }
    }

    bool check_and_update_depth_buffer(int x, int y, float inverse_z) {
        auto w = static_cast<int>(_width);
        auto h = static_cast<int>(_height);

        // convert x & y to list of values from normalized origin
        x = w / 2 +x;
        y = h / 2 -y;

        if(x < 0 || x >= w || y < 0 || y>= h){
            return false;
        }

        auto offset = w * y + x;

        if(_depth_buffer[offset] < inverse_z){
            _depth_buffer[offset] = inverse_z;
            return true;
        }

        return false;
    }

    void draw_line_3d(const vec3f pt1, const vec3f pt2, const Color& color){
        auto pv1 = project_vertex(pt1);
        auto pv2 = project_vertex(pt2);
        draw_line_2d(pv1, pv2, color);
    }
    
    void draw_triangle_3d(const vec3f& v1, const vec3f& v2, const vec3f& v3, Color color) const{
        auto pv1 = project_vertex(v1);
        auto pv2 = project_vertex(v2);
        auto pv3 = project_vertex(v3);

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
    
    vec2i project_vertex(const vec3f& v) const{
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
