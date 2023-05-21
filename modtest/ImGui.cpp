#include "ASMPatcher.hpp"
#include "CustomImGui.h"
#include "HookSystem.h"
#include "IsaacRepentance.h"
#include "LogViewer.h"
#include "ConsoleMega.h"
#include "SigScan.h"

#include <Windows.h>
#include <format>
#include <gl/GL.h>
#include <sstream>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"

// this blogpost https://werwolv.net/blog/dll_injection was a big help, thanks werwolv!

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND window;
WNDPROC windowProc;
bool menuShown = false;
bool imguiConsoleEnabled = false;
static bool imguiInitialized = false;

LogViewer logViewer;
CustomImGui customImGui;

LRESULT CALLBACK windowProc_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Toggle the overlay using the delete key
    if (uMsg == WM_KEYDOWN && wParam == VK_DELETE) {
        menuShown = !menuShown;
        return false;
    }

    // If the overlay is shown, direct input to the overlay only
    if (menuShown) {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
        return true;
    }

    // Otherwise call the game's wndProc function
    return CallWindowProc(windowProc, hWnd, uMsg, wParam, lParam);
}

HOOK_GLOBAL(OpenGL::wglSwapBuffers, (HDC hdc)->bool, __stdcall)
{

    if (!imguiInitialized) {
        HWND window = WindowFromDC(hdc);
        windowProc = (WNDPROC)SetWindowLongPtr(window,
            GWLP_WNDPROC, (LONG_PTR)windowProc_hook);

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplOpenGL3_Init();
        ImGui::StyleColorsDark();
        ImGui::GetStyle().AntiAliasedFill = false;
        ImGui::GetStyle().AntiAliasedLines = false;
        ImGui::CaptureMouseFromApp();
        ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
        imguiInitialized = true;
        logViewer.AddLog("[REPENTOGON]", "Initialized Dear ImGui\n");
        printf("[REPENTOGON] Dear ImGui initialized! Any further logs can be seen in the in-game log viewer.\n");
    }

    if (menuShown) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Tools")) {
                if (ImGui::MenuItem("Console (UNIMPLEMENTED)", NULL, &imguiConsoleEnabled)) { }
                if (ImGui::MenuItem("Log Viewer", NULL, &logViewer.enabled)) { }
                if (ImGui::MenuItem("Custom ImGui", NULL, &customImGui.enabled)) { }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (logViewer.enabled) {
            logViewer.Draw();
        }
        if (customImGui.enabled) {
            customImGui.Draw();
        }

        if (imguiConsoleEnabled) {
            if (ImGui::Begin("Console", &imguiConsoleEnabled)) {
                std::deque<Console_HistoryEntry>* history = g_Game->GetConsole()->GetHistory();

                for (auto entry = history->rbegin(); entry != history->rend(); ++entry) {
                    int colorMap = entry->GetColorMap();

                    // The console stores colors like this, because we can't have nice things.
                    // g_colorDouble is used for other things but it isn't really evident what those things are yet, so this will have to do.
                    // Decomp shows it as 0 but it... clearly isn't, so whatever.
                    float red = ((colorMap >> 0x10 & 0xFF) + g_colorDouble) / 255;
                    float green = ((colorMap >> 8 & 0xFF) + g_colorDouble)/ 255;
                    float blue = ((colorMap & 0xFF) + g_colorDouble) / 255;
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(red, green, blue, 1));
                    ImGui::TextUnformatted(entry->_text.Get());
                    ImGui::PopStyleColor();
                }

                ImGui::End();
            }
        }

        ImGui::Render();

        // Draw the overlay
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    else if (logViewer.pinned) {

        // Show the log viewer and draw the overlay without hooking any input
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        logViewer.Draw();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    return super(hdc);
}

/*
 * LOG VIEWER PRINTS
 */

void __stdcall LogMessageCallback(const char* logMessage)
{
    logViewer.AddLog("[GAME]", logMessage);
};

HOOK_METHOD(Console, Print, (const IsaacString& text, unsigned int color, unsigned int unk)->void)
{
    logViewer.AddLog("[CONSOLE]", text.Get());
    super(text, color, unk);
}