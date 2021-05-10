#include <windows.h>
#include <iostream>
#include <thread>
#include <intrin.h>

#define UNICODE

HANDLE MyThread;
LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI MyThreadFunc(LPVOID lpParam);
LPCWSTR IntToStr(int N);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HWND hwnd; // дескриптор окна
    MSG msg; // структура сообщения
    WNDCLASS w; // структура класса окна
    // Регистрация класса окна
    memset(&w, 0, sizeof(WNDCLASS));
    w.style = CS_HREDRAW | CS_VREDRAW;
    w.lpfnWndProc = WndProc; // имя оконной функции
    w.hInstance = hInstance;
    w.hbrBackground = (HBRUSH)(WHITE_BRUSH);
    w.lpszClassName = L"My Class";
    RegisterClass(&w);
    // Создание окна
    hwnd = CreateWindow(L"My Class", L"Окно пользователя",
        WS_OVERLAPPEDWINDOW, 500, 300, 700, 380, NULL, NULL, hInstance, NULL);
    ShowWindow(hwnd, nCmdShow); // отображение
    UpdateWindow(hwnd);          // перерисовка
    // Цикл обработки сообщений
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
    HINSTANCE hInst;
    HDC hdc;
    PAINTSTRUCT ps;
    static HWND hBtnMouse, hBtnSup;
    static HWND hStatMouse, hStatSup;
    static int rgr_result[2];
    switch (Message)
    {
    case WM_CREATE:
        hInst = ((LPCREATESTRUCT)lparam)->hInstance;

        MyThread = CreateThread(NULL, NULL, MyThreadFunc, &rgr_result, 0, NULL);
        if (MyThread == 0)
            exit(-1);


        hBtnMouse  = CreateWindow(L"button", L"Мышь", WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 20, 120, 30, hwnd, 0, hInst, NULL);
        hBtnSup    = CreateWindow(L"button", L"8bit", WS_CHILD | WS_VISIBLE | WS_BORDER, 50, 100, 120, 30, hwnd, 0, hInst, NULL);
        hStatMouse = CreateWindow(L"edit", L"Доподлинно неизвестно", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 270, 70, 200, 20, hwnd, 0, hInst, NULL);
        hStatSup = CreateWindow(L"edit", L"Доподлинно неизвестно", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_RIGHT, 400, 150, 200, 20, hwnd, 0, hInst, NULL);
        break;
    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        TextOut(hdc, 50, 70, L"Количество кнопок мыши: ", 25);
        TextOut(hdc, 50, 150, L"Поддержка команды сравнения и обмена 8bit: ", 44);
        EndPaint(hwnd, &ps);
        break;
    case WM_COMMAND:
        if (lparam == (LPARAM)hBtnMouse)
        {
            SetWindowText(hStatMouse, IntToStr(rgr_result[0]));
        }
        else if (lparam == (LPARAM)hBtnSup)
        {
            if(rgr_result[1])
                SetWindowText(hStatSup, L"Поддержка есть");
            else
                SetWindowText(hStatSup, L"Поддержки нет");
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, Message, wparam, lparam);
    }
    return 0;
}

DWORD WINAPI MyThreadFunc(LPVOID lpParam)
{
    HINSTANCE hilib = LoadLibraryA("dllsysteminfo.dll");
    if (hilib == 0)
    {
        MessageBox(NULL, TEXT("Библиотека не найдена"), NULL, MB_ICONERROR | MB_SYSTEMMODAL);
        exit(-1);
    }
    typedef int(*getC)();

    getC dllMouse;
    getC dllSup8;

    dllMouse = (getC)GetProcAddress(hilib, "getMouseInfo");
    dllSup8 = (getC)GetProcAddress(hilib, "getSupport8info");
    if (!dllMouse)
        MessageBox(NULL, TEXT("getCountMouseButtons не найдена"), NULL, MB_ICONERROR | MB_SYSTEMMODAL);
    else
        ((int*)lpParam)[0] = dllMouse();
    if (!dllSup8)
        MessageBox(NULL, TEXT("getInfoSupCx8 не найдена"), NULL, MB_ICONERROR | MB_SYSTEMMODAL);
    else
    {
        ((int*)lpParam)[1] = dllSup8();
    }
    FreeLibrary(hilib);
    
    /*
    int iedx, res;
    __asm
    {
        mov eax, 0x00000001
        cpuid
        mov iedx, edx
    }
    res = (iedx & (1 << 8)) != 0;


    ((int*)lpParam)[0] = GetSystemMetrics(SM_CMOUSEBUTTONS);
    ((int*)lpParam)[1] = res;
    */
    return 0;
}

LPCWSTR IntToStr(int n)
{
    wchar_t buffer[256];
    wsprintfW(buffer, L"%d", n);

    return buffer;
}