#include "SceneDeclarations.h"
#include "ScoreBoard.h"

void WinScene::Init()
{
    std::vector<unsigned char> bck_screen_img = Engine::GetResourcesArchive()->GetFile("bckscreen.png");
    Object *bck_screen = CreateObject();
    bck_screen->SetLayer(-1000);
    bck_screen->AddComponent(new Image(bck_screen_img));
    SetWindowSize(1280, 720);
    //ScoreBoard *scoreBoard = new ScoreBoard(ScoreBoard::DEFAULT_PATH);
    std::vector<unsigned char> winImgData = Engine::GetResourcesArchive()->GetFile("win_screen.png");

    Object *win_screen = CreateObject();
    win_screen->AddComponent(new Image(winImgData));
    win_screen->GetComponent<Image>()->SetSize(Vector2(1280, 720)); 

    Object *timeObj = CreateObject();
    timeObj->SetPosition(Vector2(640, 361));
    TextComponent *timeTextComp = new TextComponent(49, timeText_, TextAlignment::LEFT);
    timeTextComp->SetOutline(0, {0, 0, 0, 255});
    timeTextComp->SetColor(0, 0, 0);
    timeObj->AddComponent(timeTextComp);

    std::vector<unsigned char> button_img = Engine::GetResourcesArchive()->GetFile("button_restart.png");

    Object *restart_button = CreateObject();
    restart_button->SetPosition(Vector2(898 / 2, 995 / 2));
        restart_button->AddComponent(new ButtonComponent([this]()
                                                                                                         { SwitchToScene(new MainGameScene("")); }));

        Image *restartBttn_image = restart_button->GetComponent<Image>();
        restartBttn_image->SetNewSprite(button_img);
    restartBttn_image->SetSize(restartBttn_image->GetSize() / 2);
}

void WinScene::ShowTable(const std::vector<PlayerRecord> &allRecords,
                         const std::vector<size_t> &indices,
                         int posY)
{
    float rowHeight = 88.0f;
    for (size_t row = 0; row < indices.size(); ++row)
    {
        size_t idx = indices[row];
        const PlayerRecord &rec = allRecords[idx];

        int minutes = static_cast<int>(rec.time / 60);
        int seconds = static_cast<int>(rec.time % 60);

        std::string time =
            (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
            (seconds < 10 ? "0" : "") + std::to_string(seconds);

        int rank = static_cast<int>(idx) + 1;
        std::string lineText =
            std::to_string(rank) + ". " + rec.name + " - " + time;

        float posX = (row < 5) ? 125.0f : 685.0f;
        float startY = 315.0f;
        if (row == 5)
            rowHeight = 88.0f;
        rowHeight--;

        Object *rowObj = CreateObject();
        rowObj->SetPosition(Vector2(posX, startY + (row % 5) * rowHeight));

        TextComponent *textComp =
            new TextComponent(28, "", TextAlignment::LEFT);
        textComp->SetOutline(2, {0, 0, 0, 255});
        rowObj->AddComponent(textComp);

        if (currId_ == rec.id)
            textComp->SetColor(255, 215, 0); // Gold for the current run
        else
            textComp->SetColor(255, 255, 255); // White for other rows

        textComp->setText(lineText);
    }
}

void WinScene::onKeyReleased(SDL_Keycode key)
{
    if (key == SDLK_ESCAPE)
        SwitchToScene(new StartScene());
}
