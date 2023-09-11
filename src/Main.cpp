#include "Canvas.h"
#include "Misc.h"
#include "A3DBModel.h"
#include <iostream>

int main(){
    Canvas Canvas("", 650, 650);

    auto cube = A3DBModel::load("Cube.a3db");

    if (cube == nullptr)
    {
        std::cout << "failed to load model!" << std::endl;
        return -1;
    }    

    while (should_keep_rendering())
    {
        Canvas.clear();
        
        Canvas.draw_simple_model_naive_2(*cube, {-1.5, 0, 7});

        //Canvas.draw_line_2d({50,100},{-50,-100}, Color::red);

        Canvas.present();
    }

    return 0;
}