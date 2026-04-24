#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>
#include <string>
#include "Utils.h"
#include <vector>
class Sprite {
public:

    ~Sprite();
    static void ClearTextureCache();

    void draw();
    void draw(const Vector2& vec2);
    void setPosition(int x, int y);
    void setSize(int width, int height);
    Vector2 getSize();
    friend class SceneManager;
    void SetColorAndOpacity(Uint8 red, Uint8 green, Uint8 blue, float alpha);
    void setAngle(float angle); 
    void setPivot(int x, int y);
    void setFlip(SDL_RendererFlip flip);
private:
   // Sprite(SDL_Texture* m_texture, SDL_Renderer* renderer);
    Sprite(const std::vector<unsigned char>& imageData, SDL_Renderer* renderer);
    SDL_Texture* m_texture = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    SDL_Rect m_destRect;
    float m_angle = 0.0f;
    SDL_Point m_pivot;
    bool m_useCustomPivot = false;
    SDL_RendererFlip m_flip = SDL_FLIP_NONE;
    Uint8 m_colorR = 255;
    Uint8 m_colorG = 255;
    Uint8 m_colorB = 255;
    Uint8 m_alpha = 255;
};

#endif // SPRITE_H
