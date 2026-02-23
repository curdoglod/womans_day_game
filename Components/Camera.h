#pragma once
#include "../Core/Utils.h"
#include "../Core/object.h"
#include "../Core/component.h"
#include "../Core/SceneManager.h"

class Object;
class Camera : public Component {
public:
    Camera(); 

    void Init() override; 

    void Update() override;

    void setPosition(const Vector2& v) { position = v; }
    void setOffset(const Vector2& v) { offset = v; }
    Vector2 getPosition() const { return position; }
    Vector2 getOffset() const { return offset; }

private:
    Vector2 position;
    Vector2 offset;

};

