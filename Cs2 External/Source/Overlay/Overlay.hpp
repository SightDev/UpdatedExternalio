#pragma once

// Libs 
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>
#include <d3d11.h>
#include <dwmapi.h>
#include <Globals/Globals.hpp>
#include <Functions/Console.hpp>
#include <Interfaces/Loader.hpp>
#include <Interfaces/Interfaces.hpp>
#include <Readers/Entity.hpp>
#include <Readers/Game.hpp>
#include <Readers/LocalPlayer.hpp>
#include <Hacks/Loop.hpp>
#include <Utilities/Threads.hpp>
#include <Overlay/Message.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK Window_Procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param))
    {
        return 0L;
    }

    if (message == WM_DESTROY)
    {
        PostQuitMessage(0);
        return 0L;
    }

    return DefWindowProc(window, message, w_param, l_param);
}

HWND InitializeWindow(HINSTANCE instance, int cmd_show)
{
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Window_Procedure;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = 0;
    wc.hCursor = 0;
    wc.hbrBackground = 0;
    wc.lpszMenuName = 0;
    wc.lpszClassName = L"External Overlay Class";
    wc.hIconSm = 0;

    RegisterClassExW(&wc);

    HWND window = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
        wc.lpszClassName,
        L"External Overlay",
        WS_POPUP,
        0, 0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );   

    SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_COLORKEY);

    RECT client_area{}, window_area{};
    GetClientRect(window, &client_area);
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
    ShowWindow(window, cmd_show);
    UpdateWindow(window);
    return window;
}

bool InitializeDirectX(HWND window, ID3D11Device** device, ID3D11DeviceContext** device_context, IDXGISwapChain** swap_chain, ID3D11RenderTargetView** render_target_view)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferDesc.RefreshRate.Numerator = 60U;
    sd.BufferDesc.RefreshRate.Denominator = 1U;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.SampleDesc.Count = 1U;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2U;
    sd.OutputWindow = window;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL level;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0U,
        levels,
        2U,
        D3D11_SDK_VERSION,
        &sd,
        swap_chain,
        device,
        &level,
        device_context
    );

    if (FAILED(hr)) return false;

    ID3D11Texture2D* back_buffer = nullptr;
    (*swap_chain)->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
    if (back_buffer)
    {
        (*device)->CreateRenderTargetView(back_buffer, nullptr, render_target_view);
        back_buffer->Release();
    }
    else return false;

    return true;
}

VOID InitializeImGui(HWND window, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX11_Init(device, device_context);
    ImGui_Loader::LoadFont();   
}

VOID CleanUp(IDXGISwapChain* swap_chain, ID3D11Device* device, ID3D11DeviceContext* device_context, ID3D11RenderTargetView* render_target_view, HWND window, WNDCLASSEXW wc)
{
    Threads::JoinThreads();
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    if (render_target_view) render_target_view->Release();
    if (swap_chain) swap_chain->Release();
    if (device_context) device_context->Release();
    if (device) device->Release();

    DestroyWindow(window);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

VOID ShowTitle()
{
    vector<string> CheatTitle = {
            "We",
            "Don't",
            "Speak",
            "Russain, (offsets updated)",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
            "",
    
    };

    for (const auto& line : CheatTitle) {
        SetConsoleTextColor(RED);
        std::cout << line << std::endl;
    }

    if (CS2::ProcID > 0 && CS2::ClientDll > 0)
    {
        SetConsoleTextColor(YELLOW);
        cout << '\n';
        cout << " [+] CS2 Process ID : " << CS2::ProcID << '\n';
        cout << '\n';
        cout << " [+] Retrieving Modules\n";
        Sleep(500);
        cout << '\n';
        cout << "  => Client Address : " << CS2::ClientDll << '\n';
        cout << "  => Engine2 Address : " << CS2::Engine2Dll << '\n';
        cout << "  => Inputsystem Address : " << CS2::InputsystemDll << '\n';
        cout << "  => Matchmaking Address : " << CS2::MatchMakingDll << '\n';
        cout << "  => Soundsystem Address : " << CS2::SoundsystemDll << '\n';
        cout << "  => Server Address : " << CS2::ServerDll << '\n';
        cout << '\n';
        cout << " [+] Updating Local Player\n";
        Sleep(500);
        cout << " [+] Updating Entities\n";
        Sleep(500);
        cout << " [+] Updating Game Vars\n";
        cout << '\n';
        cout << " [+] External Loaded! \n";
       // CreateLogNotification("Memory", "CONERR$");   
    }
    else
    {
        SetConsoleTextColor(YELLOW);
        cout << '\n';
        cout << " [+] CS2 Process ID : " << CS2::ProcID << '\n';
        cout << '\n';
        cout << " [+] Retrieving Modules\n";
        Sleep(1000);
        cout << '\n';
        cout << "  => Client Address : " << CS2::ClientDll << '\n';
        cout << "  => Engine2 Address : " << CS2::Engine2Dll << '\n';
        cout << "  => Inputsystem Address : " << CS2::InputsystemDll << '\n';
        cout << "  => Matchmaking Address : " << CS2::MatchMakingDll << '\n';
        cout << "  => Soundsystem Address : " << CS2::SoundsystemDll << '\n';
        cout << "  => Server Address : " << CS2::ServerDll << '\n';
        cout << '\n';
        cout << "  Cheat Failed To Attach! \n";
        cout << " [-] yeah, you need to update the offsets for client.dll in globals\n";
        //CreateLogNotification("Memory", "CONERR$");
    }
}
