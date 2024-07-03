#include <iostream>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include "Window.hpp"
#include "LevelLoader.hpp"

using namespace ModernBoy;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main(void){
    LevelLoader levelLoader("glob.json");
    auto wi=levelLoader.getWindowInfo();

    Window window(wi);
    window.update();

	return 0;
}
