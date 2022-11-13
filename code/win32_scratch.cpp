#include <windows.h>

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PWSTR pCmdLine,
                    int nCmdShow)
{
    MessageBoxA(0, "This is dev from scratch",  "From scratch", MB_OK|MB_ICONINFORMATION);
    return(0);
}