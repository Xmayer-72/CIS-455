#pragma once

#include "CanvasBase.h"
#include "Model.h"
#include "Mat.h"

#include <vector>
#include <iostream>

class Canvas final : public CanvasBase
{
    struct correspondingCoordinateLists{
        std::vector<float> left;
        std::vector<float> right;
    };

    static constexpr float viewport_size = 1;
    static constexpr float projection_z = 1;

public:
    Canvas (const char* window_title, size_t width, size_t height) 
        : CanvasBase(window_title, height, width){
    }

    void draw_simple_model(const Model& model,
                           const Mat& transformation = Mat::get_identity_matrix()) const{

        for(auto& triangle : model.triangles){
            std::vector<vec2i> projected_vertexies(model.verticies.size());

            for(int i; i < model.verticies.size(); ++i){
                projected_vertexies[i] = project_vortex(transformation * model.verticies[i]);
            }

            for (auto& triangle : model.triangles){
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
};
