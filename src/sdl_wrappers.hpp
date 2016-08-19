#pragma once

#include <iostream>
#include <vector>

#include "SDL.h"

void printError(const char* context);
bool noQuitMessage();

class Sdl
{
public:
    Sdl(const char* window_title, int width, int height);
    ~Sdl();
    void clear();
    void update();
	//void setPixels()
    std::vector<Uint32> pixels;
private:
	int pitch() const;
	int width;
	int height;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
};
