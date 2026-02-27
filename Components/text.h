#ifndef TEXT_H
#define TEXT_H

#include "../Core/component.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>


enum class TextAlignment {
    LEFT,
    CENTER,
    RIGHT
};


class TextComponent : public Component {
public:
   
    

    /**
   Font Size
   Text
   Alignment TextAlignment::LEFT, CENTER RIGHT
  */
    TextComponent(int fontSize, const std::string& text, TextAlignment alignment = TextAlignment::LEFT)
        : text(text), color({0, 0, 0}), font(nullptr), texture(nullptr), outlineTexture(nullptr), fontSize(fontSize), m_renderer(nullptr), alignment(alignment), outlineSize(0), outlineColor({0, 0, 0, 255}) {

    }

    ~TextComponent() {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
        if (outlineTexture) {
            SDL_DestroyTexture(outlineTexture);
        }
        if (font) {
            TTF_CloseFont(font);
        }

    }

    void SetFont(std::vector<unsigned char>  _fontDataBuffer) {
        if (font) {
            TTF_CloseFont(font);
            font = nullptr;
        }

        SDL_RWops* rw = SDL_RWFromConstMem(_fontDataBuffer.data(), _fontDataBuffer.size());
        if (!rw) {
            std::cerr << "Failed to create SDL_RWops for font data." << std::endl;
            return;
        }

        font = TTF_OpenFontRW(rw, 1, fontSize);
        if (!font) {
            std::cerr << "Failed to load font from data buffer: " << TTF_GetError() << std::endl;
            SDL_RWclose(rw);
            return;
        }

        UpdateTextures();
    }
    /**
    Text
   */
    void setText(const std::string& newText) {
        text = newText;
        UpdateTextures();
    }

    void SetOutline(int size, const SDL_Color& col = {0, 0, 0, 255}) {
        outlineSize = size;
        outlineColor = col;
        UpdateTextures();
    }

    void UpdateTextures() {
        if (texture) {
            SDL_DestroyTexture(texture);
            texture = nullptr;
        }
        if (outlineTexture) {
            SDL_DestroyTexture(outlineTexture);
            outlineTexture = nullptr;
        }

        if (!font) {
            return;
        }
       
        if (text.empty()) {
            return;
        }

        // Render Outline
        if (outlineSize > 0) {
            TTF_SetFontOutline(font, outlineSize);
            SDL_Surface* bg_surface = TTF_RenderText_Blended(font, text.c_str(), outlineColor);
            if (bg_surface) {
                outlineTexture = SDL_CreateTextureFromSurface(m_renderer, bg_surface);
                SDL_FreeSurface(bg_surface);
            }
            TTF_SetFontOutline(font, 0); // reset outline
        }

        // Render Foreground text
        SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surface) {
            return;
        }

        texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        SDL_FreeSurface(surface);
    }

    /**
   Alignment TextAlignment::LEFT, CENTER RIGHT
  */
    void SetAlignment(TextAlignment newAlignment) {
        alignment = newAlignment;
    }

    void Init() override {

        m_renderer = object->GetRenderer();
        fontDataBuffer = Engine::GetDefaultArchive()->GetFile("Roboto-Black.ttf");
      

        SDL_RWops* rw = SDL_RWFromConstMem(fontDataBuffer.data(), fontDataBuffer.size());
        if (!rw) {
            std::cerr << "Failed to create SDL_RWops for font data." << std::endl;
            return;
        }

        font = TTF_OpenFontRW(rw, 1, fontSize);
         if (!font) {
              SDL_RWclose(rw);
          }

        UpdateTextures();
    }

    void update(float deltaTime) override {
        if (texture) {
            SDL_Rect renderQuad = { 0, 0, 0, 0 };
            SDL_QueryTexture(texture, NULL, NULL, &renderQuad.w, &renderQuad.h);

           
            Vector2 spriteSize = object->GetSize();

            //std :: cout << object->GetSize() <<std::endl; 
      
            switch (alignment) {
            case TextAlignment::LEFT:
                renderQuad.x = object->GetPosition().x + offsetX;
                break;
            case TextAlignment::CENTER:
                renderQuad.x = object->GetPosition().x - renderQuad.w / 2 + spriteSize.x / 2 + offsetX;
                break;
            case TextAlignment::RIGHT:
                renderQuad.x = object->GetPosition().x + spriteSize.x -  renderQuad.w + offsetX;
                break;
            }

     
            renderQuad.y = object->GetPosition().y + (spriteSize.y - renderQuad.h) / 2 + offsetY;

            if (outlineTexture && outlineSize > 0) {
                SDL_Rect outlineQuad = renderQuad;
                outlineQuad.x -= outlineSize;
                outlineQuad.y -= outlineSize;
                SDL_QueryTexture(outlineTexture, NULL, NULL, &outlineQuad.w, &outlineQuad.h);
                SDL_RenderCopy(m_renderer, outlineTexture, NULL, &outlineQuad);
            }

            SDL_RenderCopy(m_renderer, texture, NULL, &renderQuad);
        }
    }

    void SetColor(const Uint8& r, const Uint8& g , const Uint8& b, const Uint8& a = 255)
    {
        color = {r,g,b,a}; 
        UpdateTextures();
    }

    void SetOffset(float x, float y) {
        offsetX = x;
        offsetY = y;
    }
private:
    std::string text;
    SDL_Color color;
    TTF_Font* font;
    std::vector<unsigned char> fontDataBuffer;
    int fontSize; 
    SDL_Texture* texture;
    SDL_Texture* outlineTexture;
    SDL_Renderer* m_renderer;
    TextAlignment alignment;
    int outlineSize;
    SDL_Color outlineColor;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
   
};

#endif // TEXT_COMPONENT_H
