
# Log-Service Project

This is a small project realizing a C++ Logger-Library suited for the VS-Code environment. It's based on CMake which generates Ninja build-files. It supports Win32, Linux and MacOS. Cross-Compiling is not supported (yet).

## Prerequisites

In order to get this project running you'll need the following tools installed and available in your PATH:

* CMAKE >= v3.18.
* Ninja >= v1.8.*
* LLVM >= v11.0.0 (for macOS and optionally for clang-format)
* GCC >= 8.3 (for LinUX)
* MSVC (for Win32)

## VSCode settings

Once you have all your tools installed you need to adjust the search paths and compiler locations in `c_cpp_properties.json` underneath the `.vscode`-folder. In `launch.json` do the same for the debugger settings.

## Getting started

You can now open, if not yet done, the VSCode workspace specified in `workspace.code-workspace` by simply double-clicking (if associated with VSCode) or by selecting to open it via `File -> Open Workspace from File`. VSCode will load settings and other stuff needed. You'll need at least the following extensions installed:

* C/C++ (Microsoft)
* C/C++ Extension Packs (Microsoft)

You can find them in the integrated VSCode marketplace.

In the side panel you should see the Task Explorer which will list a set of useful tasks that can be launched by clicking. The very first task that must be launched is the `Setup Build Environment` which configures everything CMake-related in order to get you started. As long as you do not need to change anything build-related like the `CMakeLists.txt` file or the toolchain files located in the `toolchains`-directory you do not need to run this task any more. In case you need to, please first call `Cleanup Build Environment` after you altered your build system and the re-run `Setup Build Environment`.

The next step is to build your application. For this purpose there are three Build- and three Clean-Tasks. Chose the one that fits your needs and you'll find your application binary in the `build/bin/<Debug|Release>`-directory, which is called `logger_app`. The standalone library is built in `lib/bin/<Debug|Release>`-directory.

To give you a kind of look and feel you can play around with the example in `playground_app.cpp`.

If you do not want to use the VS-Code IDE that's totally ok. You then need to run the predifined tasks manually in your console in the workspace directory.

* Setup Build Environment: `cmake -G "Ninja Multi-Config" . -B build -DCMAKE_TOOLCHAIN_FILE:PATH=toolchains/<toolchain_file>.cmake`
* Cleanup Build Environment: `rm -r build`
* Build: `cmake --build ${workspaceFolder}/build --config <Release | Debug> --target application -v"`

## Debugging

Visual Debugging is supported, too. Press `F5` and you should see the preparation process. If there is no debug-version of your application available it will be built first.

## Code formatting

This project contains a `.clang-format` file that specifies a set of rules according to the current state of the art. To benefit from it you need to have `clang-format` installed which comes with the LLVM bundle. VSCode can ease your life if you apply some configurations to your `settings.json`.

```json
// files settings
"files.autoSave": "afterDelay",
"files.autoSaveDelay": 1200,
// editor settings
"editor.autoIndent": "advanced",
"editor.renderWhitespace": "all",
"editor.formatOnPaste": true,
"editor.formatOnSave": true,
"editor.formatOnType": true,
// C/C++ settings
"C_Cpp.intelliSenseEngine": "Default",
"C_Cpp.clang_format_path": "/usr/bin/clang-format", // adjust according to your system
```

With these configurations saving your file explicitly (`Strg + s`, `Cmd + s`) will also format your code.
