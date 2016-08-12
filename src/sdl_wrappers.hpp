#pragma once

#include <iostream>
#include <vector>

#include "SDL.h"

void printError(const char* context);
bool noQuitMessage();

struct Sdl
{
    Sdl(const char* window_title, int width, int height);
    ~Sdl();

    void clear();
    void update();
    int pitch() const;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::vector<Uint32> pixels;
    int width;
    int height;
};
