#include "SceneDeclarations.h"
#include "ScoreBoard.h"

void WinScene::Init()
{
    std::vector<unsigned char> bck_screen_img = Engine::GetResourcesArchive()->GetFile("bckscreen.png");
    Object *bck_screen = CreateObject();
    bck_screen->SetLayer(-1000);
    bck_screen->AddComponent(new Image(bck_screen_img));
    SetWindowSize(1280, 720);
    ScoreBoard *scoreBoard = new ScoreBoard(ScoreBoard::DEFAULT_PATH);
    std::vector<unsigned char> winImgData = Engine::GetResourcesArchive()->GetFile("win_screen_v2.png");

    Object *win_screen = CreateObject();
    win_screen->AddComponent(new Image(winImgData));

    const auto &records = scoreBoard->getRecords();
    currId_ = scoreBoard->getLastId();

    // Find and show current run in the gold banner
    for (size_t i = 0; i < records.size(); ++i)
    {
        if (records[i].id == currId_)
        {
            int minutes = static_cast<int>(records[i].time / 60);
            int seconds = static_cast<int>(records[i].time % 60);

            std::string time = (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + std::to_string(seconds);
            int rank = static_cast<int>(i) + 1;
            std::string lineText = std::to_string(rank) + ". " + records[i].name + " - " + time;

            Object *bannerObj = CreateObject();
            bannerObj->SetPosition(Vector2(GetWindowSize().x / 2 + 40, 160));
            TextComponent *textComp = new TextComponent(30, "", TextAlignment::CENTER);
            textComp->SetOutline(2, {0, 0, 0, 255});
            textComp->SetColor(255, 255, 255);
            textComp->setText(lineText);
            bannerObj->AddComponent(textComp);
            break;
        }
    }

    std::vector<size_t> top10Idx;
    for (size_t i = 0; i < records.size() && i < 10; ++i)
        top10Idx.push_back(i);
    ShowTable(records, top10Idx, 0);
    delete scoreBoard;
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

        if (rec.name == playerName_)
        {
            if (currId_ == rec.id)
                textComp->SetColor(255, 215, 0); // Gold for the current run
            else
                textComp->SetColor(200, 150, 255); // Soft purple for player's other top 10 runs
        }
        else
            textComp->SetColor(255, 255, 255); // White for other players

        textComp->setText(lineText);
    }
}

void WinScene::onKeyReleased(SDL_Keycode key)
{
    if (key == SDLK_ESCAPE)
        SwitchToScene(new StartScene());
}
