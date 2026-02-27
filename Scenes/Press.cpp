#include "Press.h"
#include "../Core/Utils.h"
#include "../Core/object.h"
#include "../Core/components.h"
#include "../Core/SceneManager.h"
#include "paddle.h"
#include "SceneDeclarations.h"

Press::Press(Object *_player) : player(_player)
{
}

void Press::Init()
{
    std::vector<unsigned char> blockImgData = Engine::GetResourcesArchive()->GetFile("up_part.png");
    std::vector<unsigned char> blockImgData2 = Engine::GetResourcesArchive()->GetFile("down_part.png");

    upPart = CreateObject();
    downPart = CreateObject();
    upPart->AddComponent(new Image(blockImgData));
    downPart->AddComponent(new Image(blockImgData2));
    // downPart->SetAngle(180);
    // upPart->SetAngle(180);
    downPart->SetLayer(-1);
    upPart->SetLayer(0);
    distance = object->GetScene()->GetWindowSize().y / 2.5f;
    upPart->SetPosition(Vector2(object->GetPosition().x, object->GetPosition().y -400));
    downPart->SetPosition(Vector2(object->GetPosition().x, (object->GetScene()->GetWindowSize().y - 200) + object->GetPosition().y));
}

void Press::update(float deltaTime)
{
    float currentDistance =
        downPart->GetPosition().y -
        (upPart->GetPosition().y + upPart->GetSize().y * 0.98);

    if (max_speed_closing > speed_closing)
        speed_closing += 0.005f;

    if (closing)
    {
        if (currentDistance > 0)
        {
            upPart->MoveY(1 * speed_closing);
            downPart->MoveY(-1 * speed_closing);
            if (player->Crossing(downPart, 0.8f, 1.0f))
            {
                player->GetComponent<PaddleComponent>()->SetStoped();
                player->MoveY(-1 * speed_closing);
            }
            else if (player->Crossing(upPart, 0.8f, 1.0f))
            {
                player->GetComponent<PaddleComponent>()->SetStoped();
                player->MoveY(1 * speed_closing);
            }
        }
        else
        {
            closing = false;
        }
    }
    else
    {
        if (currentDistance < distance)
        {
            upPart->MoveY(-1 * speed_closing);
            downPart->MoveY(1 * speed_closing);
            if (player->Crossing(downPart, 0.8f, 1.0f))
            {
                player->GetComponent<PaddleComponent>()->SetStoped();
                player->MoveY(1 * speed_closing);
                // std::cout<<"crossing\n";
            }
            else if (player->Crossing(upPart, 0.8f, 1.0f))
            {
                player->GetComponent<PaddleComponent>()->SetStoped();
                player->MoveY(-1 * speed_closing);
            }
        }
        else
        {
            closing = true;
        }
    }

    if (current_blocks)
    {
        if (player->Crossing(upPart) || player->Crossing(downPart))
        {
            player->GetComponent<PaddleComponent>()->SetBump(true);
            // std::cout<<"crossing\n";
            // if (player->Crossing(upPart) && player->Crossing(downPart))
            //     object->GetScene()->SwitchToScene(new StartScene);
        }
    }
}

void Press::onKeyReleased(SDL_Keycode key)
{
    if (key == SDLK_RETURN)
    {
        if (current_blocks)
            SlowDown();
    }
}

void Press::SlowDown()
{
    if (speed_closing >= 0.2f)
        speed_closing -= 0.15f;
}

Vector2 Press::GetSizePress() const
{
    return upPart->GetSize();
}

Object *Press::CreateObject()
{
    return object->GetScene()->CreateObject();
}
