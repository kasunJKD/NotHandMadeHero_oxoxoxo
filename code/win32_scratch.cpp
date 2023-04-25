#include <windows.h>
#include <stdint.h>

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
RenderWeirdGradient (wind32_offscreen_buffer Buffer ,int XOffset, int YOffset)
{
    uint8 *Row = (uint8 *)Buffer.BitmapMemory;
    for (int Y = 0;
            Y < Buffer.BitmapHeight;
            Y++)
    {
        uint32 *Pixel = (uint32 *) Row;
        for (int X = 0;
                X < Buffer.BitmapWidth;
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
        Row += Buffer.Pitch;
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
                         int X, int Y, int Width, int Height, wind32_offscreen_buffer Buffer)
{

    StretchDIBits(DeviceContext,
        // X,Y,Width,Height,
        // X,Y,Width,Height,
        0,0,WindowWidth,WindowHeight,
        0,0,Buffer.BitmapWidth,Buffer.BitmapHeight,
        Buffer.BitmapMemory,
        &Buffer.bitmapInfo,
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
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;

            win32_window_dimension Dimension = GetWindowDimension(Window);
            Win32CopyBufferToWindow(DeviceContext, Dimension.Width,Dimension.Height, X, Y, Width, Height, GlobalBackBuffer);
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
    WNDCLASS WindowClass = {};
           
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

                RenderWeirdGradient(GlobalBackBuffer,XOffset, YOffset);

                HDC DeviceContext = GetDC(Window);
                win32_window_dimension Dimension = GetWindowDimension(Window);
                Win32CopyBufferToWindow(DeviceContext, Dimension.Width, Dimension.Height, 0, 0, Dimension.Width, Dimension.Height, GlobalBackBuffer);
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