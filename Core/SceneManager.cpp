#include "../Components/Camera.h"
#include "SceneManager.h"
#include <iostream>

void SceneManager::SetCamera(Camera *cam)
{
    camera = cam;
}

void SceneManager::UpdateScene(float deltaTime)
{
    Update();
    for (auto &object : objects)
    {
        if (object->active)
        {
            object->update(deltaTime);
            // if (camera != nullptr && !object->fixedCamera)
            // {
            //     //object->SetPosition(object->GetPosition() + );
                
            //     object->screen_position = camera->getPosition(); 
            //     std::cout<<camera->getPosition().x<<std::endl; 
            // }
        }
    }
}


Vector2 SceneManager::GetCameraPosition() const
{
    return  camera == nullptr ?  Vector2(0,0) :  camera->getPosition(); 
}