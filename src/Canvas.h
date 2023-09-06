#pragma once

#include "CanvasBase.h"
#include <vector>

class Canvas final : public CanvasBase
{
    struct CorrespondingCoordinateLists{
        std::vector<float> left;
        std::vector<float> right;
    };

public:
    Canvas (const char* window_title, size_t width, size_t height) 
        : CanvasBase(window_title, height, width){
    }
    
    void draw_line_2d(Vec2i pt1, Vec2i pt2, const Color& color) const {
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

    void draw_triangle_2d_outline(Vec2i pt1, Vec2i pt2, Vec2i pt3, const Color& color) const {
        draw_line_2d(pt1,pt2,color);
        draw_line_2d(pt3,pt2,color);
        draw_line_2d(pt3,pt1,color);
    }

    void draw_triangle_2d(Vec2i pt1, Vec2i pt2, Vec2i pt3, const Color& color) const {
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

private:
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

    static CorrespondingCoordinateLists interpolate_between_edges(int y0, float v0, int y1, float v1, int y2, float v2){
        auto x01 = interpolate(y0, v0, y1, v1);
        auto x12 = interpolate(y1, v1, y2, v2);
        auto x02 = interpolate(y0, v0, y2, v2);

        x01.pop_back();
        x01.insert(x01.end(), x12.begin(), x12.end());
        // auto x012 = x01;
        // x012.pop_back();
        // x012.insert(x012.end(), x12.begin(), x12.end());

        auto m = x02.size() / 2;
        auto left = x01;//
        auto right = x02;

        if (left[m] > right[m])
        {
            std::swap(left, right);
        }

        return {left, right};
    }
};
