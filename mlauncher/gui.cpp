
#include "gui.h"
#include "login.h"
#include "mainmenu.h"
#include <string>




#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"

namespace gui {
	bool showText = true; //

}



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

	
	HRGN rgn = CreateRoundRectRgn(0, 0, WIDTH, HEIGHT, 15, 15);
	SetWindowRgn(window, rgn, TRUE);
	


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

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();
	ImGuiStyle& style = ImGui::GetStyle();
	
	ImVec4 darkGray = ImVec4(0.078f, 0.078f, 0.078f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = darkGray;

	
	


	style.Colors[ImGuiCol_TitleBg] = darkGray;
	style.Colors[ImGuiCol_TitleBgActive] = darkGray;
	style.Colors[ImGuiCol_TitleBgCollapsed] = darkGray;

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

	device->Clear(
		0, 0,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_RGBA(20, 20, 20, 0), // Alpha auf 0 setzen für Transparenz
		1.0f,
		0
	);

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
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin("", &isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoBackground
	);

	static char inputText[64] = "";
	static char inputPasswort[64] = "";

	
	

	ImGui::SetCursorPos(ImVec2(400, 80));
	if (gui::showText == true) {
		ImGui::Text("MLauncher");

	}
	

	if (!login::loggedIn) {
		// Container-Einstellungen
		ImVec2 containerSize = ImVec2(400, 220);
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 containerPos = ImVec2((windowSize.x - containerSize.x) * 0.5f,
			(windowSize.y - containerSize.y) * 0.5f);

		ImGui::SetCursorPos(containerPos);

		// Container abrunden und Hintergrundfarbe setzen
		ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);               // Ecken
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f)); // leicht helleres Grau

		ImGui::BeginChild("LoginContainer", containerSize, true, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar);

		// Input-Feld-Styling
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.15f, 1.0f));        // Hintergrund
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));      // Hover
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));   // Aktiv
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f); // Ecken

		

		// Username
		ImGui::SetCursorPos(ImVec2(70, 50));
		ImGui::Text("User");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(180);
		ImGui::InputText("##User", inputText, IM_ARRAYSIZE(inputText));

		// Passwort
		ImGui::SetCursorPos(ImVec2(43, 90));
		ImGui::Text("Passwort");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(180);
		ImGui::InputText("##Passwort", inputPasswort, IM_ARRAYSIZE(inputPasswort), ImGuiInputTextFlags_Password);



		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 7.0f);
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.35f, 0.20f, 0.55f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.45f, 0.30f, 0.70f, 1.0f)); // Hover
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.15f, 0.40f, 1.0f));
		
		// Register Button
		ImGui::SetCursorPos(ImVec2(123, 160));
		if (!login::IsDeviceRegistered()) {
			if (ImGui::Button("Register Device", ImVec2(150, 20))) {
				if (strlen(inputText) == 0 || strlen(inputPasswort) == 0) {
					login::loginFailed = true;
					login::loginFailedMessage = "Please enter username and password!";

					
				}

				else if (login::RegisterDevice(inputText, inputPasswort)) {
					login::loggedIn = true;
					login::loginFailed = false;
				}
				else {
					login::loginFailed = true;
					login::loginFailedMessage = "Device already registered";
				}
			}
		}
		else {
			// Hinweis anzeigen, dass Gerät schon registriert ist
			ImGui::TextColored(ImVec4(1, 1, 0, 1), "Device already registered!");
		}

		// Login Button
		ImGui::SetCursorPos(ImVec2(123, 130));
		if (ImGui::Button("Login", ImVec2(150, 20))) {
			if (strlen(inputText) == 0 || strlen(inputPasswort) == 0) {
				login::loginFailed = true;
				login::loginFailedMessage = "Please enter username and password!";
			}
			else if (!login::CheckCredentials(inputText, inputPasswort)) {
				login::loginFailed = true;
				if (!login::IsDeviceRegistered())
					login::loginFailedMessage = "Username or Password is wrong!";
				else
					login::loginFailedMessage = "Wrong username or password!";
			}
			else {
				login::loggedIn = true;
				login::loginFailed = false;
			}
		}

		// Fehlermeldung anzeigen
		if (login::loginFailed) {
			ImGui::SetCursorPos(ImVec2(120, 190));
			ImGui::TextColored(ImVec4(1, 0, 0, 1), login::loginFailedMessage.c_str());
		}


		

		ImGui::PopStyleColor(4); // Input-Feld Farben

		ImGui::EndChild();

		ImGui::PopStyleColor(); // Containerfarbe
		ImGui::PopStyleVar();   // Container Abrundung
	}
	else {
		mainmenu::Render();
	}

	ImGui::End();
}