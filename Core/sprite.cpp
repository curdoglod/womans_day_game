#include "sprite.h"
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include "Utils.h"

//Sprite::Sprite(SDL_Texture* m_texture, SDL_Renderer* renderer)
//    : m_renderer(renderer), m_texture(m_texture) {
// 
//
//    // Set the initial size and position of the sprite
//    m_destRect.x =0 ;
//    m_destRect.y =0 ;
//    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_destRect.w, &m_destRect.h);
//
//    //setSize(m_destRect.y, m_destRect.y);
//}

Sprite::Sprite(const std::vector<unsigned char>& imageData, SDL_Renderer* renderer): m_renderer(renderer), m_texture(nullptr){

 
    SDL_RWops* rw = SDL_RWFromConstMem(imageData.data(), imageData.size());
    if (!rw) {
        std::cerr << "SDL_RWFromConstMem failed: " << SDL_GetError() << std::endl;
        return;
    }

 
    SDL_Surface* surface = IMG_Load_RW(rw, 1); 
    if (!surface) {
        std::cerr << "IMG_Load_RW failed: " << SDL_GetError() << std::endl;
        return;
    }

    
    m_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); 

    if (!m_texture) {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        return;
    }

    m_destRect.x = 0;
    m_destRect.y = 0;
    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_destRect.w, &m_destRect.h);
}

Sprite::~Sprite() {
    if (m_texture) {
        SDL_DestroyTexture(m_texture);
    }
}

void Sprite::draw() {
    SDL_Point *pivot = m_useCustomPivot ? &m_pivot : nullptr;
    SDL_RenderCopyEx(m_renderer, m_texture, nullptr, &m_destRect, m_angle, pivot, m_flip);
}

void Sprite::draw(const Vector2& vec2) {
    m_destRect.x = (int)vec2.x;
    m_destRect.y = (int)vec2.y;
    SDL_Point *pivot = m_useCustomPivot ? &m_pivot : nullptr;
    SDL_RenderCopyEx(m_renderer, m_texture, nullptr, &m_destRect, m_angle, pivot, m_flip);
}

void Sprite::SetColorAndOpacity(Uint8 red, Uint8 green, Uint8 blue, float alpha) {

    SDL_SetTextureColorMod(m_texture, red, green, blue);
    SDL_SetTextureAlphaMod(m_texture, alpha*255); 
}

void Sprite::setPosition(int x, int y) {
    m_destRect.x = x;
    m_destRect.y = y;
}

void Sprite::setSize(int width, int height) {
    m_destRect.w = width;
    m_destRect.h = height;
}

Vector2  Sprite::getSize() {
    return Vector2(m_destRect.w, m_destRect.h);
}

void Sprite::setAngle(float angle)
{
    m_angle = angle; 
}

void Sprite::setPivot(int x, int y)
{
    m_pivot = {x, y};
    m_useCustomPivot = true;
}

void Sprite::setFlip(SDL_RendererFlip flip)
{
    m_flip = flip;
}