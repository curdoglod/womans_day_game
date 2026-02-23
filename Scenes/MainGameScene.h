#pragma once

#include "../Core/SceneManager.h"
#include "../Core/components.h"
class PaddleComponent;
class RobotHand;

class MainGameScene : public SceneManager
{
public:
	MainGameScene(const std::string& name);

	void Init() override;

	void Update() override;
	~MainGameScene()
	{
	}
	void onKeyReleased(SDL_Keycode key) override;

	static std::string GetPlayerName() 
	{
		return playerName; 
	}

	static int currId;
private:
	void Generate_map(int count);
	void ShowTime();
	void StartHandTransition(int pressIndex);

	Sprite *bck = nullptr;
	Object *player = nullptr;
	PaddleComponent* playerComp; 
	Object *ball;
	Vector2 *dir_ball;
	std::vector<Object *> blocks;
	Object *start_button = nullptr;
	Object *scoreObj;
	Object *timerObj;
	TextComponent *scoreText;
	TextComponent *timerText;
	int score;
	int current_press_num = 0;
	Vector2 sizeOfPress;

	int count_presses;

	bool handTransitionActive = false;
	Object *handObj = nullptr;
	RobotHand *robotHand = nullptr;

	static std::string playerName;
	
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point last;
};
