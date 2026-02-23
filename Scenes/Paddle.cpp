#include "paddle.h"
#include "../Core/components.h"
#include "iostream"
#include "math.h"
#include "../Core/SceneManager.h"

void PaddleComponent::Init()
{
    for(int i=0 ; i<6; i++)
        carImgData.push_back(Engine::GetResourcesArchive()->GetFile("car"+std::to_string(i)+".png"));
    setedCar=0; 
    img = new Image(carImgData[setedCar]);  
    object->AddComponent(img);
    _rigidbody = new Rigidbody();
    _rigidbody->setMass(10);
    object->AddComponent(_rigidbody);
    camera = new Camera();
    object->AddComponent(camera);
    object->SetPosition(Vector2(175, 500));
    max_speed.x = 70;
    _rigidbody->setVelocity(Vector2(max_speed.x, _rigidbody->getVelocity().y));
}

void PaddleComponent::onKeyPressed(SDL_Keycode key)
{

    // if (key == SDLK_LEFT)
    // {
    //     moveDirection = -1;
    // }
    // else if (key == SDLK_RIGHT)
    // {
    //     moveDirection = 1;
    // }
}

void PaddleComponent::onKeyReleased(SDL_Keycode key)
{
    if (_frozen) return;

    if (key == SDLK_SPACE)
    {
        if (_rigidbody->getVelocity().y >= -50.0f && _rigidbody->isUsingGravity())
            _rigidbody->setVelocity(Vector2(_rigidbody->getVelocity().x, _rigidbody->getVelocity().y - 70.0f));
    }
}

void PaddleComponent::Update()
{
    if (_frozen || object == nullptr)
        return;

    windowSize = object->GetScene()->GetWindowSize();

    if (object->GetPosition().x <= 0)
        object->SetPosition(Vector2(0, object->GetPosition().y));

    if (object->GetPosition().y > windowSize.y - object->GetSize().y)
    {
        object->SetPosition(Vector2(object->GetPosition().x, windowSize.y - object->GetSize().y));
        _rigidbody->setVelocity(Vector2(_rigidbody->getVelocity().x, 0));
    }

    if (object->GetPosition().y < 0)
    {
        object->SetPosition(Vector2(object->GetPosition().x, 0));
        _rigidbody->setVelocity(Vector2(_rigidbody->getVelocity().x, 0));
    }

    if (bump)
    {
        if (_rigidbody->getVelocity().x > 0)
            bump = false;
    }

    if (_rigidbody->getVelocity().x < max_speed.x)
    {
        _rigidbody->setVelocity(Vector2(_rigidbody->getVelocity().x + 1.0f, _rigidbody->getVelocity().y));
    }
}

void PaddleComponent::SetBump(bool _bump)
{
    bump = _bump;
    _rigidbody->setVelocity(Vector2(-100.0f, _rigidbody->getVelocity().y));
}

void PaddleComponent::SetStoped()
{
    _rigidbody->setVelocity(Vector2(_rigidbody->getVelocity().x, 0));
    // _rigidbody->setUseGravity(false); 
}


void PaddleComponent::SetCar(int number)
{
    if(setedCar!=number && carImgData.size()>number)
        img->SetNewSprite(carImgData[number]); 
    setedCar = number; 
}

void PaddleComponent::SetFrozen(bool frozen)
{
    _frozen = frozen;
    if (frozen)
    {
        _rigidbody->setVelocity(Vector2(0, 0));
        _rigidbody->setUseGravity(false);
    }
    else
    {
        _rigidbody->setUseGravity(true);
        _rigidbody->setVelocity(Vector2(max_speed.x, 0));
    }
}