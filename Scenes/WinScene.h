#pragma once
#include "../Core/SceneManager.h"
#include "../Core/components.h"
#include <string>
struct PlayerRecord;

class WinScene : public SceneManager
{
public:
    explicit WinScene(const std::string &playerName) : playerName_(playerName) {}

    void Init() override;
    void onKeyReleased(SDL_Keycode key) override;

private:
    std::string playerName_;
    int currId_ = 0;

    void ShowTable(const std::vector<PlayerRecord> &allRecords,
                   const std::vector<size_t> &indices,
                   int posY);
};
