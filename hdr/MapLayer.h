#pragma once

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>
#include <tmxlite/detail/Log.hpp>

#include "SDL3/SDL.h"

class Texture;

class MapLayer
{
    public:
        MapLayer(tmx::Map& map, std::size_t idx, std::vector<Texture*> textures);
        ~MapLayer() { }

        void update(SDL_FRect view);
        void draw(SDL_Renderer* Renderer);
    private:
        SDL_Texture* texture = nullptr;
        std::vector<SDL_Vertex> fullMapVertexData;
        std::vector<SDL_Vertex> visibleMapVertexData;

        void flipY(SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5);
        void flipX(SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5);
        void flipD(SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5);
        void doFlips(std::uint8_t bits, SDL_FPoint* v0, SDL_FPoint* v1, SDL_FPoint* v2, SDL_FPoint* v3, SDL_FPoint* v4, SDL_FPoint* v5);
};