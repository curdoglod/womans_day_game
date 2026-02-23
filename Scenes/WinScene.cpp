#include "SceneDeclarations.h"
#include "ScoreBoard.h"

void WinScene::Init()
{

    SetWindowSize(1280, 720);
    ScoreBoard *scoreBoard = new ScoreBoard("Assets/statistics.csv");
    // scoreBoard->addRecord("name", 100);
    // scoreBoard->saveToFile();
    std::vector<unsigned char> winImgData = Engine::GetResourcesArchive()->GetFile("win_screen.png");

    Object* win_screen = CreateObject(); 
    win_screen->AddComponent(new Image(winImgData)); 

 
    const auto &records = scoreBoard->getRecords();

    const std::string &playerName = MainGameScene::GetPlayerName();
    MainGameScene::currId = scoreBoard->getLastId(); 
    //std::cout<<MainGameScene::currId << std::endl; 

    std::vector<size_t> top10Idx;
    for (size_t i = 0; i < records.size() && i < 10; ++i)
        top10Idx.push_back(i);
    ShowTable(records, top10Idx, 0);

    std::vector<size_t> playerIdx;
    for (size_t i = 11; i < records.size(); ++i)
    {
        if (records[i].name == playerName)
            playerIdx.push_back(i);
    }

    ShowTable(records, playerIdx, 10);
}

void WinScene::ShowTable(const std::vector<PlayerRecord> &allRecords,
                         const std::vector<size_t> &indices,
                         int posY)
{
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

        float startY = GetWindowSize().y/2 -50;
        float rowHeight = 40.0f;
        Object *rowObj = CreateObject();
        rowObj->SetPosition(Vector2(GetWindowSize().x/2-100, startY + (row + posY) * rowHeight));

        TextComponent *textComp =
            new TextComponent(24, "", TextAlignment::LEFT);
        textComp->SetOutline(2, {0, 0, 0, 255}); // Add black outline for better contrast
        rowObj->AddComponent(textComp);

        if (rec.name == MainGameScene::GetPlayerName()) 
        {
            if(MainGameScene::currId == rec.id) 
                textComp->SetColor(255, 215, 0); // Gold for the current run
            else if(rank <= 10)
                textComp->SetColor(200, 150, 255); // Soft purple for player's other top 10 runs
            else 
            {
                lineText=""; 
                posY--; 
            }
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
