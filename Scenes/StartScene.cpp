#include "SceneDeclarations.h"


void StartScene::Init()
{
	SetWindowSize(1280, 720);
	// SwitchToScene(new WinScene());
	std::vector<unsigned char> bck_img = Engine::GetResourcesArchive()->GetFile("start_screen_v2.png");
	std::vector<unsigned char> input_img = Engine::GetResourcesArchive()->GetFile("input_field_image.png");
	std::vector<unsigned char> button_img = Engine::GetResourcesArchive()->GetFile("start_button.png");

	Object *background = CreateObject();
	background->AddComponent(new Image(bck_img));
	background->SetLayer(-100);
	Object *inputField = CreateObject();
	InputFieldComponent *inputComp = new InputFieldComponent(12, "Enter your name",
			0.0f, TextAlignment::CENTER);
	if (MainGameScene::GetPlayerName() != "")
		inputComp->setValue(MainGameScene::GetPlayerName());
	inputField->AddComponent(new Image(input_img));
	inputField->AddComponent(inputComp);
	// inputField->GetComponent<Image>()->SetSize(Vector2(200, 50));
	inputField->SetPosition(GetWindowSize() / 2
		- inputField->GetComponent<Image>()->GetSize() / 2);
	inputField->SetPosition(Vector2(406, 397));

	start_button = CreateObject();
    
	start_button->AddComponent(new ButtonComponent([this,
			inputComp]() { SwitchToScene(new MainGameScene(inputComp->getValue())); }));
	// start_button->AddComponent(new ButtonComponent([this, inputComp]()
	//                                                { SwitchToScene(new WinScene()); }));
	startBttn_image = start_button->GetComponent<Image>();
    startBttn_image->SetNewSprite(button_img);
    start_button->SetPosition(Vector2(426, 544));
}

void StartScene::onKeyReleased(SDL_Keycode key)
{
	if (key == SDLK_RETURN)
		start_button->GetComponent<ButtonComponent>()->Click();
}