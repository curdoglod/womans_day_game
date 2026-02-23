#include "SceneDeclarations.h"

void StartScene::Init()
{

    SetWindowSize(1280, 720);
    // SwitchToScene(new WinScene());
    std::vector<unsigned char> bck_img = Engine::GetResourcesArchive()->GetFile("start_screen_image.png");
    Object *background = CreateObject();
    background->AddComponent(new Image(bck_img));
    background->SetLayer(-100);
    Object *inputField = CreateObject();
    InputFieldComponent *inputComp = new InputFieldComponent();
    if (MainGameScene::GetPlayerName() != "")
        inputComp->setValue(MainGameScene::GetPlayerName());
    inputField->AddComponent(inputComp);
    inputField->GetComponent<Image>()->SetSize(Vector2(200, 50));
    inputField->SetPosition(GetWindowSize() / 2 - inputField->GetComponent<Image>()->GetSize() / 2);
    inputField->SetPosition(inputField->GetPosition() + Vector2(0, 80));

    start_button = CreateObject();
    start_button->AddComponent(new ButtonComponent([this, inputComp]()
                                                   { SwitchToScene(new MainGameScene(inputComp->getValue())); }));
    // start_button->AddComponent(new ButtonComponent([this, inputComp]()
    //                                                { SwitchToScene(new WinScene()); }));
    startBttn_image = start_button->GetComponent<Image>();
    startBttn_image->SetSize(Vector2(150, 50));
    start_button->SetPosition(GetWindowSize() / 2 - startBttn_image->GetSize() / 2);
    start_button->SetPosition(start_button->GetPosition() + Vector2(0, 150));
    start_button->AddComponent(new TextComponent(20, "Start Game", TextAlignment::CENTER));
}

void StartScene::onKeyReleased(SDL_Keycode key)
{
    if (key == SDLK_RETURN)
        start_button->GetComponent<ButtonComponent>()->Click();
}