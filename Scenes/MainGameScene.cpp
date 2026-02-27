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
	count_presses = 6;
	std::vector<unsigned char> panelImgData = Engine::GetResourcesArchive()->GetFile("panel.png");
	std::vector<unsigned char> bckImgData = Engine::GetResourcesArchive()->GetFile("background.png");

	Object *panel = CreateObject();
	Image *panelImg = new Image(panelImgData);
	panel->AddComponent(panelImg);
	panel->SetLayer(1000);
	panel->SetPosition(Vector2(10, 10));
	panel->FixedCamera(true);

	scoreText = new TextComponent(20, "Level: " + std::to_string(score));
	scoreObj = CreateObject();
	scoreObj->AddComponent(scoreText);
	scoreObj->SetLayer(1001);
	scoreObj->SetPosition(Vector2(50, 100));
	scoreObj->FixedCamera(true);
	scoreText->SetColor(125, 220, 93);
	scoreText->SetFont(Engine::GetResourcesArchive()->GetFile("Orbitron.ttf"));

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
		bck->SetPosition(Vector2(bckimg->GetSize().x * i - 500, 0));
	}
	start = std::chrono::steady_clock::now();
	timerText = new TextComponent(20, "0");
	timerText->SetColor(125, 220, 93);

	timerObj = CreateObject();
	timerObj->AddComponent(timerText);
	timerObj->SetPosition(Vector2(121, 63));

	timerObj->SetLayer(1001);
	timerText->SetFont(Engine::GetResourcesArchive()->GetFile("Orbitron.ttf"));

	Generate_map(count_presses);
	StartIntroTransition();
}

void MainGameScene::Generate_map(int count)
{
	for (int i = 0; i < count; i++)
	{

		int interval = static_cast<int>(GetWindowSize().y * 0.2f);
		// std::cout << "Interval: " << interval << std::endl;
		float height_pos = (rand() % (2 * interval)) - interval;

		blocks.push_back(CreateObject());
		blocks[i]->SetPosition(Vector2(400 + 900 * i, height_pos));
		blocks[i]->AddComponent(new Press(player));
	}

	if (blocks.size() > 0)
	{
		sizeOfPress = blocks[0]->GetComponent<Press>()->GetSizePress();

		belt = CreateObject();
		belt->SetPosition(Vector2(blocks[blocks.size() - 1]->GetPosition().x + sizeOfPress.x + 700, GetWindowSize().y / 1.5f));
		std::vector<unsigned char>
			beltData = Engine::GetResourcesArchive()->GetFile("container_belt.png");
		ScrollingImage *scrollImg = new ScrollingImage(
			beltData,
			-329.0f,
			2300,
			50);
		belt->AddComponent(scrollImg);
		belt->SetLayer(-100);

		std::vector<unsigned char>
			beltPartData = Engine::GetResourcesArchive()->GetFile("part_belt.png");
		Object *belt_part = CreateObject();
		belt_part->SetPosition(Vector2(belt->GetPosition().x - 10, belt->GetPosition().y - 3));
		Image *beltPartImg = new Image(beltPartData);
		belt_part->AddComponent(beltPartImg);
		belt_part->SetLayer(-101);

		for (int i = 0; i < 20; i++)
		{
			Object *belt_support = CreateObject();
			belt_support->SetPosition(Vector2(blocks[blocks.size() - 1]->GetPosition().x + sizeOfPress.x + 700 + i * 360 - belt_part->GetSize().x + 5, GetWindowSize().y / 1.5f + 50));
			std::vector<unsigned char>
				beltSupportData = Engine::GetResourcesArchive()->GetFile("supports_belt.png");
			Image *beltSupportImg = new Image(beltSupportData);
			belt_support->AddComponent(beltSupportImg);
			belt_support->SetLayer(-99);
		}
	}
}

void MainGameScene::Update()
{

	if (waitingForWin)
	{
		auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
						   std::chrono::steady_clock::now() - winTime)
						   .count();
		if (elapsed >= 2400)
			SwitchToScene(new WinScene(playerName));
		return;
	}

	if (endTransitionActive)
	{
		if (robotHand && robotHand->IsCompleted())
		{
			robotHand->Cleanup();
			DeleteObject(handObj);
			handObj = nullptr;
			robotHand = nullptr;
			endTransitionActive = false;

			ScoreBoard *scoreBoard = new ScoreBoard(ScoreBoard::DEFAULT_PATH);
			std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
			auto elapsed =
				std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
			scoreBoard->addRecord(playerName, elapsed);
			scoreBoard->saveToFile();
			waitingForWin = true;
			winTime = std::chrono::steady_clock::now();
		}
		ShowTime();
		return;
	}

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

	ShowTime();

	if (current_press_num == count_presses && blocks.size() > 0 &&
		belt != nullptr && player->GetPosition().x >= belt->GetPosition().x - 800)
	{
		StartEndTransition();
	}
	playerComp->SetCar(current_press_num);
	char score_ch = '0' + current_press_num;
	scoreText->setText("Level: " + std::string(1, score_ch));
}

void MainGameScene::StartHandTransition(int pressIndex)
{
	float targetX = blocks[pressIndex + 1]->GetPosition().x - 350;
	handObj = CreateObject();
	robotHand = new RobotHand(player, targetX, GetWindowSize().y / 2, false);
	handObj->AddComponent(robotHand);
	handTransitionActive = true;
}

void MainGameScene::StartIntroTransition()
{
	if (blocks.empty())
		return;
	// Place player at carry height — hand appears already holding them
	float startX = blocks[0]->GetPosition().x - 600.0f;
	player->SetPosition(Vector2(startX, GetWindowSize().y / 2.0f + 50.0f));

	float targetX = blocks[0]->GetPosition().x - 300.0f;
	handObj = CreateObject();
	robotHand = new RobotHand(player, targetX, -1.0f, false, RobotHand::Phase::CARRY);
	handObj->AddComponent(robotHand);
	handTransitionActive = true;
}

void MainGameScene::StartEndTransition()
{
	if (belt == nullptr)
		return;
	// Carry the player onto the conveyor belt
	float targetX = belt->GetPosition().x + 100.0f;
	float beltSurfaceY = belt->GetPosition().y;
	handObj = CreateObject();
	robotHand = new RobotHand(player, targetX, beltSurfaceY, true);
	handObj->AddComponent(robotHand);
	endTransitionActive = true;
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
	scoreText->setText("Level: " + std::to_string(current_press_num));
}

void MainGameScene::onKeyReleased(SDL_Keycode key)
{
	if (key == SDLK_RETURN && handTransitionActive)
	{
		if (current_press_num < (int)blocks.size())
		{
			Press *nextPress = blocks[current_press_num]->GetComponent<Press>();
			if (nextPress)
				nextPress->SlowDown();
		}
		return;
	}

	if (key == SDLK_ESCAPE)
		SwitchToScene(new StartScene());
}

std::string MainGameScene::playerName = "";