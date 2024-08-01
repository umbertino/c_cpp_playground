#!/usr/bin/python3

"""!
@mainpage A build helper tool

@details This python module helps setting up and perform standard building tasks,
         like build, clean and rebuild. The real steps to be performed are definded
         in dedicated functions.

@date 2024-03-22

@copyright Copyright (c) umbertino@mailbox.org 2024. All rights reserved.
"""

import os
import sys
import platform
import argparse
import raise_cmds as rcmd


this_os = platform.system()
this_arch = platform.machine()
this_target = this_os + "/" + this_arch

action_dict = {
    "build": ["Building", "Built"],
    "rebuild": ["Re-Building", "Re-Built"],
    "clean": ["Cleaning", "Cleaned"]
}

build_modes = {"release", "debug", "all"}

doc_action_dict = {
    "build": ["Building Doc", "Doc built"],
    "clean": ["Cleaning Doc", "Doc built"]
}

doc_build_modes = {"internal", "official"}


def main():
    """!
    @brief The main-routine.

    @details This main routine parses the required arguments in order to
             provide and perform the requested build functionality
    """
    scriptname = os.path.basename(sys.argv[0])
    scriptname_wo_ext = os.path.splitext(scriptname)[0]
    ret = 1

    # the main argument parser
    parser = argparse.ArgumentParser(
        prog=scriptname,
        description='\'' + scriptname_wo_ext + '\'' +
        ' is a Build-Tool to ease build-tasks on command line')
    parser.set_defaults(subCommand='missing')

    # create sub-parser
    sub_parsers = parser.add_subparsers(help='sub-command help')
    sub_parsers.required = True
    sub_parsers.dest = 'subCommand'

    # create the parser for the "build" sub-command
    build_parser = sub_parsers.add_parser('build',
                                          help='The build sub-command',
                                          description='The subcommand build, aimed to compile, link, etc.')
    build_parser_ex = build_parser.add_mutually_exclusive_group()
    build_parser_ex.add_argument('-a', '--all',
                                 dest='build_type',
                                 action='store_const',
                                 const='all',
                                 default='release',
                                 help='Builds all build-types')
    build_parser_ex.add_argument('-d', '--debug',
                                 dest='build_type',
                                 action='store_const',
                                 const='debug',
                                 default='release',
                                 help='Builds Debug build-type')
    build_parser_ex.add_argument('-r', '--release',
                                 dest='build_type',
                                 action='store_const',
                                 const='release',
                                 default='release',
                                 help='Builds Release build-type')
    build_parser.set_defaults(subCommand='build')

    # create the parser for the "rebuild" sub-command
    rebuild_parser = sub_parsers.add_parser('rebuild',
                                            help='The rebuild sub-command',
                                            description='The subcommand rebuild, aimed to re-compile, re-link, etc.')
    rebuild_parser_ex = rebuild_parser.add_mutually_exclusive_group()
    rebuild_parser_ex.add_argument('-a', '--all',
                                   dest='build_type',
                                   action='store_const',
                                   const='all',
                                   default='release',
                                   help='Re-Builds all build-types')
    rebuild_parser_ex.add_argument('-d', '--debug',
                                   dest='build_type',
                                   action='store_const',
                                   default='release',
                                   const='debug',
                                   help='Re-Builds Debug build-type')
    rebuild_parser_ex.add_argument('-r', '--release',
                                   dest='build_type',
                                   action='store_const',
                                   const='release',
                                   default='release',
                                   help='re-Builds Release build-type')
    rebuild_parser.set_defaults(subCommand='rebuild')

    # create the parser for the "clean" sub-command
    clean_parser = sub_parsers.add_parser('clean',
                                          help='The clean sub-command',
                                          description='The subcommand clean, aimed to remove artifacts ' +
                                          'created by the build/re-build subcommand')
    clean_parser_ex = clean_parser.add_mutually_exclusive_group()
    clean_parser_ex.add_argument('-a', '--all',
                                 dest='build_type',
                                 action='store_const',
                                 const='all',
                                 default='release',
                                 help='Cleans all build-types artefacts')
    clean_parser_ex.add_argument('-d', '--debug',
                                 dest='build_type',
                                 action='store_const',
                                 const='debug',
                                 default='release',
                                 help='Cleans the Debug build-type artefacts')
    clean_parser_ex.add_argument('-r', '--release',
                                 dest='build_type',
                                 action='store_const',
                                 const='release',
                                 default='release',
                                 help='Cleans the Release build-type artefacts')
    clean_parser.set_defaults(subCommand='clean')

    # create the parser for the "test" sub-command
    test_parser = sub_parsers.add_parser('test',
                                         help='The test sub-command',
                                         description='The subcommand test, aimed to run unit-tests ' +
                                         'created by the build/re-build subcommand')
    test_parser_ex = test_parser.add_mutually_exclusive_group()
    test_parser_ex.add_argument('-d', '--debug',
                                dest='test_type',
                                action='store_const',
                                const='debug',
                                default='release',
                                help='Runs the unit-tests built for debug')
    test_parser_ex.add_argument('-r', '--release',
                                dest='test_type',
                                action='store_const',
                                const='release',
                                default='release',
                                help='Runs the unit-tests built for release')
    test_parser.set_defaults(subCommand='test')

    # create the parser for the "doc" sub-command
    doc_parser = sub_parsers.add_parser('doc',
                                        help='The doc sub-command',
                                        description='The subcommand doc, aimed to build documentation ')
    doc_parser_ex = doc_parser.add_mutually_exclusive_group()
    doc_parser_ex.add_argument('-i', '--internal',
                               dest='doc_type',
                               action='store_const',
                               const='internal',
                               default='internal',
                               help='Builds internal documentation for developers')
    doc_parser_ex.add_argument('-o', '--official',
                               dest='doc_type',
                               action='store_const',
                               const='official',
                               default='internal',
                               help='Builds official documentation for end users ')
    doc_parser_ex.add_argument('-c', '--clean',
                               dest='doc_type',
                               action='store_const',
                               const='clean',
                               default='internal',
                               help='Cleans all documentation-build artifacts')
    doc_parser.set_defaults(subCommand='doc')

    args = parser.parse_args()
    arg_list = vars(args)

    # we always need one sub-command and a sub-command argument
    # however, this should not happen
    if len(arg_list) != 2:
        sys.exit(1)

    # get current sub-command and sub-command's argumant
    sub_command = list(arg_list.values())[0]
    sub_command_arg = list(arg_list.values())[1]

    print("Raising '" + sub_command + " " +
          sub_command_arg + "' on " + this_target + "...")

    # check subcommand selected
    if sub_command == 'build':
        ret = build(sub_command_arg)
    elif sub_command == 'rebuild':
        ret = rebuild(sub_command_arg)
    elif sub_command == 'clean':
        ret = clean(sub_command_arg)
    elif sub_command == 'test':
        ret = test(sub_command_arg)
    elif sub_command == 'doc':
        ret = doc(sub_command_arg)
    else:
        # should never be reached, catched by argument parser
        print(scriptname + " requires exactly one sub-command")
        ret = 1
        return ret

    result = "failed"

    if ret == 0:
        result = "succeeded"

    print("Raising '" + sub_command + " " + sub_command_arg +
          "' on " + this_target + " " + result)

    return ret


def build(build_mode='release'):
    """!
    @brief The generic build-function.
    @details This function calls the specific function to perform the build task

    @param[in] mode Determines the build-type (release, debug, all)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    ret = 1

    if build_mode not in build_modes:
        print("Build-mode " + build_mode + " not supported or unknown")
        return ret

    if build_mode == "release":
        ret = rcmd.build_cmd('release')

    if build_mode == "debug":
        ret = rcmd.build_cmd('debug')

    if build_mode == "all":
        ret = rcmd.build_cmd('release')
        ret = ret or rcmd.build_cmd('debug')

    return ret


def rebuild(build_mode='release'):
    """!
    @brief The generic rebuild-function.
    @details This function calls the specific function to perform the re-build task

    @param[in] mode Determines the build-type (release, debug, all)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    ret = 1

    if build_mode not in build_modes:
        print("Build-mode " + build_mode + " not supported or unknown")
        return ret

    if build_mode == "release":
        ret = rcmd.clean_cmd('release')
        ret = ret or rcmd.build_cmd('release')

    if build_mode == "debug":
        ret = rcmd.clean_cmd('debug')
        ret = ret or rcmd.build_cmd('debug')

    if build_mode == "all":
        ret = rcmd.clean_cmd('release')
        ret = ret or rcmd.clean_cmd('debug')
        ret = ret or rcmd.build_cmd('release')
        ret = ret or rcmd.build_cmd('debug')

    return ret


def clean(build_mode='release'):
    """!
    @brief The generic clean-function.
    @details This function calls the specific function to perform the clean task

    @param[in] mode Determines the build-type (release, debug, all)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    ret = 1

    if build_mode not in build_modes:
        print("Build-mode " + build_mode + " not supported or unknown")
        return ret

    if build_mode == "release":
        ret = rcmd.clean_cmd('release')

    if build_mode == "debug":
        ret = rcmd.clean_cmd('debug')

    if build_mode == "all":
        ret = rcmd.clean_cmd('release')
        ret = ret or rcmd.clean_cmd('debug')

    return ret


def test(build_mode='release'):
    """!
    @brief The generic test-function.
    @details This function calls the specific function to perform the test task

    @param[in] mode Determines the build-type (release, debug)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    ret = 1

    if build_mode not in build_modes:
        print("Build-mode " + build_mode + " not supported or unknown")
        return ret

    if build_mode == "all":
        print("Build-mode " + build_mode +
              " not supported for testing. Choose 'release' or 'debug'.")
        return ret

    ret = rcmd.test_cmd(build_mode)

    return ret


def doc(doc_build_mode='internal'):
    """!
    @brief The generic documentation-function.
    @details This function calls the specific function to perform the documentation task

    @param[in] mode Determines the documentation-type (internal, official or clean to delete)

    @return The Error-Code
    @retval 0 operation succeeded
    @retval 1 operation failed
    """

    ret = rcmd.doc_cmd(doc_build_mode)

    return ret


if __name__ == "__main__":
    main()
