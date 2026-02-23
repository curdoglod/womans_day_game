#include "SceneDeclarations.h"
#include "paddle.h"
#include "Press.h"
#include "RobotHand.h"
#include <chrono>
#include "ScoreBoard.h"

MainGameScene::MainGameScene(const std::string &name)
{
	playerName = name;
}

void MainGameScene::Init()
{
	score = 0;
	count_presses = 3;
	std::vector<unsigned char> ballImgData = Engine::GetResourcesArchive()->GetFile("ball.png");
	std::vector<unsigned char> blockImgData = Engine::GetResourcesArchive()->GetFile("block.png");
	std::vector<unsigned char> bckImgData = Engine::GetResourcesArchive()->GetFile("background.png");

	scoreText = new TextComponent(20, "Score:" + std::to_string(score));
	scoreObj = CreateObject();
	scoreObj->AddComponent(scoreText);
	scoreObj->SetPosition(Vector2(30, 40));
	scoreText->SetColor(255, 255, 255);
	scoreObj->SetLayer(100);

	player = CreateObject();
	playerComp = new PaddleComponent();
	player->AddComponent(playerComp);
	player->SetLayer(-2);

	for (int i = 0; i < count_presses * 3; i++)
	{
		Image *bckimg = new Image(bckImgData);
		Object *bck = CreateObject();
		bck->AddComponent(bckimg);
		bck->SetLayer(-500);
		bck->SetPosition(Vector2(bckimg->GetSize().x * i, 0));
	}
	start = std::chrono::steady_clock::now();
	timerText = new TextComponent(20, "0");
	timerText->SetColor(255, 255, 255);
	timerObj = CreateObject();
	timerObj->AddComponent(timerText);
	timerObj->SetPosition(Vector2(30, 15));

	timerObj->SetLayer(100);

	Generate_map(count_presses);
}

void MainGameScene::Generate_map(int count)
{
	for (int i = 0; i < count; i++)
	{

		int interval = static_cast<int>(GetWindowSize().y * 0.2f);
		float height_pos = (rand() % (2 * interval)) - interval;

		blocks.push_back(CreateObject());
		blocks[i]->SetPosition(Vector2(400 + 900 * i, height_pos));
		blocks[i]->AddComponent(new Press(player));
	}

	if(blocks.size() > 0)
	{
		blocks[0]->GetComponent<Press>()->SetCurrent(true);
		sizeOfPress = blocks[0]->GetComponent<Press>()->GetSizePress();
	}
}

void MainGameScene::Update()
{
	if (handTransitionActive)
	{
		if (robotHand && robotHand->IsCompleted())
		{
			robotHand->Cleanup();
			DeleteObject(handObj);
			handObj = nullptr;
			robotHand = nullptr;
			handTransitionActive = false;

			if (current_press_num < (int)blocks.size())
				blocks[current_press_num]->GetComponent<Press>()->SetCurrent(true);
		}
		scoreText->setText("Score: " + std::to_string(current_press_num));
		ShowTime();
		return;
	}

	for (int i = current_press_num; i < (int)blocks.size(); i++)
	{
		if (player->GetPosition().x >= blocks[i]->GetPosition().x + sizeOfPress.x)
		{
			blocks[i]->GetComponent<Press>()->SetCurrent(false);
			current_press_num = i + 1;

			if (i + 1 < (int)blocks.size())
			{
				StartHandTransition(i);
				break;
			}
		}
		else
		{
			break;
		}
	}

	scoreText->setText("Score: " + std::to_string(current_press_num));
	ShowTime();

	if (current_press_num == count_presses && blocks.size() > 0 &&
		player->GetPosition().x >= blocks[count_presses - 1]->GetPosition().x + sizeOfPress.x + 100)
	{
		ScoreBoard *scoreBoard = new ScoreBoard("Assets/statistics.csv");
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		auto elapsed =
			std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
		scoreBoard->addRecord(playerName, elapsed);
		scoreBoard->saveToFile();
		SwitchToScene(new WinScene());
	}
	playerComp->SetCar(current_press_num);
}

void MainGameScene::StartHandTransition(int pressIndex)
{
	float targetX = blocks[pressIndex + 1]->GetPosition().x - 400;
	handObj = CreateObject();
	robotHand = new RobotHand(player, targetX);
	handObj->AddComponent(robotHand);
	handTransitionActive = true;
}

void MainGameScene::ShowTime()
{
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

	auto elapsed =
		std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

	int minutes = static_cast<int>(elapsed / 60);
	int seconds = static_cast<int>(elapsed % 60);

	std::string time =
		(minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
		(seconds < 10 ? "0" : "") + std::to_string(seconds);

	timerText->setText(time);
}

void MainGameScene::onKeyReleased(SDL_Keycode key)
{
	if (key == SDLK_ESCAPE)
		SwitchToScene(new StartScene());
}

std::string MainGameScene::playerName = "";
int MainGameScene::currId = 0;