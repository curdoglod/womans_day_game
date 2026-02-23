#ifndef INPUT_FIELD_COMPONENT_H
#define INPUT_FIELD_COMPONENT_H

#include "../Core/component.h"
#include "image.h"
#include "text.h" // your TextComponent
#include <SDL.h>
#include <string>
#include <cctype>

class InputFieldComponent : public Component
{
public:
    InputFieldComponent(std::size_t maxLength = 16,
                        const std::string &placeholder = "Enter text")
        : maxLength(maxLength),
          placeholder(placeholder),
          hasFocus(false),
          image(nullptr),
          textComp(nullptr)
    {
    }

    void Init() override
    {
        image = object->GetComponent<Image>();
        if (!image || !image->GetSprite())
        {
            object->AddComponent(new Image(Engine::GetDefaultArchive()->GetFile("ImageDefault.png")));
            image = object->GetComponent<Image>();
        }

        pos = object->GetPosition();
        size = object->GetSize();

        textComp = object->GetComponent<TextComponent>();
        if (!textComp)
        {

            textComp =
                new TextComponent(24, value, TextAlignment::LEFT);
            object->AddComponent(textComp);
            if (value.empty())
            {
                textComp->SetColor(100, 100, 100);
                textComp->setText(placeholder);
            }
        }
        else
        {
            textComp->SetColor(100, 100, 100);
            textComp->setText(placeholder);
        }
    }

    void update(float deltaTime) override
    {
    }

    void OnMouseButtonDown(Vector2 mouse_position) override
    {
        pos = object->GetPosition();
        size = object->GetSize();

        bool inside =
            mouse_position.x >= pos.x && mouse_position.x <= pos.x + size.x &&
            mouse_position.y >= pos.y && mouse_position.y <= pos.y + size.y;

        hasFocus = inside;
        updateVisualState();
    }

    void OnMouseButtonUp(Vector2 mouse_position) override {}
    void OnMouseButtonMotion(Vector2 mouse_position) override {}

    void onKeyPressed(SDL_Keycode key) override
    {
        if (!hasFocus)
            return;

        if (key == SDLK_BACKSPACE)
        {
            if (!value.empty())
            {
                value.pop_back();
                updateText();
            }
            return;
        }

        if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
        {
            // lose focus on Enter (optional)
            hasFocus = false;
            updateVisualState();
            return;
        }

        char c = keyToChar(key);
        if (c != 0 && value.size() < maxLength)
        {
            value.push_back(c);
            hasFocus = true;
            updateVisualState();
            updateText();
        }
    }

    // Get the current value
    const std::string &getValue() const
    {
        return value;
    }

    void setValue(const std::string &newValue)
    {
        value = newValue.substr(0, maxLength);
        updateText();
    }

private:
    std::size_t maxLength;
    std::string placeholder;
    std::string value;
    bool hasFocus;

    Vector2 pos;
    Vector2 size;
    Image *image;
    TextComponent *textComp;

    void updateText()
    {
        if (!textComp)
            return;

        if (value.empty() && !hasFocus)
        {
            textComp->SetColor(100, 100, 100);
            textComp->setText(placeholder);
        }
        else
        {
            textComp->SetColor(0, 0, 0);
            textComp->setText(value);
        }
    }

    void updateVisualState()
    {
        if (!image || !image->GetSprite())
            return;

        if (hasFocus)
        {
            image->GetSprite()->SetColorAndOpacity(220, 220, 255, 1.0f);
        }
        else
        {
            image->GetSprite()->SetColorAndOpacity(255, 255, 255, 1.0f);
        }
        updateText();
    }

    char keyToChar(SDL_Keycode key)
    {
        SDL_Keymod mod = SDL_GetModState();
        bool caps = (mod & KMOD_CAPS) != 0;
        bool shift = (mod & (KMOD_LSHIFT | KMOD_RSHIFT)) != 0;

        if (key >= SDLK_a && key <= SDLK_z)
        {
            char base = static_cast<char>('a' + (key - SDLK_a));
            bool upper = caps ^ shift;
            return upper ? static_cast<char>(toupper(base)) : base;
        }

        if (key >= SDLK_0 && key <= SDLK_9)
        {
            return static_cast<char>('0' + (key - SDLK_0));
        }
        if (key == SDLK_SPACE)
            return ' ';

        return 0;
    }
};

#endif // INPUT_FIELD_COMPONENT_H