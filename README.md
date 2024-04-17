# README for Library Cfg66 0.1.0 2024-04-17

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

Also included is a "tests" directory with small applications to test some
of the features of this library subset.

Cfg66 contains the following subdirectories, each of which holds modules
or classes in a namespace of the same name:

    *   cfg:    Contains items that can be used to manage a generic
                configuration, including application names, settings basics,
                and an INI-style configuration-file system. Added are
                data-type indicators and help text.
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
    *   PDF documentation built from LaTex.
    *   Basic dependencies: Meson 1.1 and above; C++14 and above.

##  Additional Features (in progress)

    *   To do. Beef up testing; use as a subproject.

##  Usage

##  Internal

    *   Broken into modules for easier maintenance.

##  Usage

## Recent Changes

    *   Version 0.1.0:
        *   Usage of meson instead of autotools, cmake, or qmake.

## To Do

    *   To do

// vim: sw=4 ts=4 wm=2 et ft=markdown
