#ifndef IMAGE_H
#define IMAGE_H

#include "../Core/component.h"
#include "../Core/sprite.h"
#include "../Core/SceneManager.h"
#include "../Core/engine.h"

class Image : public Component {
public:
    Image(const std::vector<unsigned char>& imgData) {
        this->imgData = imgData;
        sprite = nullptr; 
    }

    void Init() override  {
 
        SetNewSprite(imgData);
 
    }

    void SetNewSprite(const std::vector<unsigned char>& imgData) {
        if (sprite != nullptr) delete sprite;
        sprite = object->GetScene()->createSprite(imgData);
        if(!sprite)  sprite = object->GetScene()->createSprite(Engine::GetDefaultArchive()->GetFile("ImageDefault.png"));
        
        if (sprite != nullptr) {
            size = sprite->getSize();
            object->InitSize(this);
        }
    }

    void update(float deltaTime) override;

    void SetSize(const Vector2& newSize) {
        if (sprite != nullptr) {
            sprite->setSize((int)newSize.x, (int)newSize.y);
            size = newSize; 
            object->InitSize(); 
        }
        else {
            std::cerr << "Sprite is null" << std::endl;
        }
    }


    Vector2 GetSize() const {

        return size; 
    }

    Sprite* GetSprite() const {
        return sprite; 
    }

    ~Image() {
        delete sprite; 
    }


private:
    std::vector<unsigned char> imgData;
    Sprite* sprite; 
    Vector2 size;
};

#endif 
