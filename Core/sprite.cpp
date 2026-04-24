#include "sprite.h"
#include <SDL.h>
#include <SDL_image.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include "Utils.h"

namespace
{
struct TextureCacheEntry
{
    SDL_Texture *texture = nullptr;
    int width = 0;
    int height = 0;
};

std::unordered_map<std::uint64_t, TextureCacheEntry> g_textureCache;

std::uint64_t HashImageData(const std::vector<unsigned char> &imageData, SDL_Renderer *renderer)
{
    constexpr std::uint64_t kOffsetBasis = 1469598103934665603ULL;
    constexpr std::uint64_t kPrime = 1099511628211ULL;

    std::uint64_t hash = kOffsetBasis;
    for (unsigned char byte : imageData)
    {
        hash ^= static_cast<std::uint64_t>(byte);
        hash *= kPrime;
    }

    hash ^= static_cast<std::uint64_t>(imageData.size());
    hash *= kPrime;
    hash ^= static_cast<std::uint64_t>(reinterpret_cast<std::uintptr_t>(renderer));
    hash *= kPrime;

    return hash;
}
} // namespace

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

Sprite::Sprite(const std::vector<unsigned char> &imageData, SDL_Renderer *renderer)
    : m_renderer(renderer), m_texture(nullptr)
{
    m_destRect = {0, 0, 0, 0};

    if (imageData.empty() || renderer == nullptr)
    {
        return;
    }

    const std::uint64_t cacheKey = HashImageData(imageData, renderer);
    auto cachedTexture = g_textureCache.find(cacheKey);
    if (cachedTexture != g_textureCache.end())
    {
        m_texture = cachedTexture->second.texture;
        m_destRect.w = cachedTexture->second.width;
        m_destRect.h = cachedTexture->second.height;
        return;
    }

    SDL_RWops *rw = SDL_RWFromConstMem(imageData.data(), static_cast<int>(imageData.size()));
    if (!rw)
    {
        std::cerr << "SDL_RWFromConstMem failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Surface *surface = IMG_Load_RW(rw, 1);
    if (!surface)
    {
        std::cerr << "IMG_Load_RW failed: " << SDL_GetError() << std::endl;
        return;
    }

    m_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!m_texture)
    {
        std::cerr << "SDL_CreateTextureFromSurface failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_destRect.w, &m_destRect.h);
    g_textureCache[cacheKey] = {m_texture, m_destRect.w, m_destRect.h};
}

Sprite::~Sprite()
{
}

void Sprite::ClearTextureCache()
{
    for (auto &cacheEntry : g_textureCache)
    {
        if (cacheEntry.second.texture != nullptr)
        {
            SDL_DestroyTexture(cacheEntry.second.texture);
        }
    }
    g_textureCache.clear();
}

void Sprite::draw()
{
    if (m_texture == nullptr)
        return;

    SDL_SetTextureColorMod(m_texture, m_colorR, m_colorG, m_colorB);
    SDL_SetTextureAlphaMod(m_texture, m_alpha);
    SDL_Point *pivot = m_useCustomPivot ? &m_pivot : nullptr;
    SDL_RenderCopyEx(m_renderer, m_texture, nullptr, &m_destRect, m_angle, pivot, m_flip);
}

void Sprite::draw(const Vector2& vec2)
{
    if (m_texture == nullptr)
        return;

    m_destRect.x = (int)vec2.x;
    m_destRect.y = (int)vec2.y;
    SDL_SetTextureColorMod(m_texture, m_colorR, m_colorG, m_colorB);
    SDL_SetTextureAlphaMod(m_texture, m_alpha);
    SDL_Point *pivot = m_useCustomPivot ? &m_pivot : nullptr;
    SDL_RenderCopyEx(m_renderer, m_texture, nullptr, &m_destRect, m_angle, pivot, m_flip);
}

void Sprite::SetColorAndOpacity(Uint8 red, Uint8 green, Uint8 blue, float alpha)
{
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    m_colorR = red;
    m_colorG = green;
    m_colorB = blue;
    m_alpha = static_cast<Uint8>(alpha * 255.0f);
}

void Sprite::setPosition(int x, int y)
{
    m_destRect.x = x;
    m_destRect.y = y;
}

void Sprite::setSize(int width, int height)
{
    m_destRect.w = width;
    m_destRect.h = height;
}

Vector2 Sprite::getSize()
{
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