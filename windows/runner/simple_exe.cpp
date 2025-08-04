#include <windows.h>

extern "C" int __stdcall InitFlutter(HWND hwnd);

LRESULT CALLBACK wndProc(HWND const hwnd, UINT const message, WPARAM const wparam, LPARAM const lparam) noexcept
{
  if (message == WM_CREATE)
  {
    InitFlutter(hwnd);
    return 0;
  }
  if (message == WM_DESTROY)
  {
    PostQuitMessage(0);
    return 0;
  }
  return DefWindowProc(hwnd, message, wparam, lparam);
}

int APIENTRY wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev,
                      _In_ wchar_t *command_line, _In_ int show_command)
{
  CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

  LPCWSTR wndClass = L"SimpleExeWindow";
  WNDCLASSW wc{};
  wc.lpszClassName = wndClass;
  wc.hInstance = instance;
  wc.lpfnWndProc = wndProc;
  RegisterClassW(&wc);

  auto hwnd = CreateWindowExW(
      0, wndClass, L"Simple EXE Test", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
      nullptr, nullptr, instance, nullptr);
  ShowWindow(hwnd, SW_SHOWNORMAL);

  MSG msg;
  while (GetMessageW(&msg, nullptr, 0, 0))
  {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }

  CoUninitialize();
  return 0;
}
