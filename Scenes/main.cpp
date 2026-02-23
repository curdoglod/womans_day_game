#include <iostream>
#include "../Core/engine.h"
#include "SceneDeclarations.h"


class Game : public Engine {

    void Init() override {
        SetWindowTitle("Game");
        SetWindowSize(400, 700);
        ChangeScene(new StartScene());
     }

};


int main(int argc, char* argv[]) 
{
    Game game; 
    game.Run();

    return 0;
}

