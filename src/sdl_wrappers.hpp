#pragma once

#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

void printError(const char* context);
bool noQuitMessage();

class Sdl
{
public:
    Sdl(const char* window_title, int width, int height);
    ~Sdl();
    void clear();
    void update();
	void setPixels(const Uint32* pixels_begin);
private:
	int pitch() const;
	int width;
	int height;
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* texture;
	std::vector<Uint32> pixels;
};
