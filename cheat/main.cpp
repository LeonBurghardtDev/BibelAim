#include "gui.h"
#include "utils/memory.h"
#include <iostream> 
#include <thread>
#include "hacks/skinchanger/skinchanger.h"



int __stdcall wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR arguments,
    int commandShow)
{
    // create gui
    gui::CreateHWindow("BibelAim");
    gui::CreateDevice();
    gui::CreateImGui();

    /////////// load cheats

    // mem obj
    const auto memory = Memory("csgo.exe");

    // get module address
    const auto client = memory.GetModuleAddress("client.dll");
    const auto engine = memory.GetModuleAddress("engine.dll");

    if (!client || !engine)
        return EXIT_FAILURE;

    // create a thread for the `skinchanger_main` function
    std::thread skinchanger_thread(skinchanger_main, memory, client, engine);

    // detach the thread from the main thread
    skinchanger_thread.detach();
    
    /////////////
    
    while (gui::isRunning)
    {
        gui::BeginRender();
        gui::Render();
        gui::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    // destroy gui
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();

    return EXIT_SUCCESS;
}
