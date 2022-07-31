#include "sdl_wrappers.hpp"

#include <algorithm>
#include <iostream>
#include <vector>

#include "algorithm.hpp"
#include <SDL2/SDL.h>

void printError(const char* context)
{
    std::cout << "Error in " << context << ": " << SDL_GetError() << std::endl;
}

bool noQuitMessage()
{
    auto e = SDL_Event();
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            return false;
        if (e.type == SDL_KEYDOWN)
            if (e.key.keysym.sym == SDLK_ESCAPE)
                return false;
    }
    return true;
}

Sdl::Sdl(const char* window_title, int width, int height)
    : pixels(width * height, 0)
    , width(width)
    , height(height)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printError("SDL_Init");
        throw;
    }

    window = SDL_CreateWindow(
        window_title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (!window)
    {
        printError("SDL_CreateWindow");
        SDL_Quit();
        throw;
    }

    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer)
    {
        printError("SDL_CreateRenderer");
        SDL_DestroyWindow(window);
        SDL_Quit();
        throw;
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height);

    // TODO: handle texture failing.

    SDL_GetRelativeMouseState(nullptr, nullptr);
}

Sdl::~Sdl()
{
    // TODO: clean up texture.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Sdl::clear()
{
    using namespace std;
    fill(pixels, 0);
}

int Sdl::pitch() const
{
    return width * sizeof(pixels.front());
}

void Sdl::update()
{
    SDL_UpdateTexture(texture, nullptr, pixels.data(), pitch());
    SDL_RenderClear(renderer); // is this needed?
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void Sdl::setPixels(const Uint32* pixels_begin)
{
	std::copy(pixels_begin, pixels_begin + pixels.size(), pixels.begin());
}
