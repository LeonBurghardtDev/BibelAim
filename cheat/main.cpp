#include "gui.h"
#include "hacks/threads.h"
#include <iostream> 
#include <thread>
#include "globals.h"
#include "hacks/skinchanger/skinchanger.h"

void gui_main() {
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
}

INT APIENTRY WinMain(
    HINSTANCE instance,
    HINSTANCE previousInstance,
    LPSTR arguments,
    int commandShow){
    
    // mem obj
    Memory memory = Memory("csgo.exe");
    
    if (memory.GetProcessId() != 0) 
    {
       globals::csgoRunning = true;
    }
    
    globals::clientAdress = memory.GetModuleAddress("client.dll");
    globals::engineAdress = memory.GetModuleAddress("engine.dll");

    std::thread(gui_main).detach();
	// create threads 
    std::thread(threads::VisualThread, memory, instance, previousInstance, arguments, commandShow).detach();
	std::thread(threads::SkinChangerThread, memory).detach();
    std::thread(threads::MovementThread, memory).detach();
	std::thread(threads::AimbotThread, memory).detach();
	std::thread(threads::MiscThread, memory).detach();
   
    
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    

    return EXIT_SUCCESS;
}
