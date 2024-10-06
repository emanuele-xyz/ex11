#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning(push)
#pragma warning(disable : 4820) // NOTE: padding bytes added to the end of a struct
#include <d3d11.h>
#include <dxgi1_3.h>
#include <dxgidebug.h>
#pragma warning(pop)

#pragma comment(lib, "d3d11")
#pragma comment (lib, "dxgi")
#pragma comment (lib, "dxguid")

#define ex11_Unused(x) (void)(x)
#define ex11_Stringify(x) ex11_Str(x)
#define ex11_Str(x) #x
#define ex11_CountOf(a) (sizeof(a) / sizeof(*(a)))

#if defined(_DEBUG)
#define ex11_Crash(msg) __debugbreak()
#else
#define ex11_Crash(msg) do { MessageBox(0, msg, L"Crash!", MB_OK | MB_ICONERROR); *(int*)(0) = 0; } while (0)
#endif
#define ex11_Check(p) do { if (!(p)) { ex11_Crash(__FILE__ L"(" ex11_Stringify(__LINE__) L"):" __FUNCTION__ ": " #p); } } while (0)
#define ex11_CheckHR(p) do { if (!SUCCEEDED(p)) { ex11_Crash(__FILE__ L"(" ex11_Stringify(__LINE__) L"):" __FUNCTION__ ": " #p); } } while (0)

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

static b32 s_is_running = 1;
static b32 s_use_vsync = 0;
static b32 s_did_resize = 0;
static HWND s_window = 0;
static ID3D11Device *s_gfx_device = 0;
static ID3D11DeviceContext *s_gfx_context = 0;
static IDXGISwapChain1 *s_swap_chain = 0;
static ID3D11RenderTargetView *s_back_buffer_rtv = 0;

static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    LRESULT result = 0;
    switch (msg)
    {
    case WM_CLOSE:
    {
        s_is_running = 0;
        result = DefWindowProcW(hwnd, msg, wparam, lparam);
    } break;
    case WM_KEYDOWN:
    {
        s_is_running = (wparam != VK_ESCAPE);
        result = DefWindowProcW(hwnd, msg, wparam, lparam);
    } break;
    case WM_SIZE:
    {
        s_did_resize = 1;
        result = DefWindowProcW(hwnd, msg, wparam, lparam);
    } break;
    default:
    {
        result = DefWindowProcW(hwnd, msg, wparam, lparam);
    } break;
    }
    return result;
}

int APIENTRY WinMain(HINSTANCE hinst, HINSTANCE hisnt_prev, PSTR cmdline, int cmdshow)
{
    ex11_Unused(hisnt_prev);
    ex11_Unused(cmdline);
    ex11_Unused(cmdshow);

    ex11_Check(SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE));

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

    // NOTE: d3d11 initialization
    {
        // NOTE: create device and context
        {
            UINT flags = 0;
            #if defined(_DEBUG)
            flags |= D3D11_CREATE_DEVICE_DEBUG;
            #endif
            D3D_FEATURE_LEVEL requested_lvl = D3D_FEATURE_LEVEL_11_0;
            D3D_FEATURE_LEVEL supported_lvl = 0;
            ex11_CheckHR(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, flags, &requested_lvl, 1, D3D11_SDK_VERSION, &s_gfx_device, &supported_lvl, &s_gfx_context));
            ex11_Check(requested_lvl == supported_lvl);
        }

        // NOTE: setup debug layer
        #if defined(_DEBUG)
        {
            {
                ID3D11Debug *dbg = 0;
                ex11_CheckHR(s_gfx_device->lpVtbl->QueryInterface(s_gfx_device, &IID_ID3D11Debug, &dbg));
                ID3D11InfoQueue *queue = 0;
                ex11_CheckHR(dbg->lpVtbl->QueryInterface(dbg, &IID_ID3D11InfoQueue, &queue));
                queue->lpVtbl->SetBreakOnSeverity(queue, D3D11_MESSAGE_SEVERITY_CORRUPTION, 1);
                queue->lpVtbl->SetBreakOnSeverity(queue, D3D11_MESSAGE_SEVERITY_ERROR, 1);
                queue->lpVtbl->Release(queue);
                dbg->lpVtbl->Release(dbg);
            }
            {
                IDXGIInfoQueue *queue = 0;
                ex11_CheckHR(DXGIGetDebugInterface1(0, &IID_IDXGIInfoQueue, &queue));
                ex11_CheckHR(queue->lpVtbl->SetBreakOnSeverity(queue, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, 1));
                ex11_CheckHR(queue->lpVtbl->SetBreakOnSeverity(queue, DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, 1));
                queue->lpVtbl->Release(queue);
            }
        }
        #endif

        // NOTE: create swap chain
        {
            IDXGIDevice *dxgi_device = 0;
            IDXGIAdapter *dxgi_adapter = 0;
            IDXGIFactory2 *dxgi_factory = 0;

            ex11_CheckHR(s_gfx_device->lpVtbl->QueryInterface(s_gfx_device, &IID_IDXGIDevice, &dxgi_device));
            ex11_CheckHR(dxgi_device->lpVtbl->GetAdapter(dxgi_device, &dxgi_adapter));
            ex11_CheckHR(dxgi_adapter->lpVtbl->GetParent(dxgi_adapter, &IID_IDXGIFactory2, &dxgi_factory));

            DXGI_SWAP_CHAIN_DESC1 desc = { 0 };
            desc.Width = 0; // NOTE: use window width
            desc.Height = 0; // NOTE: use window height
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: try DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
            desc.Stereo = 0;
            desc.SampleDesc = (DXGI_SAMPLE_DESC){ .Count = 1, .Quality = 0 };
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = 2;
            desc.Scaling = DXGI_SCALING_NONE;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
            desc.Flags = 0;
            ex11_CheckHR(dxgi_factory->lpVtbl->CreateSwapChainForHwnd(dxgi_factory, (IUnknown *)(s_gfx_device), s_window, &desc, 0, 0, &s_swap_chain));

            // NOTE: disable Alt+Enter changing monitor resolution to match window size
            ex11_CheckHR(dxgi_factory->lpVtbl->MakeWindowAssociation(dxgi_factory, s_window, DXGI_MWA_NO_ALT_ENTER));

            dxgi_device->lpVtbl->Release(dxgi_device);
            dxgi_adapter->lpVtbl->Release(dxgi_adapter);
            dxgi_factory->lpVtbl->Release(dxgi_factory);
        }

        // NOTE: create swap chain's backbuffer render target view
        {
            ID3D11Texture2D *back_buffer = 0;
            ex11_CheckHR(s_swap_chain->lpVtbl->GetBuffer(s_swap_chain, 0, &IID_ID3D11Texture2D, &back_buffer));
            ex11_CheckHR(s_gfx_device->lpVtbl->CreateRenderTargetView(s_gfx_device, (ID3D11Resource *)(back_buffer), 0, &s_back_buffer_rtv));
            back_buffer->lpVtbl->Release(back_buffer);
        }
    }

    while (s_is_running)
    {
        // NOTE: pump window messages
        {
            MSG msg = { 0 };
            while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        if (s_did_resize)
        {
            s_gfx_context->lpVtbl->ClearState(s_gfx_context);
            s_back_buffer_rtv->lpVtbl->Release(s_back_buffer_rtv);

            ex11_CheckHR(s_swap_chain->lpVtbl->ResizeBuffers(s_swap_chain, 0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
            
            ID3D11Texture2D *back_buffer = 0;
            ex11_CheckHR(s_swap_chain->lpVtbl->GetBuffer(s_swap_chain, 0, &IID_ID3D11Texture2D, &back_buffer));
            ex11_CheckHR(s_gfx_device->lpVtbl->CreateRenderTargetView(s_gfx_device, (ID3D11Resource *)(back_buffer), 0, &s_back_buffer_rtv));
            back_buffer->lpVtbl->Release(back_buffer);

            s_did_resize = 0;
        }

        // NOTE: render
        {
            float clear_color[] = { 0.1f, 0.2f, 0.6f, 1.0f };
            s_gfx_context->lpVtbl->ClearRenderTargetView(s_gfx_context, s_back_buffer_rtv, clear_color);
        }

        // NOTE: present
        {
            ex11_CheckHR(s_swap_chain->lpVtbl->Present(s_swap_chain, s_use_vsync, 0));

            // TODO: if present ever crashes, then fix it
            #if 0
            HRESULT hr = IDXGISwapChain_Present(swap_chain, 1, 0);
            if (hr == DXGI_ERROR_DEVICE_RESET || hr == DXGI_ERROR_DEVICE_REMOVED) {
                /* to recover from this, you'll need to recreate device and all the resources */
                MessageBoxW(NULL, L"D3D11 device is lost or removed!", L"Error", 0);
                break;
            }
            else if (hr == DXGI_STATUS_OCCLUDED) {
                /* window is not visible, so vsync won't work. Let's sleep a bit to reduce CPU usage */
                Sleep(10);
            }
            #endif
        }
    }

    // NOTE: release dxgi and d3d11 resources in order to avoid noisy logs in debug output
    #if defined(_DEBUG)
    s_gfx_device->lpVtbl->Release(s_gfx_device);
    s_gfx_context->lpVtbl->Release(s_gfx_context);
    s_swap_chain->lpVtbl->Release(s_swap_chain);
    s_back_buffer_rtv->lpVtbl->Release(s_back_buffer_rtv);
    #endif

    return 0;
}
