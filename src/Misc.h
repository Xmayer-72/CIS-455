#pragma once

#include <SDL2/SDL.h>

inline bool should_keep_rendering()
{
    SDL_Event event;

    SDL_PollEvent( &event);
    return event.type != SDL_QUIT;
}