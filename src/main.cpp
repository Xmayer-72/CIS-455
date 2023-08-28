#include <iostream>
#include <SDL2/SDL.h>
#include <stdexcept>

int main(){
    std::cout << "Hello World" << std::endl;

    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        throw std::runtime_error(
            std::string("Could not INIT SDL2: ")
            + SDL_GetError()
        );
    }
    SDL_Quit();
    return 0;
}