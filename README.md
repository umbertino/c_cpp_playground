# C/C++ Playground Project
This is a small demo and playground project suited for MS Visual Studio Code. It's based on CMAKE which generates Ninja buildfiles.
Additionally it is tailored to run on Win32, Linux and MacOS. Cross-Compiling is not supported (yet).
## Prerequisites
In order to get this project running you'll need the following tools installed and available in your PATH:
* CMAKE >= v3.20.
* Ninja >= v1.8.
* LLVM >= v11.0.0 (for macOS)
* GCC >= 8.3 (for LinUX)
* MinGW (for Win32)

## VSCode settings
Once you have all you tools installed you need to adjust the search paths and compiler locations in `c_cpp_properties.json` underneath the `.vscode`-folder. In `launch.json` do the same for the debugger settings.

## Getting started
You can now open, if not yet done, the VSCode workspace specified in `workspace.code-workspace` by simply double-clicking (if associated with VSCode) or by selecting to open it via `File -> Open Workspace from File`. VSCode will load settings and other stuff needed. You'll need at least the following extensions installed:
* C/C++ (Microsoft)
* C/C++ Extension Packs (Microsoft)
* CMake Tools (Microsoft)
* Visual Studio IntelliCode (Microsoft)

You can find them in the integrated VSCode marketplace.

In the side panel you should see the Task Explorer which will list a set of useful tasks that can be launched by clicking. The very first task that must be launched is the `Setup Environment <Linux|Windows|MacOS>` which configures everything CMAKE-related in order to get you started. As long as you do not need to change anything build-related like the `CMakeLists.txt` file or the toolchain files located in the `toolchains`-directory you do not need to run this task any more. In case you need to, please first call `Cleanup Environment` after you altered your build system and the re-run `Setup Environment  <Linux|Windows|MacOS>`.

The next step is to build your application. For this purpose there three Build- and three Clean-Tasks. Chose the one that fits your needs you'll find your application binary in the
`build/<toolchain>/<Debug|Release|RelWithDebInfo>`-directory.

## Debugging
Visual Debugging is supported, too. Press `F5` and you should see the preparation process. If there is no debug-version of your application available it will be built first.