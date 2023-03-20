#include "../../../gui.h"

#include "../../../../imgui/imgui.h"
#include "../../../../imgui/imgui_impl_dx11.h"
#include "../../../../imgui/imgui_impl_win32.h"
#include "../../../utils/memory.h"
#include <string>
#include <d3d11.h>
#include <Windows.h>
#include <dwmapi.h>
#include "../../../globals.h"
#include "../../../offsets/csgo.hpp"

struct Vector {
	Vector() noexcept
		: x(), y(), z() { }
	
	Vector(float x, float y, float z) noexcept
		: x(x), y(y), z(z) { }

	Vector& operator+(const Vector& v) noexcept
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	Vector& operator-(const Vector& v) noexcept
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}
	float x, y, z;
};

struct ViewMatrix {
	ViewMatrix() noexcept
		: data() { }
	
	float* operator[](int index) noexcept {
		return data[index];
	}
	
	const float* operator[](int index) const noexcept {
		return data[index];
	}

	float data[4][4];
};

static bool world_to_screen(const Vector& world, Vector& screen, const ViewMatrix& vm) noexcept {
	float w = vm[3][0] * world.x + vm[3][1] * world.y + vm[3][2] * world.z + vm[3][3];
	
	if (w < 0.001f) {
		return false;
	}
	
	const float x = world.x * vm[0][0] + world.y * vm[0][1] + world.z * vm[0][2] + vm[0][3];
	const float y = world.x * vm[1][0] + world.y * vm[1][1] + world.z * vm[1][2] + vm[1][3];

	w = 1.0f / w;
	float nx = x * w;
	float ny = y * w;

	const ImVec2 size = ImGui::GetIO().DisplaySize;
	
	screen.x = (size.x * 0.5f * nx) + (nx + size.x * 0.5f);
	screen.y = -(size.y * 0.5f * ny) + (ny + size.y * 0.5f);

	return true;
	
}


void esp(const Memory& memory) {
	const auto localPlayer = memory.Read<std::uintptr_t>(globals::clientAdress + offsets::dwLocalPlayer);

	if (!localPlayer)
		return;

	const auto local_team = memory.Read<int>(localPlayer + offsets::m_iTeamNum);
	const auto view_matrix = memory.Read<ViewMatrix>(globals::clientAdress + offsets::dwViewMatrix);

	for (int i = 1; i < 32; i++) {
		const auto player = memory.Read<std::uintptr_t>(globals::clientAdress + offsets::dwEntityList + i * 0x10);
		
		if (!player)
			return;
		
		if (memory.Read<bool>(player + offsets::m_bDormant))
			return;
		
		if (memory.Read<int>(player + offsets::m_iTeamNum) == local_team)
			return;
		
		if (memory.Read<int>(player + offsets::m_lifeState) != 0)
			return;

		
		const auto bones = memory.Read<DWORD>(player + offsets::m_dwBoneMatrix);

		if (!bones)
			return;
		
		Vector head_pos{
			memory.Read<float>(bones + 0x30 * 8 + 0x0C),
			memory.Read<float>(bones + 0x30 * 8 + 0x1C),
			memory.Read<float>(bones + 0x30 * 8 + 0x2C)
		};
		
		auto feet_pos = memory.Read<Vector>(player + offsets::m_vecOrigin);
		
		Vector top;
		Vector bottom;
		
		if (world_to_screen(head_pos + Vector(0,0,11.f), top, view_matrix) && world_to_screen(feet_pos + Vector(0, 0, 11.f), bottom, view_matrix)) {
			const auto height = bottom.y - top.y;
			const auto width = height * 0.35f;

			ImGui::GetBackgroundDrawList()->AddRect({ top.x - width, top.y }, { top.x + width, bottom.y }, ImColor(255, 0, 0, 255));
			
		
		}
		
	}
}



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);


LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return 0L;

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}

	return  DefWindowProc(window, message, wideParameter, longParameter);
}


INT APIENTRY CreateESPwindow(const Memory& memory,HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = hInstance;
	wc.lpszClassName = L"esp";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT,
		wc.lpszClassName,
		L"esp",
		WS_POPUP,
		0, 0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	
	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}
	
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Denominator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{};
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};



	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));

	if (back_buffer) {
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else {
		return 1;

	}

	ShowWindow(window, nCmdShow);
	UpdateWindow(window);


	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);



	while (globals::esp)
	{
		if (!device)
			return 1;
		if (!device_context)
			return 1;
		
			
		MSG msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				globals::esp = false;
		}

		if (!globals::esp)
			break;




		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();


		esp(memory);
		
		// rendering

		ImGui::Render();

		constexpr float color[4] = { 0.f, 0.f, 0.f, 0.f };
		
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);
		
		
		
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		
		
		
		swap_chain->Present(0U, 0U);

	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	if (render_target_view)
		render_target_view->Release();
	if (swap_chain)
		swap_chain->Release();
	if (device_context)
		device_context->Release();
	if (device)
		device->Release();

	DestroyWindow(window);
	UnregisterClassW(wc.lpszClassName, wc.hInstance);
	
	return 0;
}


void esp_main(const Memory& memory, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	CreateESPwindow(memory,hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}