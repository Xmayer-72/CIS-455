#include "Canvas.h"
#include "Misc.h"

int main(){
    Canvas Canvas("", 650, 650);
    while (should_keep_rendering())
    {
        Canvas.clear();
        
        //Canvas.draw_triangle_2d_outline({300,200},{0,150},{-300,-250}, Color::blue_violet);
        Canvas.draw_triangle_2d({0, 0}, {-100,180}, {300,-250}, Color::blue);

        Canvas.present();
    }

    return 0;
}