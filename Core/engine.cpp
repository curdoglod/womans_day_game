#include "engine.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <algorithm>
#include "SceneManager.h"
#include "sprite.h"
#include <chrono>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif


struct Engine::Impl
{

    void preInit()
    {
#ifdef __EMSCRIPTEN__
        m_window = SDL_CreateWindow("New Window",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    1280, 720,
                                    SDL_WINDOW_SHOWN);

    // Force software renderer on web to bypass EGL swap-interval setup.
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
    SDL_SetHint(SDL_HINT_RENDER_VSYNC, "0");
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_SOFTWARE);
#else
        m_window = SDL_CreateWindow("New Window",
                                    SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    1280, 720,
                                    SDL_WINDOW_FULLSCREEN_DESKTOP);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
#endif

       SDL_RenderSetLogicalSize(m_renderer, 1280, 720);
    }
    void Tick(float deltaTime)
    {

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
#ifndef __EMSCRIPTEN__
            if (event.type == SDL_QUIT)
            {
                SDL_DestroyWindow(m_window);
                m_window = nullptr;
            }
#endif

            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST ||
                    event.window.event == SDL_WINDOWEVENT_HIDDEN ||
                    event.window.event == SDL_WINDOWEVENT_MINIMIZED)
                {
                    pausedByFocus = true;
                }
                else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED ||
                         event.window.event == SDL_WINDOWEVENT_SHOWN ||
                         event.window.event == SDL_WINDOWEVENT_RESTORED)
                {
                    pausedByFocus = false;
#ifdef __EMSCRIPTEN__
                    // Reset frame timer after tab/window restore to avoid a huge delta spike.
                    lastFrameTime = std::chrono::high_resolution_clock::now();
#endif
                }
            }

            if (currentScene != nullptr)
                currentScene->UpdateEvents(event);
        }

        SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
        SDL_RenderClear(m_renderer);

        float effectiveDeltaTime = pausedByFocus ? 0.0f : deltaTime;
        effectiveDeltaTime = std::min(effectiveDeltaTime, maxDeltaTime);

        if (currentScene != nullptr)
            currentScene->UpdateScene(effectiveDeltaTime);

        SDL_RenderPresent(m_renderer);
    }

#ifdef __EMSCRIPTEN__
    std::chrono::high_resolution_clock::time_point lastFrameTime;

    static void emMainLoop(void* arg)
    {
        Impl* self = static_cast<Impl*>(arg);
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> frameDuration = now - self->lastFrameTime;
        self->lastFrameTime = now;
        self->Tick(frameDuration.count() / 1000.0f);
    }
#endif

    int FPS = 60;
    bool pausedByFocus = false;
    const float maxDeltaTime = 1.0f / 20.0f;
    SDL_Window *m_window = nullptr;
    SDL_Renderer *m_renderer = nullptr;
    std::string nameWindow;

    SceneManager *currentScene = nullptr;
    SceneManager *prevScene = nullptr;

    static ArchiveUnpacker *DefaultArchive;
    static ArchiveUnpacker *ResourcesArchive;
};

ArchiveUnpacker *Engine::Impl::DefaultArchive = nullptr;
ArchiveUnpacker *Engine::Impl::ResourcesArchive = nullptr;

Engine::Engine() : impl(new Impl())
{
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    impl->DefaultArchive = new ArchiveUnpacker("DefaultAssets");
    impl->ResourcesArchive = new ArchiveUnpacker("Assets");
    impl->DefaultArchive->Unpack();
    impl->ResourcesArchive->Unpack();
    impl->FPS = 60;
}

void Engine::Run()
{
    impl->preInit();

    Init();

#ifdef __EMSCRIPTEN__
    impl->lastFrameTime = std::chrono::high_resolution_clock::now();
    emscripten_set_main_loop_arg(Impl::emMainLoop, impl, 0, 1);
#else
    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    int frameDelay = 1000 / impl->FPS;

    while (impl->m_window != nullptr)
    {
        auto frameStart = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> frameDuration = frameStart - lastFrameTime;
        lastFrameTime = frameStart;

        impl->Tick(frameDuration.count() / 1000.0f);

        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> elapsedTime = frameEnd - frameStart;
        int elapsedMS = static_cast<int>(elapsedTime.count());

        if (frameDelay > elapsedMS)
        {
            SDL_Delay(frameDelay - elapsedMS);
        }
    }
#endif
}

// void Engine::Run() {
//     preInit();
//     init();
//
//
//      int frameDelay = 1000 / FPS;
//
//     Uint32 frameStart;
//     int frameTime=0.0001;
//
//     while (m_window != nullptr) {
//         frameStart = SDL_GetTicks();
//         int frameDelay = 1000 / FPS;
//
//         Tick(frameTime / 1000.0f);
//
//
//         frameTime = SDL_GetTicks() - frameStart;
//
//         if (frameDelay > frameTime) {
//             SDL_Delay(frameDelay - frameTime);
//         }
//     }
// }

void Engine::ChangeScene(SceneManager *newScene)
{

    if (impl->currentScene)
    {
        if (impl->prevScene)
            delete impl->prevScene;
        impl->prevScene = impl->currentScene;
    }

    impl->currentScene = newScene;
    impl->currentScene->PreInit(this, impl->m_renderer, impl->m_window);
    impl->currentScene->Init();
}

ArchiveUnpacker *Engine::GetDefaultArchive()
{
    return Engine::Impl::DefaultArchive;
}

ArchiveUnpacker *Engine::GetResourcesArchive()
{
    return Engine::Impl::ResourcesArchive;
}

void Engine::SetFPS(const int &fps) { impl->FPS = fps; };

void Engine::SetWindowSize(const int &w, const int &h)
{
    SDL_SetWindowSize(impl->m_window, w, h);
}

void Engine::SetWindowTitle(const std::string &newTitle)
{
    SDL_SetWindowTitle(impl->m_window, newTitle.c_str());
}

void Engine::Quit()
{
    Sprite::ClearTextureCache();
    SDL_DestroyRenderer(impl->m_renderer);
    SDL_DestroyWindow(impl->m_window);
    IMG_Quit();
    SDL_Quit();
}

Engine::~Engine()
{
    Sprite::ClearTextureCache();
    SDL_DestroyRenderer(impl->m_renderer);
    SDL_DestroyWindow(impl->m_window);
    IMG_Quit();
    SDL_Quit();
    if (impl->prevScene)
        delete impl->prevScene;
    if (impl->currentScene)
        delete impl->currentScene;
    delete impl->DefaultArchive;
    delete impl->ResourcesArchive;
    if (impl)
        delete impl;
}
