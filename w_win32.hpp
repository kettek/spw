#pragma once
#include <iostream>
#include <Windows.h>
#include <RichEdit.h>
#include "common.hpp"

namespace spw {
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

  HWND main_hwnd, text_hwnd;
  unsigned int width, height;
  unsigned int padding;

  std::wstring s2ws(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
  }

  LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  void resizeWin32Windows() {
    // Get window size.
    RECT rect = { 0 };
    GetWindowRect(main_hwnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
    // Get scrollbar size.
    SCROLLBARINFO psbi = { 0 };
    psbi.cbSize = sizeof(SCROLLBARINFO);
    GetScrollBarInfo(text_hwnd, OBJID_VSCROLL, &psbi);
    unsigned int sb_width = psbi.rcScrollBar.right - psbi.rcScrollBar.left;
    GetScrollBarInfo(text_hwnd, OBJID_HSCROLL, &psbi);
    unsigned int sb_height = psbi.rcScrollBar.bottom - psbi.rcScrollBar.top;

    // Readjust inline title.

    // Readjust text view.
    MoveWindow(text_hwnd, padding, padding, width-padding*2, height - padding * 2, true);

    // Readjust buttons.
  }

  bool createWin32Window(spw::Level level, const std::string title, const std::string body) {
    LoadLibrary(TEXT("Msftedit.dll"));
    // Register the window class.
    width = default_width, height = default_height;
    padding = 5;

    // Set font to monospace.
    CHARFORMATW cf = { 0 };
    cf.cbSize = sizeof(CHARFORMATW);
    cf.dwMask = CFM_FACE | CFM_SIZE | CFM_COLOR;
    cf.dwEffects = CFE_AUTOCOLOR;
    cf.yHeight = 200;
    cf.bPitchAndFamily = DEFAULT_PITCH | FF_MODERN;

    lstrcpy(cf.szFaceName, L"Consolas");

    const wchar_t CLASS_NAME[] = L"SPW";
    WNDCLASSEX wc = { 0 };

    HINSTANCE hInstance = GetModuleHandle(0);

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    RegisterClassEx(&wc);
    //
    std::wstring wide_title = s2ws(title);
    std::wstring wide_body = s2ws(body);
    // Create main window.
    main_hwnd = CreateWindowEx(
      0,                     // no extended styles 
      CLASS_NAME,       // global string containing name of window class
      wide_title.c_str(),    // global string containing title bar text 
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT,        // default horizontal position 
      CW_USEDEFAULT,        // default vertical position 
      width,        // default width 
      height,        // default height 
      (HWND)NULL,           // no parent for overlapped windows 
      (HMENU)NULL,          // use the window class menu 
      hInstance,            // global instance handle  
      (PVOID)NULL           // pointer not needed 
    );
    // Create scrollable child.
    if (main_hwnd == NULL) {
      return 0;
    }

    HMONITOR monitor = MonitorFromWindow(main_hwnd, MONITOR_DEFAULTTONEAREST);
    if (monitor != NULL) {
      MONITORINFO lpmi;
      lpmi.cbSize = sizeof(MONITORINFO);
      if (GetMonitorInfo(monitor, &lpmi) != 0) {
        unsigned int display_width = lpmi.rcWork.right - lpmi.rcWork.left;
        unsigned int display_height = lpmi.rcWork.bottom - lpmi.rcWork.top;

        if (width > display_width) {
          width = display_width / 2;
        }
        if (height > display_height) {
          height = display_height / 2;
        }
        unsigned int window_x = display_width / 2 - width / 2;
        unsigned int window_y = display_height / 2 - height / 2;

        SetWindowPos(main_hwnd, HWND_TOPMOST, window_x, window_y, width, height, SWP_SHOWWINDOW);
      }
    }

    text_hwnd = CreateWindowEx(
      0,
      MSFTEDIT_CLASS,
      wide_body.c_str(),
      ES_MULTILINE | ES_READONLY | WS_VSCROLL | WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,
      0, 0, 0, 0, 
      main_hwnd, 0, 0, 0
    );
    SendMessage(text_hwnd, EM_SETCHARFORMAT, NULL, (LPARAM)&cf);

    ShowWindow(main_hwnd, SW_SHOWNORMAL);
    UpdateWindow(main_hwnd);

    resizeWin32Windows();
    // Run the message loop.

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    return true;
  }

  LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_SIZE:
    case WM_SIZING:
      resizeWin32Windows();
      return 0;
    /*case WM_PAINT: {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

      EndPaint(hwnd, &ps);
    }*/
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}
