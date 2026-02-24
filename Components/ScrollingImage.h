#ifndef SCROLLING_IMAGE_H
#define SCROLLING_IMAGE_H

#include "../Core/component.h"
#include "../Core/SceneManager.h"
#include "../Core/engine.h"
#include <SDL.h>
#include <SDL_image.h>
#include <vector>


class ScrollingImage : public Component
{
public:

    ScrollingImage(const std::vector<unsigned char> &imgData,
                   float speed = 150.0f,
                   int displayW = 0, int displayH = 0)
        : imgData(imgData), speed(speed),
          displayW(displayW), displayH(displayH) {}

    void SetDisplaySize(int w, int h)
    {
        displayW = w;
        displayH = h;
    }

    void SetSpeed(float s) { speed = s; }

    void Init() override
    {
        SDL_RWops *rw = SDL_RWFromMem((void *)imgData.data(), (int)imgData.size());
        SDL_Surface *surface = IMG_Load_RW(rw, 1);
        if (surface)
        {
            texture = SDL_CreateTextureFromSurface(object->GetRenderer(), surface);
            texW = surface->w;
            texH = surface->h;
            SDL_FreeSurface(surface);
        }
        if (displayW == 0) displayW = texW;
        if (displayH == 0) displayH = texH;
    }

    void update(float deltaTime) override
    {
        if (!texture) return;

        scrollOffset += speed * deltaTime;
        if (scrollOffset >= texW) scrollOffset -= texW;
        if (scrollOffset < 0)    scrollOffset += texW;

        Vector2 pos = object->GetPosition();
        if (!object->isFixedCamera())
            pos = pos - object->GetScene()->GetCameraPosition();

        int x = (int)pos.x;
        int y = (int)pos.y;
        int off = (int)scrollOffset;

        int leftSrcW  = texW - off;
        int leftDstW  = (int)((float)leftSrcW / texW * displayW);
        SDL_Rect src1 = { off, 0, leftSrcW, texH };
        SDL_Rect dst1 = { x,  y, leftDstW, displayH };
        SDL_RenderCopy(object->GetRenderer(), texture, &src1, &dst1);

        if (off > 0)
        {
            SDL_Rect src2 = { 0, 0, off, texH };
            SDL_Rect dst2 = { x + leftDstW, y, displayW - leftDstW, displayH };
            SDL_RenderCopy(object->GetRenderer(), texture, &src2, &dst2);
        }
    }

    ~ScrollingImage() override
    {
        if (texture) SDL_DestroyTexture(texture);
    }

private:
    std::vector<unsigned char> imgData;
    SDL_Texture *texture = nullptr;
    float speed;
    float scrollOffset = 0.0f;
    int texW = 0, texH = 0;
    int displayW, displayH;
};

#endif // SCROLLING_IMAGE_H
