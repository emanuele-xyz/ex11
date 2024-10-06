#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#define ex11_Unused(x) (void)(x)

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    ex11_Unused(hInst);
    ex11_Unused(hInstPrev);
    ex11_Unused(cmdline);
    ex11_Unused(cmdshow);

    MessageBox(0, L"Hello!", L"Hello", MB_OK);

    return 0;
}
