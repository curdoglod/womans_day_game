
#include "Camera.h"
#include "../Core/SceneManager.h" 
#include "../Core/object.h"

Camera::Camera(): position(0,0), offset(400,0), active(false) {}

void Camera::Init()
{
    object->GetScene()->SetCamera(this);
    active = true; 
}

void Camera::update(float deltaTime)
{
    if(!active) return;

    if((object->GetPosition().x - position.x) > offset.x)
         position = Vector2(object->GetPosition().x, 0) - offset; 
    else 
     if((object->GetPosition().x - position.x)< offset.x/10  )
        position = Vector2(object->GetPosition().x-offset.x/10, 0) ; 
    //std::cout<< object->GetPosition().x - position.x <<std:: endl; 
}
