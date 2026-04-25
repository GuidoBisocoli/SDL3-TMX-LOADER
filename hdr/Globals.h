#pragma once

#include <SDL3/SDL.h>

// constants
SDL_Window* Window{ nullptr };
SDL_Renderer* Renderer{ nullptr };
const int screenWidth = 1280;
const int screenHeight = 800;
const double timeStep = 1.0 / 60.0;

// time & fps
Uint64 freq = SDL_GetPerformanceFrequency();
Uint64 last = SDL_GetPerformanceCounter();
Uint64 now = 0;
double deltaTime = 0.0;
double elapsed = 0.0;