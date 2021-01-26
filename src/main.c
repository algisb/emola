#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include "GBdevice.h"
#include "defs.h"
#include "debug.h"

int main(int argc, char ** argv)
{
    dbg_initLogging();
    
    SDL_Window* window = NULL;
    SDL_Surface* screenSurface = NULL;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
        return 1;
    }
    window = SDL_CreateWindow(
                    "emola",
                    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                    GB_SCREEN_WIDTH, GB_SCREEN_HEIGHT,
                    SDL_WINDOW_SHOWN
                    );
    if (window == NULL) {
        fprintf(stderr, "could not create window: %s\n", SDL_GetError());
        return 1;
    }
    screenSurface = SDL_GetWindowSurface(window);
    
    GBDevice * device = NULL;
    createGBDevice(&device);
    
    uint32_t t0 = 0;
    uint32_t t1 = 0;
    while(device->on)
    {
        t0 = SDL_GetTicks();
        
        
        run(device);
        SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
        SDL_UpdateWindowSurface(window);
        
        t1 = SDL_GetTicks() - t0;
        if(t1 < GB_TARGET_FRAMETIME_MS)
        {
            SDL_Delay(GB_TARGET_FRAMETIME_MS - t1);
        }
    }
    
    
    destroyGBDevice(&device);
    SDL_DestroyWindow(window);
    SDL_Quit();
    dbg_deinitLogging();

	return 0;
}

