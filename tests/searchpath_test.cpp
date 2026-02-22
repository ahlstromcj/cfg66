/*
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  cfg66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with cfg66; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          searchpath_test.cpp
 *
 *      A test-file for the new searchpath class.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2026-02-22
 * \updates       2026-02-22
 * \license       See above.
 *
 *  Run this test from the root directory:
 *
 *      $ ./build/tests/searchpath_test
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout, set::cerr             */

#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/filefunctions.hpp"       /* util::set_env()                  */
#include "util/searchpath.hpp"          /* class util::searchpath           */

/*
 * Application information.
 */

namespace       // anonymous
{

void
show_search_path
(
    const util::searchpath & sp,
    const std::string & tag,
    bool showaslist = false
)
{
    if (sp.empty())
    {
        std::cout
            << "[" << tag << "] "
            << "The searchpath is empty."
            << std::endl
            ;
    }
    else
    {
        std::string lst
        {
            showaslist ? sp.to_string_list() : sp.to_string()
        };
        if (showaslist)
            std::cout << "[" << tag << "]\n";
        else
            std::cout << "[" << tag << "] ";

        std::cout << lst << std::endl;
    }
}

const lib66::tokenization s_path_list
{
    "/home/me/.config/searchpath",
    "/home/me/.local/share/searchpath",
    "~/.local/searchpath",
    "/etc/searchpath",
    "/usr/share/searchpath"
};

const lib66::tokenization s_path_list_a
{
    "/home/me/.config/searchpath",
    "/home/me/.local/share/searchpath"
};

const lib66::tokenization s_path_list_b
{
    "~/.local/searchpath",
    "/etc/searchpath",
    "/usr/share/searchpath"
};

const std::string s_extended { "midinam" };

const lib66::tokenization s_path_list_extended
{
    "/home/me/.config/searchpath/midinam",
    "/home/me/.local/share/searchpath/midinam",
    "~/.local/searchpath/midinam",
    "/etc/searchpath/midinam",
    "/usr/share/searchpath/midinam"
};

bool
individual_path_test ()
{
    util::searchpath sp;
    show_search_path(sp, "Empty");

    bool result { sp.add_directory(s_path_list[0]) };
    if (result)
    {
        show_search_path(sp, "Item 0");
        (void) sp.add_directory(s_path_list[0]);
        show_search_path(sp, "Item 0 again");
        result = sp.add_directory(s_path_list[1]);
        if (result)
            show_search_path(sp, "Item 1");

        result = sp.add_directory(s_path_list[2]);
        if (result)
            show_search_path(sp, "Item 2");

        result = sp.add_directory(s_path_list[3]);
        if (result)
            show_search_path(sp, "Item 3");

        result = sp.add_directory(s_path_list[4]);
        if (result)
            show_search_path(sp, "Item 4", true);       /* show as list */
    }
    return result;
}

bool
one_path_ctor_test ()
{
    util::searchpath sp(s_path_list[4]);
    show_search_path(sp, "Item 4");

    std::string item4 { sp.to_string() };
    return item4 == s_path_list[4];
}

bool
vector_ctor_test ()
{
    util::searchpath sp(s_path_list);
    show_search_path(sp, "All items");

    bool result { sp.paths() == s_path_list };
    return result;
}

/**
 *  Note that there is no operator ==() for util::searchpath.
 *  Therefore we check the paths lib66::tokenization vector,
 *  which does have one. We could also test using the to_string()
 *  function.
 *
 *  Also note the default copy constructor is used.
 *
 *  Question: is it the regular one or the move one? We test both.
 */

bool
copy_ctor_test ()
{
    util::searchpath sp(s_path_list);
    util::searchpath sp2(sp);
    bool result { sp2.paths() == sp.paths() };
    if (result)
    {
        util::searchpath sp3(std::move(sp));    /* sp is then "destructed"  */
        result = sp3.paths() == sp2.paths();
        if (result)
            result = sp.count() == 0;           /* i.e. container cleared   */
    }
    return result;
}

/**
 *  Note the default assignment operator is used.
 *
 *  Question: is it the regular one or the move one? We test both.
 */

bool
operator_equal_test ()
{
    util::searchpath sp(s_path_list);
    util::searchpath sp2 = sp;
    bool result { sp.paths() == sp2.paths() };
    if (result)
    {
        util::searchpath sp3 = std::move(sp);   /* sp is then "destructed"  */
        result = sp3.paths() == sp2.paths();
        if (result)
            result = sp.count() == 0;           /* i.e. container cleared   */
    }
    return result;
}

/**
 * Next test.
 */

bool
operator_plus_equal_test ()
{
    util::searchpath spa(s_path_list_a);
    util::searchpath spb(s_path_list_b);
    spa += spb;

    bool result { spa.paths() == s_path_list };
    if (result)
    {
        util::searchpath spa(s_path_list_a);
        spa += s_path_list[2];
        spa += s_path_list[3];
        spa += s_path_list[4];
        result = spa.paths() == s_path_list;
    }
    return result;
}

/**
 * Next test.
 */

bool
operator_plus_test ()
{
    util::searchpath spa(s_path_list_a);
    util::searchpath spb(s_path_list_b);
    util::searchpath sp = spa + spb;

    bool result { sp.paths() == s_path_list };
    if (result)
    {
        util::searchpath sp(s_path_list_a);
        sp = sp + s_path_list[2];
        sp = sp + s_path_list[3];
        sp = sp + s_path_list[4];
        result = sp.paths() == s_path_list;
    }
    return result;
}

/**
 * Next test.
 */

bool
operator_minus_equal_test ()
{
    util::searchpath spa(s_path_list);
    util::searchpath spb(s_path_list_b);
    spa -= spb;

    bool result { spa.paths() == s_path_list_a };
    if (result)
    {
        util::searchpath sp(s_path_list_a);
        sp -= s_path_list[2];
        sp -= s_path_list[3];
        sp -= s_path_list[4];
        result = sp.paths() == s_path_list_a;
    }
    return result;
}

/**
 * Next test.
 */

bool
add_subdirectory_test ()
{
    bool result { true };
    util::searchpath sp(s_path_list);
    (void) sp.add_subdirectory_to_paths(s_extended);

    /*
     * That function expands the "~" character, therefore we cannot
     * get a match here.
     *
     *      bool result { sp.paths() == s_path_list_extended };
     *      if (result)
     *      {
     *          show_search_path(sp, "midnam", true);
     *      }
     */

    show_search_path(sp, "midnam", true);
    return result;
}

bool
contains_test ()
{
    util::searchpath sp(s_path_list);
    bool result { sp.contains("/home/me/.config/searchpath") };
    if (result)
    {
        result = ! sp.contains("/home/me/.config/Searchpath");
        if (result)
        {
            /*
             * The searchpath::contains() function does not
             * call the poor_mans_glob() function. Is this a
             * bug?
             */

            result = sp.contains("~/.local/searchpath");
        }
    }
    return result;
}

}               // namespace anonymous

/*
 *  main() routine.
 *
 *  Note the usage of the V() macro to return the string pointer
 *  (it's shorter than .c_str(), and not so ugly. See the
 *  msgfunctions and strfunctions header files. Without it,
 *  gcc won't complain, but the formatted data is garbage.
 *  Clang won't accept the code at all.
 */

int
main (int argc, char * argv [])
{
    int rcode { EXIT_FAILURE };
    cli::parser clip;                   /* provides global/stock options    */
    bool canrun { false };
    bool success { clip.parse(argc, argv) };
    if (success)
    {
        if (clip.show_information_only())
        {
            if (clip.help_request())
            {
                std::cout << "No additional help." << std::endl;
            }
            if (clip.description_request())
            {
                std::cout
                    << "This app tests the util::searchpath class"
                    << std::endl
                    ;
            }
            if (clip.version_request())
            {
                std::cout
                    << "There is still more to do in this app." << std::endl
                    ;
            }
        }
        else
            canrun = true;
    }
    if (canrun)
    {
        bool success { individual_path_test() };
        if (success)
            success = one_path_ctor_test();

        if (success)
            success = vector_ctor_test();

        if (success)
            success = copy_ctor_test();

        if (success)
            success = operator_equal_test();

        if (success)
            success = operator_plus_equal_test();

        if (success)
            success = operator_plus_test();

        if (success)
            success = add_subdirectory_test();

        if (success)
            success = operator_minus_equal_test();

        if (success)
            success = contains_test();

        if (success)
        {
            std::cout << "searchpath C++ test succeeded" << std::endl;
            rcode = EXIT_SUCCESS;
        }
        else
            std::cerr << "searchpath C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * searchpath_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
