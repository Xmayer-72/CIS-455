#include <iostream>
#include <SDL2/SDL.h>
#include <stdexcept>

int main(){
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        throw std::runtime_error(
            std::string("Could not INIT SDL2: ")
            + SDL_GetError()
        );
    }
    SDL_Quit();
    return 0;
}