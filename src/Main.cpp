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

    ModelInstance cube2{*cube, {1.25, 2.5, 7.5}, 1, 195, {0, 1, 0}};

    ModelInstance cube1{*cube, {-1.5, 0, 7}, 0.75};

    Canvas.set_camera_pos({-3, 1, 2});
    Canvas.set_camera_orient(Mat::get_rotation_matrix(30, {0, 1, 0}));

    while (should_keep_rendering())
    {
        Canvas.clear();
        
        Canvas.draw_simple_model(cube2);

        Canvas.draw_simple_model(cube1);

        Canvas.present();
    }

    return 0;
}