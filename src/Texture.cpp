#include "Texture.h"

#include <SDL3_image/SDL_image.h>

#include <iostream>
#include <cstdint>
#include <assert.h>

Texture::Texture() : mTexture{ nullptr }, mWidth{ 0 }, mHeight{ 0 }
{

}

Texture::~Texture()
{
    destroy();
}

SDL_Surface* SDL_CreateRGBSurfaceFrom(void* pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    return SDL_CreateSurfaceFrom(width, height,
        SDL_GetPixelFormatForMasks(depth, Rmask, Gmask, Bmask, Amask),
        pixels, pitch);
}

bool Texture::loadFromFile(SDL_Renderer* Renderer, std::string path)
{
    destroy();

    if (SDL_Surface* loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr)
    {
        SDL_Log("Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());
    }
    else
    {
        if (SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0x00, 0xFF, 0xFF)) == false)
        {
            SDL_Log("Unable to color key! SDL error: %s", SDL_GetError());
        }
        else
        {
            if (mTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface); mTexture == nullptr)
            {
                SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
            }
            else
            {
                mWidth = loadedSurface->w;
                mHeight = loadedSurface->h;
            }
        }

        SDL_DestroySurface(loadedSurface);
    }

    return mTexture != nullptr;
}

void Texture::destroy()
{
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void Texture::setColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(mTexture, r, g, b);
}

void Texture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void Texture::setBlending(SDL_BlendMode blendMode)
{
    SDL_SetTextureBlendMode(mTexture, blendMode);
}

void Texture::renderAsBackground(SDL_Renderer* Renderer, float xOnScreen, float yOnScreen)
{
    SDL_FRect rect{ 0.f, 0.f, xOnScreen, yOnScreen };

    SDL_RenderTexture(Renderer, mTexture, &rect, NULL);
}

void Texture::render(SDL_Renderer* Renderer, const SDL_FRect srcrect, const SDL_FRect dstrect, double angle)
{
    SDL_RenderTextureRotated(Renderer, mTexture, &srcrect, &dstrect, angle, NULL, SDL_FLIP_NONE);
}

int Texture::getWidth()
{
    return mWidth;
}

int Texture::getHeight()
{
    return mHeight;
}

bool Texture::isLoaded()
{
    return mTexture != nullptr;
}