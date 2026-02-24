#pragma once
#include "../Core/component.h"
#include "../Core/components.h"
#include "paddle.h"
#include <cmath>

class RobotHand : public Component
{
public:
    enum class Phase
    {
        DESCEND,
        GRAB,
        LIFT,
        CARRY,
        LOWER,
        RELEASE,
        RETREAT,
        DONE
    };

    RobotHand(Object *_player, float _targetX, float _groundY = -1.0f, bool _keepFrozen = false)
        : player(_player), targetX(_targetX), customGroundY(_groundY), keepFrozen(_keepFrozen) {}

    void Init() override
    {
        auto clawData = Engine::GetResourcesArchive()->GetFile("hand0.png");
        auto armData = Engine::GetResourcesArchive()->GetFile("hand1.png");

        clawObj = object->GetScene()->CreateObject();
        clawObj->AddComponent(new Image(clawData));
        clawObj->SetLayer(100);
        Image *clawImg = clawObj->GetComponent<Image>();
        clawImg->SetSize(clawImg->GetSize() / 2.0f);
        clawImg->SetPivot((int)(clawImg->GetSize().x / 2), 0);
        clawImg->SetFlip(SDL_FLIP_HORIZONTAL);

        armObj = object->GetScene()->CreateObject();
        armObj->AddComponent(new Image(armData));
        armObj->SetLayer(99);
        Image *armImg = armObj->GetComponent<Image>();
        armImg->SetSize(armImg->GetSize() / 2.0f);
        armImg->SetPivot((int)(armImg->GetSize().x / 2), 0);

        Vector2 playerPos = player->GetPosition();
        Vector2 playerSize = player->GetSize();
        Vector2 clawSize = clawObj->GetSize();

        Vector2 clawStart = clawPosForPlayerPos(playerPos);
        clawObj->SetPosition(Vector2(clawStart.x, -clawSize.y));
        clawObj->SetAngle(baseClawAngle);
        armObj->SetAngle(baseArmAngle);
        updateArmPosition();

        grabY = playerPos.y - clawSize.y + 20;
        liftY = 50.0f;
        groundY = (customGroundY > 0.0f) ? customGroundY : object->GetScene()->GetWindowSize().y / 2 + 50;

        phase = Phase::DESCEND;
    }

    void update(float deltaTime) override
    {
        if (phase == Phase::DONE)
            return;

        Vector2 clawPos = clawObj->GetPosition();
        Vector2 clawSize = clawObj->GetSize();
        Vector2 playerSize = player->GetSize();

        float dt = deltaTime * 60.0f;

        switch (phase)
        {
        case Phase::DESCEND:
        {
            // Целевая позиция клешни — из текущей позиции игрока
            Vector2 targetClawPos = clawPosForPlayerPos(player->GetPosition());

            // Плавно обновляем grabY
            if (targetClawPos.y < grabY)
                grabY = targetClawPos.y;
            else
                grabY += (targetClawPos.y - grabY) * 0.05f * dt;

            // Плавно следим за игроком по X
            clawPos.x += (targetClawPos.x - clawPos.x) * 0.1f * dt;
            clawPos.y += descendSpeed * dt;
            clawObj->SetPosition(clawPos);

            float totalDist = grabY + clawSize.y;
            float progress = totalDist > 0 ? (clawPos.y + clawSize.y) / totalDist : 1.0f;
            float swingAngle = std::sin(std::min(progress, 1.0f) * 3.14159f) * 8.0f;
            clawObj->SetAngle(baseClawAngle + swingAngle);
            armObj->SetAngle(baseArmAngle + swingAngle * 0.5f);
            updateArmPosition();

            if (clawPos.y >= grabY)
            {
                player->GetComponent<PaddleComponent>()->SetFrozen(true);
                Vector2 snapClawPos = clawPosForPlayerPos(player->GetPosition());
                clawObj->SetPosition(snapClawPos);
                clawObj->SetAngle(baseClawAngle);
                armObj->SetAngle(baseArmAngle);
                updateArmPosition();
                syncPlayerToClaw();
                phase = Phase::GRAB;
                timer = 0;
            }
            break;
        }
        case Phase::GRAB:
        {
            syncPlayerToClaw();
            timer += dt;
            if (timer > 40)
            {
                phase = Phase::LIFT;
            }
            break;
        }
        case Phase::LIFT:
        {
            clawPos.y -= liftSpeed * dt;
            clawObj->SetPosition(clawPos);
            updateArmPosition();
            syncPlayerToClaw();

            if (clawPos.y <= liftY)
            {
                clawObj->SetPosition(Vector2(clawPos.x, liftY));
                phase = Phase::CARRY;
            }
            break;
        }
        case Phase::CARRY:
        {
            Vector2 currentPlayerPos = playerPosForClawPos(clawPos);
            float dist = targetX - currentPlayerPos.x;
            float dir = (dist > 0) ? 1.0f : -1.0f;

            float speed = carrySpeed * dt;
            if (std::abs(dist) < 100.0f)
                speed *= std::abs(dist) / 100.0f + 0.1f;

            float newClawX = clawPos.x + dir * speed;

            float swingAngle = dir * std::min(std::abs(dist) / 50.0f, 5.0f);
            clawObj->SetAngle(baseClawAngle + swingAngle);
            armObj->SetAngle(baseArmAngle + swingAngle * 0.5f);

            Vector2 newPlayerPos = playerPosForClawPos(Vector2(newClawX, clawPos.y));
            bool arrived = (dir > 0) ? (newPlayerPos.x >= targetX) : (newPlayerPos.x <= targetX);
            if (arrived)
            {
                Vector2 finalClawPos = clawPosForPlayerPos(Vector2(targetX, newPlayerPos.y));
                newClawX = finalClawPos.x;
                clawObj->SetAngle(baseClawAngle);
                armObj->SetAngle(baseArmAngle);
            }

            clawObj->SetPosition(Vector2(newClawX, clawPos.y));
            updateArmPosition();
            syncPlayerToClaw();

            if (arrived)
                phase = Phase::LOWER;
            break;
        }
        case Phase::LOWER:
        {
            clawPos.y += liftSpeed * dt;
            clawObj->SetPosition(clawPos);
            updateArmPosition();
            syncPlayerToClaw();

            float playerY = player->GetPosition().y;
            if (playerY >= groundY)
            {
                player->SetPosition(Vector2(player->GetPosition().x, groundY));
                phase = Phase::RELEASE;
                timer = 0;
            }
            break;
        }
        case Phase::RELEASE:
        {
            timer += dt;
            if (timer > 30)
            {
                if (!keepFrozen)
                    player->GetComponent<PaddleComponent>()->SetFrozen(false);
                else
                {
                    player->GetComponent<Rigidbody>()->setVelocity(Vector2(60, 0)); 
                }
                player->SetAngle(0);
                phase = Phase::RETREAT;
            }
            break;
        }
        case Phase::RETREAT:
        {
            clawPos.y -= descendSpeed * 2.0f * dt;
            clawObj->SetPosition(clawPos);
            updateArmPosition();

            if (clawPos.y < -clawSize.y - 300)
            {
                phase = Phase::DONE;
                completed = true;
            }
            break;
        }
        case Phase::DONE:
            break;
        }
    }

    bool IsCompleted() const { return completed; }

    void Cleanup()
    {
        if (clawObj)
        {
            object->GetScene()->DeleteObject(clawObj);
            clawObj = nullptr;
        }
        if (armObj)
        {
            object->GetScene()->DeleteObject(armObj);
            armObj = nullptr;
        }
    }

private:
    Object *player;
    float targetX;
    float customGroundY = -1.0f;
    bool keepFrozen = false;

    Object *clawObj = nullptr;
    Object *armObj = nullptr;

    Phase phase = Phase::DESCEND;
    float timer = 0;
    bool completed = false;

    float grabY, liftY, groundY;

    const float descendSpeed = 3.5f;
    const float liftSpeed = 3.0f;
    const float carrySpeed = 5.0f;

    float baseArmAngle = 0.0f;
    float baseClawAngle = 40.0f;
    float clawAttachOffsetY = 30.0f;
    float clawAttachOffsetX = -75.0f; // dont touch this

    float playerGrabOffsetX = 112.0f;
    float playerGrabOffsetY = -25.0f;

    void updateArmPosition()
    {
        if (!armObj || !clawObj)
            return;
        Vector2 clawPos = clawObj->GetPosition();
        Vector2 armSize = armObj->GetSize();
        Vector2 clawSize = clawObj->GetSize();

        float currentArmAngle = armObj->GetAngle();
        float rad = currentArmAngle * 3.14159f / 180.0f;

        float currentClawAngle = clawObj->GetAngle();
        float clawRad = currentClawAngle * 3.14159f / 180.0f;

        float dx = -armSize.y * std::sin(rad);
        float dy = armSize.y * std::cos(rad);

        float localAttachX = clawAttachOffsetX;
        float localAttachY = clawAttachOffsetY;

        float rotatedAttachX = localAttachX * std::cos(clawRad) - localAttachY * std::sin(clawRad);
        float rotatedAttachY = localAttachX * std::sin(clawRad) + localAttachY * std::cos(clawRad);

        float attachX = clawPos.x + clawSize.x / 2.0f + rotatedAttachX;
        float attachY = clawPos.y + rotatedAttachY;

        // Позиция верхнего пивота плеча
        float pivotX = attachX - dx;
        float pivotY = attachY - dy;

        // Устанавливаем координаты плеча (относительно левого верхнего угла)
        armObj->SetPosition(Vector2(
            pivotX - armSize.x / 2.0f,
            pivotY));
    }

    Vector2 clawPosForPlayerPos(Vector2 playerPos)
    {
        Vector2 cSize = clawObj->GetSize();
        Vector2 pSize = player->GetSize();
        float rad = baseClawAngle * 3.14159f / 180.0f;
        float localX = playerGrabOffsetX;
        float localY = cSize.y + playerGrabOffsetY;
        float rotX = localX * std::cos(rad) - localY * std::sin(rad);
        float rotY = localX * std::sin(rad) + localY * std::cos(rad);
        return Vector2(
            playerPos.x + pSize.x / 2.0f - cSize.x / 2.0f - rotX,
            playerPos.y - rotY);
    }

    Vector2 playerPosForClawPos(Vector2 clawPos)
    {
        Vector2 cSize = clawObj->GetSize();
        Vector2 pSize = player->GetSize();
        float rad = baseClawAngle * 3.14159f / 180.0f;
        float localX = playerGrabOffsetX;
        float localY = cSize.y + playerGrabOffsetY;
        float rotX = localX * std::cos(rad) - localY * std::sin(rad);
        float rotY = localX * std::sin(rad) + localY * std::cos(rad);
        return Vector2(
            clawPos.x + cSize.x / 2.0f + rotX - pSize.x / 2.0f,
            clawPos.y + rotY);
    }

    void syncPlayerToClaw()
    {
        if (!clawObj || !player)
            return;
        Vector2 cPos = clawObj->GetPosition();
        Vector2 cSize = clawObj->GetSize();
        Vector2 pSize = player->GetSize();

        float clawAngle = clawObj->GetAngle();
        float rad = clawAngle * 3.14159f / 180.0f;

        // Локальная точка захвата относительно верхнего центра клешни
        float localX = playerGrabOffsetX;
        float localY = cSize.y + playerGrabOffsetY;

        // Вращаем вокруг верхнего центра клешни
        float rotX = localX * std::cos(rad) - localY * std::sin(rad);
        float rotY = localX * std::sin(rad) + localY * std::cos(rad);

        float px = cPos.x + cSize.x / 2.0f + rotX - pSize.x / 2.0f;
        float py = cPos.y + rotY;
        player->SetPosition(Vector2(px, py));
        player->SetAngle(clawAngle * 0.7f);
    }
};
