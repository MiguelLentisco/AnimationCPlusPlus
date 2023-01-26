// Constants to reduce code for windows
#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <iostream>
#include <windows.h>
#include <glad/glad.h>

#include "Application/AdditiveBlendApp.h"
#include "Application/AllBlendApp.h"
#include "Application/BasicRenderApp.h"
#include "Application/CCDIKApp.h"
#include "Application/FABRIKApp.h"
#include "Application/InterpolationsApp.h"
#include "Application/SimpleBlendApp.h"
#include "Application/SkeletalAnimationApp.h"
#include "Application/SkeletalMeshAnimationApp.h"

// Forward declaration
int WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Link to OpenGL32.lib (could be done on project settings)
#if _DEBUG
#pragma comment(linker, "/subsystem:console")
int main(int argc, const char** argv)
{
    return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLineA(), SW_SHOWDEFAULT);
}
#else
#pragma comment(linker, "/subsystem:windows")
#endif
#pragma comment(lib, "opengl32.lib")

// From wglext.h: declaration constants and function "wglCreateContextAttribsARB"
#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092
#define WGL_CONTEXT_FLAGS_ARB             0x2094
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001
#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

// From wgl.h: functions "wglGetExtensionStringEXT", "wglSwapIntervalEXT" and "wglGetSwapIntervalEXT"
typedef const char *(WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC) (void);
typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
typedef int (WINAPI * PFNWGLGETSWAPINTERVALEXTPROC) (void);

// Pointers to application and VAO (Vertex Array Object)
Application* gApplication = 0;
GLuint gVertexArrayObject = 0;

// ---------------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    // Standard window definition
    gApplication = new FABRIKApp();
    
    WNDCLASSEX wndClass;
    wndClass.cbSize = sizeof(WNDCLASSEX);
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
    wndClass.lpszMenuName = 0;
    wndClass.lpszClassName = TEXT("Win32 Game Window");
    RegisterClassEx(&wndClass);

    // Adjust window's size and position to center
    const int screenWidthHalf = GetSystemMetrics(SM_CXSCREEN) / 2;
    const int screenHeightHalf = GetSystemMetrics(SM_CYSCREEN) / 2;
    static constexpr int CLIENT_WIDTH_HALF = 800 / 2;
    static constexpr int CLIENT_HEIGHT_HALF = 600 / 2;
    RECT windowRect;
    SetRect(&windowRect, screenWidthHalf - CLIENT_WIDTH_HALF, screenHeightHalf - CLIENT_HEIGHT_HALF,
        screenWidthHalf + CLIENT_WIDTH_HALF, screenHeightHalf + CLIENT_HEIGHT_HALF);

    // Minimize or maximized but not resized
    DWORD style = (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX); // | WS_THICKFRAME to resize
    AdjustWindowRectEx(&windowRect, style, FALSE, 0);

    // Create actual window
    const HWND hwnd = CreateWindowEx(0, wndClass.lpszClassName, TEXT("GameWindow"), style, windowRect.left,
                                     windowRect.top, windowRect.right - windowRect.left,
                                     windowRect.bottom - windowRect.top, nullptr, nullptr, hInstance, szCmdLine);
    const HDC hdc = GetDC(hwnd);

    // Find and apply correct pixel format
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 32;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;
    const int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    // Create temporary OpenGL context
    const HGLRC tempRC = wglCreateContext(hdc);
    wglMakeCurrent(hdc, tempRC);
    auto wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) wglGetProcAddress("wglCreateContextAttribsARB");

    // Create OpengGL4.6 Core context profile, bind it and delete legacy context
    const int attribList[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        WGL_CONTEXT_FLAGS_ARB, 0,
        WGL_CONTEXT_PROFILE_MASK_ARB,
        WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0,
    };
    const HGLRC hglrc = wglCreateContextAttribsARB(hdc, nullptr, attribList);
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(tempRC);
    wglMakeCurrent(hdc, hglrc);

    // Load GL
    if (!gladLoadGL())
    {
        std::cout << "Could not initialize GLAD" << std::endl;
        return 1;
    }
    std::cout << "OpenGL Version " << GLVersion.major << "." << GLVersion.minor << std::endl;

    // Initialize OpenGL context and check vsync extension
    const auto wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress("wglGetExtensionsStringEXT");
    const bool swapControlSupported = strstr(wglGetExtensionsStringEXT(), "WGL_EXT_swap_control") != nullptr;

    // Load extension and turn on vSync
    int vSync = 0;
    if (swapControlSupported)
    {
        const auto wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress("wglSwapIntervalEXT");
        const auto wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC) wglGetProcAddress("wglGetSwapIntervalEXT");
        if (wglSwapIntervalEXT(1))
        {
            std::cout << "Enabled vsync" << std::endl;
            vSync = wglGetSwapIntervalEXT();
        }
        else
        {
            std::cout << "Could not enable vSync" << std::endl; 
        }
    }
    else
    {
        std::cout << "WGL_EXT_swap_control not supported" << std::endl;
    }

    // Create and bind VAO, won't be destroyed until the window is
    glGenVertexArrays(1, &gVertexArrayObject);
    glBindVertexArray(gVertexArrayObject);

    // Display current window and initialize app
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    gApplication->Initialize();

    // Game loop
    DWORD lastTick = GetTickCount();
    MSG msg;
    while (true)
    {
        // Handle window events
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Avoid same tick
        const DWORD thisTick = GetTickCount();
        if (thisTick == lastTick)
        {
            continue;
        }

        // Update tick
        if (gApplication != nullptr)
        {
            const float dt = static_cast<float>(thisTick - lastTick) * .001f;
            lastTick = thisTick;
            gApplication->Update(dt);
        }

        // Render
        if (gApplication != nullptr)
        {
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            const LONG clientWidth = clientRect.right - clientRect.left;
            const LONG clientHeight = clientRect.bottom - clientRect.top;
            
            glViewport(0, 0, clientWidth, clientHeight);
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glPointSize(5.f);
            glBindVertexArray(gVertexArrayObject);

            glClearColor(.5f, .6f, .7f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            float aspect = static_cast<float>(clientWidth) / static_cast<float>(clientHeight);
            gApplication->Render(aspect); 
        }

        // Swap buffers and finish if vSync is enabled
        if (gApplication != nullptr)
        {
            SwapBuffers(hdc);
            if (vSync != 0)
            {
                glFinish();
            }
        }
    }

    if (gApplication != nullptr)
    {
        std::cout << "Expected application to be nullptr on exit" << std::endl;
        delete gApplication;
        gApplication = nullptr;
    }

    return static_cast<int>(msg.wParam);
    
} // WinMain

// ---------------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    switch (iMsg)
    {
        // Shutdown app and destroy window
        case WM_CLOSE:
        {
            if (gApplication == nullptr)
            {
                std::cout << "Already shut down!" << std::endl;
                break;
            }

            gApplication->Shutdown();
            delete gApplication;
            gApplication = nullptr;
            DestroyWindow(hwnd);
            break;
        }
        // Release OpenGL
        case WM_DESTROY:
        {
            if (gVertexArrayObject == 0)
            {
                std::cout << "Multiple destroy messages" << std::endl;
                break;
            }

            const HDC hdc = GetDC(hwnd);
            const HGLRC hglrc = wglGetCurrentContext();

            glBindVertexArray(0);
            glDeleteVertexArrays(1, &gVertexArrayObject);
            gVertexArrayObject = 0;

            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(hglrc);
            ReleaseDC(hwnd, hdc);

            PostQuitMessage(0);
            break;
        }
        // Ignore
        case WM_PAINT:
        case WM_ERASEBKGND:
            return 0;
        default:
            break;
    }

    return DefWindowProc(hwnd, iMsg, wParam, lParam);
    
} // WndProc

// ---------------------------------------------------------------------------------------------------------------------