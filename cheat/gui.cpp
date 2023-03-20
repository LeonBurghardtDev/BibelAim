#include "gui.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"
#include <string>
#include <Windows.h>
#include <dwmapi.h>
#include "globals.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);


long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (gui::device && wideParameter != SIZE_MINIMIZED)
		{
			gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
			gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
			gui::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(longParameter); // set click points
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::WIDTH &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;
	


	}

	return DefWindowProc(window, message, wideParameter, longParameter);
}


void gui::CreateHWindow(const char* windowName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		0,
		"class001",
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);
	
	if (window)
	{
		// Set the window to be always on top
		SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool gui::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void gui::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("../fonts/Ubuntu-Regular.ttf", 14.0f);

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT)
		{
			isRunning = !isRunning;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}


void gui::Render() noexcept
{

	// good -> https://www.youtube.com/watch?v=iOQ7ZrNQLuI
	
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"BibelAim",
		&isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoMove
	);
	
	if (globals::csgoRunning) {

		if (ImGui::BeginTabBar("##tabs")) {



			if (ImGui::BeginTabItem("Main")) {



				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Visuals")) {

				ImGui::Checkbox("glow", &globals::glow);
				if (globals::glow) {
					ImGui::ColorEdit4("glow color", globals::glowColor);
				}
				ImGui::Checkbox("radar", &globals::radar);

				ImGui::EndTabItem();


			}
			static bool recordingKey = false; // boolean to indicate if we are currently recording a key
			static int recordedKey = globals::triggerbot_key; // variable to store the recorded key code, initialized to the current triggerbot key
			if (ImGui::BeginTabItem("Aimbot")) {
				ImGui::Checkbox("Triggerbot", &globals::triggerbot);
		
				
				ImGui::Checkbox("Aimbot", &globals::aimbot);

				if (globals::aimbot) {
					ImGui::Checkbox("AimThroughObjects", &globals::aimbotThroughWalls);
					ImGui::Checkbox("AimForHead", &globals::aimForHead);
					ImGui::SliderFloat("Aimbot FOV", &globals::aimbotFov, 1.0f, 10.0f, "%.1f");

				}
				ImGui::Checkbox("RCS", &globals::rcs);


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Movement")) {
				ImGui::Checkbox("Bhop", &globals::bhop);


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("SkinChanger")) {
				ImGui::Checkbox("SkinChanger", &globals::skinchanger);

				if (globals::skinchanger) {

					if (ImGui::BeginTabBar("##tabs")) {
						if (ImGui::BeginTabItem("Weapons")) {

							ImGui::Text("Enter the skin id to change skin");
							if (ImGui::Button("Get Skin IDs")) {
								ImGui::OpenPopup("Open totalcsgo.com?");
							}

							if (ImGui::BeginPopupModal("Open totalcsgo.com?", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
								ImGui::Text("Are you sure you want to open totalcsgo.com?");
								ImGui::Separator();

								if (ImGui::Button("Open")) {
									ShellExecute(NULL, "open", "https://totalcsgo.com/skin-ids", NULL, NULL, SW_SHOWNORMAL);
									ImGui::CloseCurrentPopup();
								}

								ImGui::SameLine();

								if (ImGui::Button("Cancel")) {
									ImGui::CloseCurrentPopup();
								}

								ImGui::EndPopup();
							}

							if (ImGui::CollapsingHeader("Pistols")) {
								// Glock label and input field
								ImGui::Text("Glock");
								static int glock_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##glock", &glock_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(1, glock_skin_id); // set the skin id of the Glock to the user inputted value

								// USP label and input field
								ImGui::Text("USP");
								static int usp_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##usp", &usp_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(2, usp_skin_id); // set the skin id of the USP to the user inputted value

								// P2000 label and input field
								ImGui::Text("P2000");
								static int p2000_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##p2000", &p2000_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(3, p2000_skin_id); // set the skin id of the P2000 to the user inputted value

								// Five-SeveN label and input field
								ImGui::Text("Five-SeveN");
								static int five_seven_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##five-seven", &five_seven_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(4, five_seven_skin_id); // set the skin id of the Five-SeveN to the user inputted value

								// Tec-9 label and input field
								ImGui::Text("Tec-9");
								static int tec9_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##tec-9", &tec9_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(5, tec9_skin_id); // set the skin id of the Tec-9 to the user inputted value

								// CZ75-Auto label and input field
								ImGui::Text("CZ75-Auto");
								static int cz75_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##cz75", &cz75_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(6, cz75_skin_id); // set the skin id of the CZ75-Auto to the user inputted value

								// Desert Eagle label and input field
								ImGui::Text("Desert Eagle");
								static int deagle_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##deagle", &deagle_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(9, deagle_skin_id); // set the skin id of the Desert Eagle to the user inputted value
							}

							if (ImGui::CollapsingHeader("Rifles")) {
								// AK-47 label and input field
								ImGui::Text("AK-47");
								static int ak47_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##ak47", &ak47_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(7, ak47_skin_id); // set the skin id of the AK-47 to the user inputted value

								// M4A4 label and input field
								ImGui::Text("M4A4");
								static int m4a4_skin_id = 0;
								ImGui::InputInt("##m4a4", &m4a4_skin_id, 32);
								skins_ids::set_skin_id(16, m4a4_skin_id);

								// M4A1-S label and input field
								ImGui::Text("M4A1-S");
								static int m4a1s_skin_id = 0;
								ImGui::InputInt("##m4a1s", &m4a1s_skin_id, 32);
								skins_ids::set_skin_id(60, m4a1s_skin_id);

								// FAMAS label and input field
								ImGui::Text("FAMAS");
								static int famas_skin_id = 0;
								ImGui::InputInt("##famas", &famas_skin_id, 32);
								skins_ids::set_skin_id(10, famas_skin_id);

								// Galil AR label and input field
								ImGui::Text("Galil AR");
								static int galil_skin_id = 0;
								ImGui::InputInt("##galil", &galil_skin_id, 32);
								skins_ids::set_skin_id(13, galil_skin_id);
							}

							if (ImGui::CollapsingHeader("SMGs")) {

								// MP7 label and input field
								ImGui::Text("MP7");
								static int mp7_skin_id = 0;
								ImGui::InputInt("##mp7", &mp7_skin_id, 32);
								skins_ids::set_skin_id(17, mp7_skin_id);

								// MP9 label and input field
								ImGui::Text("MP9");
								static int mp9_skin_id = 0;
								ImGui::InputInt("##mp9", &mp9_skin_id, 32);
								skins_ids::set_skin_id(34, mp9_skin_id);

								// MAC-10 label and input field
								ImGui::Text("MAC-10");
								static int mac10_skin_id = 0;
								ImGui::InputInt("##mac10", &mac10_skin_id, 32);
								skins_ids::set_skin_id(17, mac10_skin_id);

								// UMP-45 label and input field
								ImGui::Text("UMP-45");
								static int ump45_skin_id = 0;
								ImGui::InputInt("##ump45", &ump45_skin_id, 32);
								skins_ids::set_skin_id(24, ump45_skin_id);

								// P90 label and input field
								ImGui::Text("P90");
								static int p90_skin_id = 0;
								ImGui::InputInt("##p90", &p90_skin_id, 32);
								skins_ids::set_skin_id(19, p90_skin_id);

								// PP-Bizon label and input field
								ImGui::Text("PP-Bizon");
								static int bizon_skin_id = 0;
								ImGui::InputInt("##bizon", &bizon_skin_id, 32);
								skins_ids::set_skin_id(26, bizon_skin_id);
							}


							if (ImGui::CollapsingHeader("Snipers")) {

								// AWP label and input field
								ImGui::Text("AWP");
								static int awp_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##awp", &awp_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(9, awp_skin_id); // set the skin id of the AWP to the user inputted value

								// SSG 08 label and input field
								ImGui::Text("SSG 08");
								static int ssg08_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##ssg08", &ssg08_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(40, ssg08_skin_id); // set the skin id of the SSG 08 to the user inputted value

								// SCAR-20 label and input field
								ImGui::Text("SCAR-20");
								static int scar20_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##scar20", &scar20_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(38, scar20_skin_id); // set the skin id of the SCAR-20 to the user inputted value

								// G3SG1 label and input field
								ImGui::Text("G3SG1");
								static int g3sg1_skin_id = 0; // initialize the skin id to 0
								ImGui::InputInt("##g3sg1", &g3sg1_skin_id, 32); // update the skin id when the user finishes inputting
								skins_ids::set_skin_id(11, g3sg1_skin_id); // set the skin id of the G3SG1 to the user inputted value

							}
							if (ImGui::CollapsingHeader("Shotguns")) {

								// Nova label and input field
								ImGui::Text("Nova");
								static int nova_skin_id = 0;
								ImGui::InputInt("##nova", &nova_skin_id, 32);
								skins_ids::set_skin_id(35, nova_skin_id);

								// XM1014 label and input field
								ImGui::Text("XM1014");
								static int xm1014_skin_id = 0;
								ImGui::InputInt("##xm1014", &xm1014_skin_id, 32);
								skins_ids::set_skin_id(25, xm1014_skin_id);

								// Sawed-Off label and input field
								ImGui::Text("Sawed-Off");
								static int sawed_off_skin_id = 0;
								ImGui::InputInt("##sawed-off", &sawed_off_skin_id, 32);
								skins_ids::set_skin_id(29, sawed_off_skin_id);

								// MAG-7 label and input field
								ImGui::Text("MAG-7");
								static int mag7_skin_id = 0;
								ImGui::InputInt("##mag7", &mag7_skin_id, 32);
								skins_ids::set_skin_id(27, mag7_skin_id);
							}

							if (ImGui::CollapsingHeader("Heavy (LMGs)")) {
								// M249 label and input field
								ImGui::Text("M249");
								static int m249_skin_id = 0;
								ImGui::InputInt("##m249", &m249_skin_id, 32);
								skins_ids::set_skin_id(14, m249_skin_id);

								// Negev label and input field
								ImGui::Text("Negev");
								static int negev_skin_id = 0;
								ImGui::InputInt("##negev", &negev_skin_id, 32);
								skins_ids::set_skin_id(13, negev_skin_id);

							}

							ImGui::EndTabItem();

						}
					}
				
					if (ImGui::BeginTabItem("Gloves")) {
							ImGui::Text("CT");
							static char ct_gloves[32] = "";
							ImGui::InputText("##ct_gloves", ct_gloves, 32);

							ImGui::Text("T");
							static char t_gloves[32] = "";
							ImGui::InputText("##t_gloves", t_gloves, 32);

							ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Knifes")) {
							// add content for Knifes tab here

							ImGui::EndTabItem();
						}

						ImGui::EndTabBar();
					}
				}
				ImGui::EndTabItem();
			}
			

			if (ImGui::BeginTabItem("Misc")) {

				ImGui::Checkbox("FOV Changer", &globals::fovChanger);
				if (globals::fovChanger) {
					ImGui::SliderFloat("##FOV", &globals::fovChangerValue, 0, 180);
				}
				
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Settings")) {



				ImGui::EndTabItem();
			}


			ImGui::EndTabBar();
		}
	else {
		ImGui::Text("Counter Strike Global Offensive is not currently running\nor the process (csgo.exe) could not be found.\nPlease start the game before running the cheat.");

	}

	ImGui::End();
}

