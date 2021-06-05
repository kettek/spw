#pragma once
#include <iostream>
#include <Windows.h>
#include "common.hpp"

namespace spw {

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

  bool createWin32Window(spw::Level level, const std::string title, const std::string body) {
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"SPW";

    WNDCLASS wc = { };

    HINSTANCE hInstance = GetModuleHandle(0);

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);
    //
    std::wstring wide_title = s2ws(title);
    // Create main window.
    HWND hwnd = CreateWindowEx(
      0,                     // no extended styles 
      CLASS_NAME,       // global string containing name of window class
      wide_title.c_str(),    // global string containing title bar text 
      WS_OVERLAPPEDWINDOW |
      WS_HSCROLL | WS_VSCROLL, // window styles 
      CW_USEDEFAULT,        // default horizontal position 
      CW_USEDEFAULT,        // default vertical position 
      CW_USEDEFAULT,        // default width 
      CW_USEDEFAULT,        // default height 
      (HWND)NULL,           // no parent for overlapped windows 
      (HMENU)NULL,          // use the window class menu 
      hInstance,            // global instance handle  
      (PVOID)NULL           // pointer not needed 
    );
    // Create scrollable child.
    if (hwnd == NULL) {
      return 0;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);

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
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hwnd, &ps);

      FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

      EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}
