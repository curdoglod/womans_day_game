#pragma once
#include "../Core/Utils.h"
#include "../Core/object.h"
#include "../Core/component.h"
#include "../Core/SceneManager.h"

class Object;
class Press : public Component {
public:
    Press(Object* _player); 

    void Init() override; 

    void Update() override;

    void onKeyReleased(SDL_Keycode key) override; 

    void SetCurrent(bool curr)
    {
        current_blocks = curr; 
    }

    Vector2 GetSizePress() const; 
    Object* CreateObject();
private:
    Object* upPart; 
    Object* downPart; 
    float distance; 
    float speed_closing = 0; 
    float max_speed_closing = 2; 
    Object* player; 
    bool closing = true;
    bool current_blocks = false; 
};

