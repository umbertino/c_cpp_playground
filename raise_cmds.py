#!/usr/bin/python


"""!
@mainpage User functions to be provided to raise.py

@details This python module povides the specific and user defined functions to
         perform the tasks offered by raise.py.

@date 2024-03-22

@copyright Copyright (c) umbertino@mailbox.org 2024. All rights reserved.
"""

import os
import platform


def build_cmd(mode):
    """!
    @brief The specific build-function.
    @details This specifies and performs the concrete build tasks.
                It issues commands that fits to the underlying OS.

    @param[in] mode Determines the build-type (release, debug)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    this_os = platform.system()
    # this_arch = platform.machine()
    build_type = None
    target_dir = None
    ret = 1

    if mode == 'release':
        build_type = "Release"
        target_dir = "Release"
    elif mode == 'debug':
        build_type = "Debug"
        target_dir = "Debug"
    else:
        return ret

    if this_os == 'Linux':
        ret = os.system("conan install . --output-folder=conan/" + target_dir +
                        " --build=missing  -s build_type=" + build_type + " && " +
                        ". ./conan/" + target_dir + "/conanbuild.sh" + "&& " +
                        "cmake ./ -GNinja -DCMAKE_BUILD_TYPE=" + build_type +
                        " -DCMAKE_TOOLCHAIN_FILE:PATH=./conan/" + target_dir +
                        "/conan_toolchain.cmake -B./build/" + target_dir + " && " +
                        "cmake --build ./build/" + target_dir + " && " +
                        ". ./conan/" + target_dir + "/deactivate_conanbuild.sh")
    elif this_os == 'Darwin':
        print("No build command specified for " + this_os)

        ret = 1
    elif this_os == 'Windows':
        print("No build command specified for " + this_os)

        ret = 1
    else:
        print("No able to detect underlying platform/OS ")

        ret = 1

    return ret


def clean_cmd(mode):
    """!
    @brief The specific clean-function.
    @details This specifies and performs the concrete clean tasks.
             It issues commands that fits to the underlying OS.

    @param[in] mode Determines the build-type (release, debug)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    this_os = platform.system()
    # this_arch = platform.machine()
    target_dir = None
    ret = 1

    if mode == 'release':
        target_dir = "Release"
    elif mode == 'debug':
        target_dir = "Debug"
    else:
        return ret

    if this_os == 'Linux':
        ret = os.system("rm -rf build/" + target_dir)
        ret = ret or os.system("rm -rf conan/" + target_dir)
    elif this_os == 'Darwin':
        print("No build task specified for " + this_os)

        ret = 1
    elif this_os == 'Windows':
        print("No build task specified for " + this_os)

        ret = 1
    else:
        print("No able to detect underlying platform/OS ")

    return ret


def test_cmd(mode):
    """!
    @brief The specific test-function.
    @details This function executes the unit-tetst.
             It issues commands that fits to the underlying OS.

    @param[in] mode Determines the build-type (release, debug)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    this_os = platform.system()
    # this_arch = platform.machine()

    ret = 1

    if mode == 'release':
        ret = 0
    elif mode == 'debug':
        ret = 0
    else:
        return ret

    if this_os == 'Linux':
        print("No test task specified for " + this_os)

        ret = 1
    elif this_os == 'Darwin':
        print("No test task specified for " + this_os)

        ret = 1
    elif this_os == 'Windows':
        print("No test task specified for " + this_os)

        ret = 1
    else:
        print("No able to detect underlying platform/OS for the \'test\' subcommand")

        ret = 1

    return ret


def doc_cmd(mode):
    """!
    @brief The specific documentation-function.
    @details This function builds the documentation.
             It issues commands that fits to the underlying OS.

    @param[in] mode Determines the documentation-type (internal, official or clean to delete)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    this_os = platform.system()
    # this_arch = platform.machine()

    ret = 1

    if mode in ('internal', 'official', 'clean'):
        ret = 0
    else:
        return ret

    if this_os == 'Linux':
        if mode in ('internal', 'official'):
            ret = os.system("doxygen && cd doc/latex && make" )
        elif mode == 'clean':
            ret = os.system("rm -rf ./doc")
        else:
            ret = 1
    elif this_os == 'Darwin':
        print("No documentation task specified for " + this_os)

        ret = 1
    elif this_os == 'Windows':
        print("No documentation task specified for " + this_os)

        ret = 1
    else:
        print("No able to detect underlying platform/OS for the \'test\' subcommand")

        ret = 1

    return ret
