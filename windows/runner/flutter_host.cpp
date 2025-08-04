#include <flutter/dart_project.h>
#include <flutter/flutter_view_controller.h>
#include <flutter_windows.h>

#include <windows.h>
#include <commctrl.h>

#include <optional>
#include <memory>
#include <iostream>
#include <io.h>
#include <stdio.h>

#include "flutter/generated_plugin_registrant.h"

class FlutterThings
{
public:
  explicit FlutterThings(HWND hwnd) : hwnd(hwnd), project(L"data")
  {
    attachConsoleIfDebug();
    
    SetWindowSubclass(hwnd, _subclassProc, reinterpret_cast<UINT_PTR>(this), reinterpret_cast<DWORD_PTR>(this));

    RECT frame;
    GetClientRect(hwnd, &frame);
    controller = std::make_unique<flutter::FlutterViewController>(
        frame.right - frame.left, frame.bottom - frame.top, project);
    // Ensure that basic setup of the controller was successful.
    if (!controller->engine() || !controller->view())
    {
      throw std::runtime_error("Failed to initialize Flutter engine or view");
    }

    RegisterPlugins(controller->engine());
    auto hwndFlutter = controller->view()->GetNativeWindow();
    SetParent(hwndFlutter, hwnd);
    MoveWindow(hwndFlutter, frame.left, frame.top, frame.right - frame.left,
               frame.bottom - frame.top, TRUE);

    controller->ForceRedraw();
  }

private:
  HWND hwnd;
  std::unique_ptr<flutter::FlutterViewController> controller;
  flutter::DartProject project;

  void onDestroy()
  {
    RemoveWindowSubclass(hwnd, _subclassProc, reinterpret_cast<UINT_PTR>(this));
    delete this;
  }

  LRESULT subclassProc(
      HWND hWnd,
      UINT uMsg,
      WPARAM wParam,
      LPARAM lParam)
  {
    if (controller)
    {
      auto result = controller->HandleTopLevelWindowProc(hwnd, uMsg, wParam, lParam);
      if (result)
        return *result;
    }

    switch (uMsg)
    {
    case WM_FONTCHANGE:
      controller->engine()->ReloadSystemFonts();
      break;
    case WM_SIZE:
    {
      RECT rect;
      GetClientRect(hwnd, &rect);
      MoveWindow(controller->view()->GetNativeWindow(), rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, TRUE);
      break;
    }
    case WM_DESTROY:
      onDestroy();
      break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
  }

  static LRESULT _subclassProc(
      HWND hWnd,
      UINT uMsg,
      WPARAM wParam,
      LPARAM lParam,
      UINT_PTR uIdSubclass,
      DWORD_PTR dwRefData)
  {
    return reinterpret_cast<FlutterThings *>(dwRefData)->subclassProc(
        hWnd, uMsg, wParam, lParam);
  }

  // Attach to console when present (e.g., 'flutter run') or create a
  // new console when running with a debugger.
  void attachConsoleIfDebug()
  {
    if (!AttachConsole(ATTACH_PARENT_PROCESS) && IsDebuggerPresent())
    {
      if (AllocConsole())
      {
        FILE *unused;
        if (freopen_s(&unused, "CONOUT$", "w", stdout))
        {
          _dup2(_fileno(stdout), 1);
        }
        if (freopen_s(&unused, "CONOUT$", "w", stderr))
        {
          _dup2(_fileno(stdout), 2);
        }
        std::ios::sync_with_stdio();
        FlutterDesktopResyncOutputStreams();
      }
    }
  }
};

extern "C" __declspec(dllexport) int WINAPI InitFlutter(HWND hwnd)
{
  new FlutterThings(hwnd);
  return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) { return TRUE; }
