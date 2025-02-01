# README for Library Cfg66 0.3 2025-02-01

__Cfg66__ is a configuration library based on the "cfg" directories
in the __Seq66__ project, but that is useful in other applications.
It provides more elegant methods in many cases. It is a work in
progress and the precursor to a potential Seq66 version 2.

Support sites (still in progress):

    *   https://ahlstromcj.github.io/
    *   https://github.com/ahlstromcj/ahlstromcj.github.io/wiki

# Major Features

The "cfg66" directory holds generic code for handling various tasks needed for
the configuration and management of a full-fledged application. Most of the
modules here are refactorings of basic modules in the Seq66 project.
Cfg66 contains the following subdirectories, each of which holds modules
or classes in a namespace of the same name:

    *   cfg:    Contains items that can be used to manage a generic
                configuration, including application names, settings basics,
                and an INI-style configuration-file system. Added are
                data-type indicators and help text. Also provided are
                history<> and memento<> templates to support undo/redo.
    *   cli:    Provides C/C++ code to handle command-line parsing without
                needing to use, for example, getopt. While it somewhat matches
                how getopt works, it also allows combining option sets and
                provides a parser object that contains the current status
                of all available options, as well as help text.
    *   session: Contains classes for managing a basic "session". Here, a
                session is simply a location to put configuration files;
                multiple locations can be supported. Session filenames are
                based on a "home" configuration directory, optional
                subdirectories, and application-specific item names.
    *   util:   Contains file functions, message functions, string functions,
                and other functionality common to all our "66" applications.
    *   tests:  Small test applications are provided to test and illustrate
                most of the classes.

    Note that a work.sh script is provided to simplify or clarify various
    operations such as cleaning, building, making a release, and installing
    or uninstalling the library.

##  Library Features

    *   Can be built using GNU C++ or Clang C++.
    *   Basic dependencies: Meson 1.1 and above; C++14 and above.
    *   The build system is Meson, and sample wrap files are provided
        for using Cfg66 as a C++ subproject.
    *   PDF documentation built from LaTeX.

##  Code

    *   The code is a mix of hard-core C++ and C-like functions.
    *   The C++ STL and advanced language features are used as much as
        possible
    *   C++14 is required for some of its features.
    *   The GNU and Clang C++ compilers are supported.
    *   Broken into modules for easier maintenance.

##  Fixes

    *   Improved the work.sh, added an --uninstall option.

##  Documentation

    *   A PDF developers guide is in progress.

## To Do

    -   Issue: On Windows, we don't have glob.h, so need to add
        Windows-specific code, or perhaps upgrade MingW somehow.
    *   Beef up testing.
    *   Beef up the LaTeX documentation.

## Recent Changes

    For all changes, see the NEWS file.

    *   Version 0.3:
        *   To do.
    *   Version 0.2:
        *   Added the bytevector big-endian handling class and added
            initial tests for it.
        *   Finally added support for parsing and writing INI files
            based on options lists provided in the code.
        *   Potential interface changes; definitely a lot of internal
            changes.
    *   Version 0.1.1:
        *   Many fixes to Meson files and the build script.
        *   Improvements in use as a Meson subproject.
        *   Moved msglevel and tokenization into "lib66" namespace.
    *   Version 0.1.0:
        *   Usage of meson instead of autotools, cmake, or qmake.

// vim: sw=4 ts=4 wm=2 et ft=markdown
