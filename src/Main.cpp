#include "Canvas.h"
#include "Misc.h"

int main(){
    Canvas Canvas("", 600, 600);
    while (should_keep_rendering())
    {
        Canvas.clear();

        Canvas.draw_line_2d({-120, -240}, {120, 240}, Color::aquamarine);

        Canvas.draw_line_2d({240, 120},{-200,-100}, Color::red_violet);

        Canvas.draw_line_2d({-50,-200},{60,240}, Color::gold);

        Canvas.present();
    }

    return 0;
}