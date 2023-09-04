#include "Canvas.h"
#include "Misc.h"

int main(){
    Canvas Canvas("", 650, 650);
    while (should_keep_rendering())
    {
        Canvas.clear();
        
        Canvas.draw_triangle_outline({300,200},{0,200},{-300,-250}, Color::blue_violet);
        Canvas.draw_triangle_2d({299,199},{1,199},{-299,-249}, Color::blue);

        Canvas.present();
    }

    return 0;
}