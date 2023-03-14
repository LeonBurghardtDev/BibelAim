#include "gui.h"
#include "hacks/hacks.h"
#include <iostream> 
#include <thread>
#include "globals.h"
#include "hacks/skinchanger/skinchanger.h"



int __stdcall wWinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    PWSTR arguments,
    int commandShow){

    /////////// load cheats

    // mem obj
    Memory memory = Memory("csgo.exe");

	globals::clientAdress = memory.GetModuleAddress("client.dll");
	globals::engineAdress = memory.GetModuleAddress("engine.dll");
	
    if (!globals::clientAdress || !globals::engineAdress)
        return EXIT_FAILURE;

  
    std::thread(hacks::VisualThread, memory).detach();
        
    // create gui
    gui::CreateHWindow("BibelAim");
    gui::CreateDevice();
    gui::CreateImGui();
    
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
