#pragma once
#include "../Core/SceneManager.h"
#include "../Core/components.h"
struct PlayerRecord;

class WinScene : public SceneManager
{

    void Init() override;
	void onKeyReleased(SDL_Keycode key) override;

private:
    Object *start_button;
    Image *startBttn_image;
    void ShowTable(const std::vector<PlayerRecord> &allRecords,
                             const std::vector<size_t> &indices,
                             int posY);
};
