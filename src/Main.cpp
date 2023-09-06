#include "Canvas.h"
#include "Misc.h"

int main(){
    Canvas Canvas("", 650, 650);
    while (should_keep_rendering())
    {
        Canvas.clear();
        
        //Canvas.draw_triangle_2d_outline({300,200},{0,150},{-300,-250}, Color::blue_violet);
        //Canvas.draw_triangle_2d({0, 0}, {-100,180}, {300,-250}, Color::blue);
        //roof
        Canvas.draw_triangle_2d({0,200},{-200,100},{200,100},Color::red);
        //wall
        Canvas.draw_triangle_2d({-150,100},{150,100},{150,-175},Color::green);
        Canvas.draw_triangle_2d({-150,100},{-150,-175},{150,-175},Color::green);
        //window 1
        Canvas.draw_triangle_2d({-125,75},{-25,75},{-25,0},Color::blue);
        Canvas.draw_triangle_2d({-125,75},{-125,0},{-25,0},Color::blue);
        //window 2
        Canvas.draw_triangle_2d({125,75},{15,75},{15,0},Color::blue);
        Canvas.draw_triangle_2d({125,75},{125,0},{15,0},Color::blue);
        //door
        Canvas.draw_triangle_2d({-40,-15},{40,-15},{40,-175}, Color::dark_violet);
        Canvas.draw_triangle_2d({-40,-15},{-40,-175},{40,-175}, Color::dark_violet);

        Canvas.present();
    }

    return 0;
}