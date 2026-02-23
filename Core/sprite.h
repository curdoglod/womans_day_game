#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>
#include <string>
#include "Utils.h"
#include <vector>
class Sprite {
public:

    ~Sprite();

    void draw();
    void draw(const Vector2& vec2);
    void setPosition(int x, int y);
    void setSize(int width, int height);
    Vector2 getSize();
    friend class SceneManager;
    void SetColorAndOpacity(Uint8 red, Uint8 green, Uint8 blue, float alpha);
    void setAngle(float angle); 
private:
   // Sprite(SDL_Texture* m_texture, SDL_Renderer* renderer);
    Sprite(const std::vector<unsigned char>& imageData, SDL_Renderer* renderer);
    SDL_Texture* m_texture;
    SDL_Renderer* m_renderer;
    SDL_Rect m_destRect;
    float m_angle = 0.0f; 
};

#endif // SPRITE_H
