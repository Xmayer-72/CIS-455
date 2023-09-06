#include "Canvas.h"
#include "Misc.h"

int main(){
    Canvas Canvas("", 650, 650);

    vec3f cube_ftl = {-2, -0.5, 5};
    vec3f cube_ftr = {-2, 0.5, 5};
    vec3f cube_fbl = {-1, -0.5, 5};
    vec3f cube_fbr = {-1, 0.5, 5};
    vec3f cube_btl = {-2, -0.5, 6};
    vec3f cube_btr = {-2, 0.5, 6};
    vec3f cube_bbl = {-1, -0.5, 6};
    vec3f cube_bbr = {-1, 0.5, 6};

    while (should_keep_rendering())
    {
        Canvas.clear();
        
        Canvas.draw_line_3d(cube_ftl, cube_ftr, Color::blue);
        Canvas.draw_line_3d(cube_ftl, cube_fbl, Color::blue);
        Canvas.draw_line_3d(cube_fbr, cube_ftr, Color::blue);
        Canvas.draw_line_3d(cube_fbr, cube_fbl, Color::blue);
        
        Canvas.draw_line_3d(cube_btl, cube_btr, Color::red);
        Canvas.draw_line_3d(cube_btl, cube_bbl, Color::red);
        Canvas.draw_line_3d(cube_bbr, cube_btr, Color::red);
        Canvas.draw_line_3d(cube_bbr, cube_bbl, Color::red);
        
        Canvas.draw_line_3d(cube_ftl, cube_btl, Color::lime);
        Canvas.draw_line_3d(cube_ftr, cube_btr, Color::lime);
        Canvas.draw_line_3d(cube_fbl, cube_bbl, Color::lime);
        Canvas.draw_line_3d(cube_fbr, cube_bbr, Color::lime);

        Canvas.present();
    }

    return 0;
}