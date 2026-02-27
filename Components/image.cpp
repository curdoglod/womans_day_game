#include "../Core/component.h"
#include "../Core/sprite.h"
#include "../Core/SceneManager.h"
#include "../Core/engine.h"
#include "image.h"

void Image::update(float deltaTime)
{
    if (sprite == nullptr) return;

    if (!object->isFixedCamera())
        sprite->draw(object->GetPosition() - object->GetScene()->GetCameraPosition());
    else
        sprite->draw(object->GetPosition());

    sprite->setAngle(object->GetAngle());
}