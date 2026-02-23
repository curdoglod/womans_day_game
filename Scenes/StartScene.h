#pragma once
#include "../Core/SceneManager.h"
#include "../Core/components.h"

class StartScene : public SceneManager {


    void Init() override; 
	void onKeyReleased(SDL_Keycode key) override;

private:
    Object* start_button;
    Image* startBttn_image; 
};
