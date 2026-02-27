#pragma once
#include "../Core/component.h"

class Rigidbody;
class Camera;
class Image;

class PaddleComponent : public Component
{
public:
    void Init() override;

    void onKeyPressed(SDL_Keycode key) override;

    void onKeyReleased(SDL_Keycode key) override;

    void update(float deltaTime) override;

    void SetBump(bool _bump);

    void SetStoped();

    void SetCar(int number);

    void SetFrozen(bool frozen);
    bool IsFrozen() const { return _frozen; }
    Camera* GetCamera() const { return camera; }
    
private:
    Rigidbody *_rigidbody;
    Vector2 windowSize;
    int moveDirection = 0;
    Camera *camera;
    bool bump;
    bool _frozen = false;
    Image* img; 
    int setedCar; 
    std::vector<std::vector<unsigned char>> carImgData; 
    Vector2 max_speed; 
};
