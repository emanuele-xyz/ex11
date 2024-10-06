#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define ex11_Unused(x) (void)(x)
#define ex11_Stringify(x) ex11_Str(x)
#define ex11_Str(x) #x

#if defined(_DEBUG)
#define ex11_Crash(msg) __debugbreak()
#else
#define ex11_Crash(msg) do { MessageBox(0, msg, L"Crash!", MB_OK | MB_ICONERROR); *(int*)(0) = 0; } while (0)
#endif
#define ex11_Check(p) do { if (!(p)) { ex11_Crash(__FILE__ L"(" ex11_Stringify(__LINE__) L"):" __FUNCTION__ ": " #p); } } while (0)

#define EX11_WINDOW_CLASS_NAME L"ex11_window_class"
#define EX11_WINDOW_NAME L"ex11"
#define EX11_WINDOW_CLIENT_WIDTH 1280
#define EX11_WINDOW_CLIENT_HEIGHT 720

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef char      i8;
typedef short     i16;
typedef int       i32;
typedef long long i64;

typedef char      b8;
typedef short     b16;
typedef int       b32;
typedef long long b64;

_Static_assert(sizeof(u8) == 1, "sizeof(u8) must be 1");
_Static_assert(sizeof(u16) == 2, "sizeof(u16) must be 2");
_Static_assert(sizeof(u32) == 4, "sizeof(u32) must be 4");
_Static_assert(sizeof(u64) == 8, "sizeof(u64) must be 8");

_Static_assert(sizeof(i8) == 1, "sizeof(i8) must be 1");
_Static_assert(sizeof(i16) == 2, "sizeof(i16) must be 2");
_Static_assert(sizeof(i32) == 4, "sizeof(i32) must be 4");
_Static_assert(sizeof(i64) == 8, "sizeof(i64) must be 8");

_Static_assert(sizeof(b8) == 1, "sizeof(b8) must be 1");
_Static_assert(sizeof(b16) == 2, "sizeof(b16) must be 2");
_Static_assert(sizeof(b32) == 4, "sizeof(b32) must be 4");
_Static_assert(sizeof(b64) == 8, "sizeof(b64) must be 8");

static HWND s_window = 0;
static b32 s_is_running = 1;

static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    switch (msg)
    {
    case WM_CLOSE:
    {
        s_is_running = 0;
    } break;
    case WM_KEYDOWN:
    {
        s_is_running = (wparam != VK_ESCAPE);
    } break;
    default:
    {
        result = DefWindowProcW(hwnd, msg, wparam, lparam);
    }
    }
    return result;
}

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hisnt_prev, PSTR cmdline, int cmdshow)
{
    ex11_Unused(hinst);
    ex11_Unused(hisnt_prev);
    ex11_Unused(cmdline);
    ex11_Unused(cmdshow);

    // NOTE: register window class
    {
        WNDCLASSEX wndclass = { 0 };
        wndclass.cbSize = sizeof(wndclass);
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = &WindowProcedure;
        //wndclass.cbClsExtra = ;
        //wndclass.cbWndExtra = ;
        wndclass.hInstance = hinst;
        wndclass.hIcon = LoadIconW(0, IDI_APPLICATION);
        wndclass.hCursor = LoadCursorW(0, IDC_ARROW);
        //wndclass.hbrBackground = ;
        //wndclass.lpszMenuName = ;
        wndclass.lpszClassName = EX11_WINDOW_CLASS_NAME;
        wndclass.hIconSm = LoadIconW(0, IDI_APPLICATION);

        ex11_Check(RegisterClassEx(&wndclass));
    }

    // NOTE: create window
    {
        RECT rect = { 0, 0, EX11_WINDOW_CLIENT_WIDTH, EX11_WINDOW_CLIENT_HEIGHT };
        ex11_Check(AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, 0, WS_EX_OVERLAPPEDWINDOW));
        LONG window_width = rect.right - rect.left;
        LONG window_height = rect.bottom - rect.top;
        s_window = CreateWindowEx(
            WS_EX_OVERLAPPEDWINDOW,
            EX11_WINDOW_CLASS_NAME,
            EX11_WINDOW_NAME,
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            CW_USEDEFAULT, CW_USEDEFAULT,
            window_width, window_height,
            0, 0, hinst, 0
        );
        ex11_Check(s_window);
    }

    while (s_is_running)
    {
        MSG msg = { 0 };
        while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}
