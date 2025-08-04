# flutter_independent_dll

This project is trying to separate Flutter functionality to a simple DLL and use it in a simple wrapper program without much hassle.

## Windows

[windows/runner/CMakeLists.txt](windows/runner/CMakeLists.txt) is modified to:

- build a simple executable ([windows/runner/simple_exe.cpp](windows/runner/simple_exe.cpp)) that only shows a blank window
- build a DLL ([windows/runner/main.cpp](windows/runner/main.cpp)) that hosts all Flutter related functionality and export `InitFlutter` function only
