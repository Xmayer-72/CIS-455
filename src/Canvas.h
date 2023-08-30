#pragma once

#include "CanvasBase.h"
#include <vector>

class Canvas final : public CanvasBase
{
public:
    Canvas (const char* window_title, size_t width, size_t height) 
        : CanvasBase(window_title, height, width){
    }
    
    void draw_line_2d(Vec2i pt1, Vec2i pt2, const Color& color){
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
};
