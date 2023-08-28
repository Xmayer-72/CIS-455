#include "CanvasBase.h"
#include "Misc.h"

int main(){
    CanvasBase Canvas("", 300, 300);
while (should_keep_rendering)
{
    Canvas.clear();

    Canvas.put_pixel({0,0},Color::red);

    Canvas.present();
}

    
    return 0;
}