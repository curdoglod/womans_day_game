#include <iostream>
#include "../Core/engine.h"
#include "SceneDeclarations.h"


class Game : public Engine {

    void Init() override {
        SetWindowTitle("Game");
        SetWindowSize(1280, 720);
        ChangeScene(new StartScene());
        //ChangeScene(new WinScene("00:00"));
     }

};


int main(int argc, char* argv[]) 
{
    Game game; 
    game.Run();

    return 0;
}

