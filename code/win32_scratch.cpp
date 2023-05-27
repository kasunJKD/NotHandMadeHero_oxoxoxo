#include <windows.h>
#include <stdint.h>
#include <xinput.h>

#define internal static 
#define local_persist static
#define global_variable static 

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

struct wind32_offscreen_buffer
{
    BITMAPINFO bitmapInfo;
    void *BitmapMemory;
    int BitmapWidth;
    int BitmapHeight;
    int BytesPerPixel;
    int Pitch;
};

global_variable bool Running;
global_variable wind32_offscreen_buffer GlobalBackBuffer;

struct win32_window_dimension
{
    int Width;
    int Height;
};
//XInputGetState
#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
typedef X_INPUT_GET_STATE(x_input_get_state);
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return(0);
}
global_variable x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_

//XInputSetState
#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state);
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return(0);
}
global_variable x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_

internal void
Win32LoadXInput(void)
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");
    if(XInputLibrary)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(XInputLibrary, "XInputGetState");
        XInputSetState = (x_input_set_state *)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

win32_window_dimension GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;
    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Height = ClientRect.bottom - ClientRect.top;
    Result.Width = ClientRect.right - ClientRect.left;    
    return(Result);        
}


internal void
RenderWeirdGradient (wind32_offscreen_buffer *Buffer ,int XOffset, int YOffset)
{
    uint8 *Row = (uint8 *)Buffer->BitmapMemory;
    for (int Y = 0;
            Y < Buffer->BitmapHeight;
            Y++)
    {
        uint32 *Pixel = (uint32 *) Row;
        for (int X = 0;
                X < Buffer->BitmapWidth;
                ++X)
                {
                    /*
                        Memory: BB GG RR xx
                        Register: xx RR GG BB
                    */
                   uint8 Blue = (X + XOffset);
                   uint8 Green = (Y + YOffset);
                    *Pixel++ = ((Green << 8) | Blue);

                }
        Row += Buffer->Pitch;
    }
}

internal void
Win32ResizeDIBSection(wind32_offscreen_buffer *Buffer, int Width, int Height)
{
    if(Buffer->BitmapMemory)
    {
        VirtualFree(Buffer->BitmapMemory, 0, MEM_RELEASE);
    }

    Buffer->BitmapWidth = Width;
    Buffer->BitmapHeight = Height;
    Buffer->BytesPerPixel = 4;

    Buffer->bitmapInfo.bmiHeader.biSize = sizeof(Buffer->bitmapInfo.bmiHeader);
    Buffer->bitmapInfo.bmiHeader.biWidth = Buffer->BitmapWidth;
    Buffer->bitmapInfo.bmiHeader.biHeight = -Buffer->BitmapHeight;
    Buffer->bitmapInfo.bmiHeader.biPlanes = 1;
    Buffer->bitmapInfo.bmiHeader.biBitCount = 32;
    Buffer->bitmapInfo.bmiHeader.biCompression = BI_RGB;

    
    int BitmapMemorySize = (Buffer->BitmapWidth*Buffer->BitmapHeight)*Buffer->BytesPerPixel;
    Buffer->BitmapMemory =  VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Width*Buffer->BytesPerPixel;
}

internal void
Win32CopyBufferToWindow(HDC DeviceContext, int WindowWidth, int WindowHeight,
                         int X, int Y, int Width, int Height, wind32_offscreen_buffer *Buffer)
{

    StretchDIBits(DeviceContext,
        // X,Y,Width,Height,
        // X,Y,Width,Height,
        0,0,WindowWidth,WindowHeight,
        0,0,Buffer->BitmapWidth,Buffer->BitmapHeight,
        Buffer->BitmapMemory,
        &Buffer->bitmapInfo,
        DIB_RGB_COLORS,SRCCOPY);
}

LRESULT CALLBACK 
Win32MainWindowCallback(HWND Window,
                    UINT Message,
                    WPARAM WParam,
                    LPARAM LParam)
{

    LRESULT Result = 0;

    switch(Message)
    {
        case WM_SIZE:
        {

            
        } break;
        case WM_DESTROY:
        {
            Running = false;
            
        } break;
        case WM_CLOSE:
        {
            Running = false;   
            
        } break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            
        } break;
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32 VKCode = WParam;
            bool WasDown = ((LParam & (1 << 30)) !=0);
            bool IsDown = ((LParam & (1 << 31)) == 0);
            if(WasDown != IsDown)
            {
                if(VKCode == 'W')
                {
                }
                else if (VKCode == 'A')
                {
                }
                else if (VKCode == 'S')
                {
                }
                else if (VKCode == 'D')
                {
                }
                else if (VKCode == 'Q')
                {
                }
                else if (VKCode == 'E')
                {
                }
                else if (VKCode == VK_UP)
                {
                }
                else if (VKCode == VK_LEFT)
                {
                }
                else if (VKCode == VK_RIGHT)
                {
                }
                else if (VKCode == VK_DOWN)
                {
                }
                else if (VKCode == VK_ESCAPE)
                {
                }
                else if (VKCode == VK_SPACE)
                {
                }
            }       
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;

            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32CopyBufferToWindow(DeviceContext, Dimension.Width,Dimension.Height, X, Y, Width, Height, &GlobalBackBuffer);
            EndPaint(Window, &Paint);
            
        } break;
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
            
        } break;
    }
    return(Result);
}

int CALLBACK WinMain(HINSTANCE Instance,
                    HINSTANCE hPrevInstance,
                    LPSTR     CommandLine,
                    int ShowCode)
{
    WNDCLASSA WindowClass = {};
           
    Win32ResizeDIBSection(&GlobalBackBuffer, 1280, 720);

    WindowClass.style = CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance = Instance;
  //hInstance.hIcon;
    WindowClass.lpszClassName = "NotHandMadeHeroWindowClass";

    if(RegisterClass(&WindowClass))
    {
        HWND Window = 
            CreateWindowEx(
                    0,
                    WindowClass.lpszClassName,
                    "NotHandMade Hero",
                    WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    CW_USEDEFAULT,
                    0,
                    0,
                    Instance,
                    0);

        if(Window) 
        {
            int XOffset = 0;
            int YOffset = 0;
            Running = true;
            while(Running)
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT) 
                    {
                        Running =false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                
                for (DWORD ControllerIndex=0;
                     ControllerIndex < XUSER_MAX_COUNT;
                     ++ControllerIndex)   
                {
                    XINPUT_STATE ControllerState;
                    if(XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                    {
                        //controller is not plugged in
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                        bool Up = (Pad -> wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        bool Down = (Pad -> wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        bool Left = (Pad -> wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        bool Right = (Pad -> wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        bool Start = (Pad -> wButtons & XINPUT_GAMEPAD_START);
                        bool Back = (Pad -> wButtons & XINPUT_GAMEPAD_BACK);
                        bool LeftShoulder = (Pad -> wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        bool RightShoulder = (Pad -> wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        bool AButton = (Pad -> wButtons & XINPUT_GAMEPAD_A);
                        bool BButton = (Pad -> wButtons & XINPUT_GAMEPAD_B);
                        bool XButton = (Pad -> wButtons & XINPUT_GAMEPAD_X);
                        bool YButton = (Pad -> wButtons & XINPUT_GAMEPAD_Y);
                        
                        int16 StickX = Pad -> sThumbLX;
                        int16 StickY = Pad -> sThumbLY;
                    }
                    else 
                    {
                        
                    }
                }

                RenderWeirdGradient(&GlobalBackBuffer,XOffset, YOffset);

                HDC DeviceContext = GetDC(Window);
                win32_window_dimension Dimension = GetWindowDimension(Window);
                Win32CopyBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height, 0, 0, Dimension.Width, Dimension.Height, &GlobalBackBuffer);
                ReleaseDC(Window, DeviceContext);

                ++XOffset;
                YOffset +=2;
                
            }
            
        }
        else
        {

        }
    }
    else
    {

    }

    return(0);
}