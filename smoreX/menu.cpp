#include <Windows.h>
#include "offsets.h"
#include "config.h"
#include "hook.h"
#include "helper.h"
#include "menu.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_opengl2.h"
#include "ImGui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
WNDPROC oriWndProc = NULL;

typedef int(__cdecl* tSDL_ShowCursor)(int);
tSDL_ShowCursor SDL_ShowCursor = (tSDL_ShowCursor)offsets::showCursorAddr;

static const char* aimKeys[]{ "RMB", "LMB", "LCtrl" };
static const char* aimModes[]{ "Crosshair", "Distance" };
static int selectedMode = 0;

LRESULT __stdcall hWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	if (cfg::showMenu) {
		if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) { return true; }
		ImGuiIO &io = ImGui::GetIO();

		switch (uMsg) {
		case WM_LBUTTONDOWN: io.MouseDown[1] = !io.MouseDown[0]; return 0;
		case WM_RBUTTONDOWN: io.MouseDown[1] = !io.MouseDown[1]; return 0;
		case WM_MBUTTONDOWN: io.MouseDown[1] = !io.MouseDown[0]; return 0;
		case WM_MOUSEWHEEL: return 0;
		case WM_MOUSEMOVE:
			io.MousePos.x = (signed short)(lParam);
			io.MousePos.y = (signed short)(lParam >> 16);
			return 0;
		}
	}

	return CallWindowProc(oriWndProc, hWnd, uMsg, wParam, lParam);
}

void menu::create() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
}

void menu::init() {
	HWND hwnd = NULL;
	while(hwnd == NULL) { 
		hwnd = FindWindowA(NULL, "AssaultCube"); 
	}

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplOpenGL2_Init();

	{ // start of imgui theme thanks to https://github.com/CookiePLMonster
		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowRounding = 4.f;
		style->FramePadding = ImVec2(5, 5);
		style->FrameRounding = 4.0f;
		style->ItemSpacing = ImVec2(10, 6);
		style->ScrollbarSize = 10.0f;

		ImVec4* colors = style->Colors;
		colors[ImGuiCol_Text] = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.81f, 0.83f, 0.81f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.93f, 0.65f, 0.14f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_Tab] = ImVec4(0.51f, 0.36f, 0.15f, 1.00f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.91f, 0.64f, 0.13f, 1.00f);
		colors[ImGuiCol_TabActive] = ImVec4(0.78f, 0.55f, 0.21f, 1.00f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	} // end of imgui theme


	ImGui::SetNextWindowSize(ImVec2(1200, 1000), 0);
	oriWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hWndProc);
}

void menu::render() {
	if (selectedMode == 0) {
		cfg::bDistance = false;
		cfg::bCrosshair = true;
	}
	else { 
		cfg::bCrosshair = false;
		cfg::bDistance = true;
	}
	if (cfg::showMenu) {
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplOpenGL2_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		SDL_ShowCursor(1);

		ImGui::Begin("smoreX ~beta~ --offical_dev_build", &cfg::showMenu, ImGuiWindowFlags_None);
		ImGui::BeginTabBar("smoreX-TAB");

		if (ImGui::BeginTabItem("Aimbot")) {
			ImGui::Checkbox("Enable Aimbot", &cfg::bAimbot);
			if (ImGui::TreeNode("Aimbot Options")) {
				ImGui::Checkbox("Check if Visible", &cfg::bVisibleAim);
				ImGui::Text("Aimbot Key"); ImGui::Combo("##Aimbot Key", &cfg::selectedAimKey, aimKeys, IM_ARRAYSIZE(aimKeys));
				ImGui::Text("Aimbot Mode"); ImGui::Combo("##Aimbot Mode", &selectedMode, aimModes, IM_ARRAYSIZE(aimModes));
				ImGui::Checkbox("Smoothing Options", &cfg::bSmooth);
				ImGui::SliderFloat("Smoothing Value", &cfg::Smooth, 1, 10);
				ImGui::TreePop();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Visuals")) {
			ImGui::Checkbox("Enable Esp", &cfg::bEsp);
			if (ImGui::TreeNode("Esp Options")) {
				ImGui::Checkbox("Team Esp", &cfg::bTeamEsp);
				ImGui::Checkbox("Check if Visible", &cfg::bVisibleEsp);
				ImGui::Checkbox("Name", &cfg::bName);
				ImGui::Checkbox("Healthbar", &cfg::bHealthbar);
				ImGui::Checkbox("Snaplines", &cfg::bSnaplines);
				ImGui::TreePop();
			} ImGui::Separator();
			if (ImGui::TreeNode("Colors")) {
				ImGui::Text("Enemy Box Color:"); ImGui::SameLine(); ImGui::ColorEdit3("Enemy Box Color", (float*)&rgb::enemyBox, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::Text("Enemy Visible Color:"); ImGui::SameLine(); ImGui::ColorEdit3("Enemy Visible Color", (float*)&rgb::enemyBoxVisible, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::Text("Team Box Color:"); ImGui::SameLine(); ImGui::ColorEdit3("Team Box Color", (float*)&rgb::teamBox, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::Text("Name Color:"); ImGui::SameLine(); ImGui::ColorEdit3("Name Color", (float*)&rgb::text, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::Text("Snapline Color:"); ImGui::SameLine(); ImGui::ColorEdit3("Snapline Color", (float*)&rgb::snapline, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
				ImGui::TreePop();
			}

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Misc")) {
			ImGui::Text("smoreX created by spiffy#4049"); ImGui::SameLine(); if (ImGui::Button("Detach Dll")) { cfg::closeMenu = !cfg::closeMenu; } ImGui::SameLine(); IconHelpMarker("Note can cause crashes");

			ImGui::Separator();
			ImGui::Checkbox("Health Options", &cfg::bHealth); ImGui::SameLine(); IconHelpMarker("Note editing health/armor only works in singleplayer");
			if (ImGui::TreeNode("Health Values")) {
				ImGui::Text("Health"); ImGui::SliderInt("##Health", &cfg::Health, 0, 9999);
				ImGui::Text("Armor"); ImGui::SliderInt("##Armor", &cfg::Armor, 0, 9999);
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Checkbox("Ammo Options", &cfg::bAmmo);
			if (ImGui::TreeNode("Ammo Values")) {
				ImGui::Text("Loaded"); ImGui::SliderInt("##Loaded", &cfg::Ammo, 0, 9999);
				ImGui::Text("Reserve"); ImGui::SliderInt("##Reserve", &cfg::AmmoClip, 0, 9999);
				ImGui::Text("Grenades"); ImGui::SliderInt("##Grenades", &cfg::AmmoGrenade, 0, 9999);
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Checkbox("Damage Options", &cfg::bDamage);
			if (ImGui::TreeNode("Damage Value")) {
				ImGui::Text("Damage"); ImGui::SliderInt("##Damage", &cfg::Damage, 0, 9999);
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Checkbox("Sprint Options", &cfg::bSprint); ImGui::SameLine(); IconHelpMarker("Hold LSHIFT to sprint");
			if (ImGui::TreeNode("Sprint Value")) {
				ImGui::Text("Speed"); ImGui::SliderInt("##Speed", &cfg::SprintSpeed, 0, 25);
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Checkbox("Jump Options", &cfg::bJump); ImGui::SameLine(); IconHelpMarker("Press SPACE to jump");
			if (ImGui::TreeNode("Jump Value")) {
				ImGui::Text("Jump"); ImGui::SliderFloat("##Jump", &cfg::JumpHeight, 0.0f, 25.f);
				ImGui::TreePop();
			}

			ImGui::Separator();
			ImGui::Checkbox("Anti Recoil", &cfg::bRecoil);
			ImGui::Checkbox("Rapid Fire", &cfg::bAuto);
			ImGui::Checkbox("Triggerbot", &cfg::bTriggerbot);
			ImGui::Checkbox("Only Headshots", &cfg::bHeadshot);
			ImGui::Checkbox("Paperwalls", &cfg::bPaperwalls);
			ImGui::Checkbox("Fly Bypass", &cfg::bFly);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	} else { SDL_ShowCursor(0); }
}

void menu::stop() {
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	SetWindowLongPtr(FindWindowA(NULL, "AssaultCube"), GWLP_WNDPROC, (LONG_PTR)oriWndProc);
	SDL_ShowCursor(0);
}