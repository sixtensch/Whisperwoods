#include "core.h"
#include "DebugGUI.h"

#include "Window.h"
#include "RenderCore.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

DebugGUI::DebugGUI(const RenderCore* renderCore, bool docking, bool viewports)
{
	LOG("Initializing GUI handler.");
	LOG_TRACE("Creating ImGui context.");
	ImGui::CreateContext();

	if (docking)
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	}

	if (viewports)
	{
		ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	}

	LOG_TRACE("Initializing ImGui Win32 and DX11 implementation.");
	renderCore->InitImGui();

	// Set the imgui input-overriding wndproc
	Window::LoadPrioritizedWndProc(ImGui_ImplWin32_WndProcHandler);
}

DebugGUI::~DebugGUI()
{
	LOG("Deinitializing ImGuiHandler.");

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();
}

void DebugGUI::BeginDraw()
{
	LOG_FRAMETRACE("Begin new ImGui frame.");
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void DebugGUI::EndDraw()
{
	LOG_FRAMETRACE("Render finished ImGui frame.");
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		LOG_FRAMETRACE("Update ImGui platform windows.");
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}