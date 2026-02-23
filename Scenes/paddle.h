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

    void Update() override;

    void SetBump(bool _bump);

    void SetStoped();

    void SetCar(int number);

private:
    Rigidbody *_rigidbody;
    Vector2 windowSize;
    int moveDirection = 0;
    Camera *camera;
    bool bump;
    Image* img; 
    int setedCar; 
    std::vector<std::vector<unsigned char>> carImgData; 
    Vector2 max_speed; 
};
