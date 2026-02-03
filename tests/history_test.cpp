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
 * \file          history_test.cpp
 *
 *      A test-file for the history and mememento mechanism.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-07-28
 * \updates       2026-02-03
 * \license       See above.
 *
 *  This program is an extension of sorts for the options_test program. Here
 *  we want to use the mememto and history template classes to support
 *  and test an undo/redo mechanism.
 *
 *  For starters, we want to be able to work with options.  We could work with
 *  inisection objects, but the options they contain are the only thing that
 *  changes during a run.
 *
 *  Here's how the process works, ignoring test-option overhead:
 *
 *      1.  Choose the item to hold the data. Call it "DATUM". It must have:
 *
 *          a.  Default constructor.
 *          b.  Copy constructor.
 *          c.  Principal assignment operator.
 *
 *      2.  Use one of the following constructors:
 *
 *          a.  cfg::history<DATUM>(). This default constructor sets up
 *              a history-list with a default maximum of 128 items.
 *          b.  cfg::history<DATUM>(max_items). This constructor
 *              allows up to "max_items" items to be stored.
 *          c.  cfg::history<DATUM>(max_items, const DATUM & first_one).
 *              This constructor automatically add the first item to
 *              the history.
 *
 *      3,  Let's call the history list "H", and the DATUM item "D".
 *
 *          a.  H.push_undo(D). Add the first item. This is D1.
 *          b.  Make a change to D.
 *          c.  H.push_undo(D). This is D2.
 *          d.  D = H.get_present(). D should equal D2.
 *          e.  D = H.undo(). D should equal D1.
 *          f.  D = H.redo(). D should equal D2.
 *
 * Quick summary of some history<> functions.
 *
 *  -   push_undo(D) makes a memento for D and pushes it.
 *  -   remove() pops the latest memento and throws it away.
 *  -   undo() back-tracks to get the previous memento.
 *  -   redo() moves forward to get the next memento.
 *  -   reset() clears all history.
 *  -   get(i) gets memento i or a dummy value if i is out-of-range.
 *  -   get_present() gets the current memento or a dummy value if
 *      there are none.
 *  -   Functions that can also be used internally.
 *      -   undoable() checks if there is an item to undo.
 *      -   redoable() checks if there is an item to redo.
 *      -   active() returns true if the history object has a memento.
 *      -   present() returns the index to the current memento.
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout                        */

#include "cfg/history.hpp"              /* cfg::history template class      */
#include "cfg/options.hpp"              /* cfg::options class               */
#include "cli/parser.hpp"               /* cli::parser class                */

/**
 *  Test options.
 *
\verbatim
                                           :
        0123456789#@AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz
                                   o                o         oo
                     h        h            h
\endverbatim
 */

static cfg::options::container s_test_options
{
    {
        "alertable",
        {
            'a', cfg::options::kind::boolean, cfg::options::enabled,
            "false", "false", false, false,
            "If specified, the application is alertable.", false
        }
    },
    {
        "loop-count",
        {
            cfg::options::code_null, cfg::options::kind::integer,
            cfg::options::enabled,
            "30", "0-0-99", false, false,
            "Specifies the number of loops to make.", false
        }
    },
    {
        "flux",
        {
            'f', cfg::options::kind::floating, cfg::options::enabled,
            "22.3", "0.0", false, false,
            "Specifies the number of loops to make.", false
        }
    }
};

/*
 * Explanation text.
 */

static const std::string s_help_intro
{
    "This test program illustrates/tests the history/memento of\n"
    "the cfg66 library.  The options available are as follows:\n\n"
};

/*
 * main() routine
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cfg::options optionset(s_test_options);
    cli::parser clip(optionset.option_pairs());
    bool success = clip.parse(argc, argv);
    if (success)
    {
        /*
         *  The application can substitute its own code for the common
         *  options, which are always present.
         */

        if (clip.help_request())
        {
            std::cout << s_help_intro << clip.help_text();
        }
        else
        {
            bool show_history_list = clip.verbose_request();
            cfg::history<cfg::options> h0;
            success = ! h0.active();
            if (success)
            {
                /*
                 * See history.cpp comments.
                 */

                cfg::options & opts = clip.option_set();
                cfg::history<cfg::options> h1 { 4, opts };
                success = h1.active();
                if (success)
                {
                    /*
                     * Follow the step in the banner of the history.cpp
                     * module.
                     */

                    std::cout
                        << "[1]. History ctor initial options\n\n"
                        << opts.debug_text() << std::endl
                        ;

                    if (show_history_list)
                    {
                        std::string hstr = options_history(h1);
                        std::cout << hstr << std::endl;
                    }
                    success = opts.change_value("alertable", "true");
                    if (success)
                    {
                        std::cout
                            << "[2]. Alertable changed to true, "
                               "added to history\n\n"
                            << opts.debug_text() << std::endl
                            ;

                        h1.push_undo(opts);                   /* push a new value */
                        if (show_history_list)
                        {
                            std::string hstr = options_history(h1);
                            std::cout << hstr << std::endl;
                        }
                    }
                    if (success)
                        success = opts.change_value("loop-count", "99");

                    if (success)
                    {
                        std::cout
                            << "[3]. Loop-count changed to 99, "
                               "added to history\n\n"
                            << opts.debug_text() << std::endl
                            ;

                        h1.push_undo(opts);                   /* push a new value */
                        if (show_history_list)
                        {
                            std::string hstr = options_history(h1);
                            std::cout << hstr << std::endl;
                        }
                        success = h1.undo(opts);
                        if (success)
                        {
                            std::cout
                                << "[4]. Loop-count undo to original (30)\n\n"
                                << opts.debug_text() << std::endl
                                ;
                            if (show_history_list)
                            {
                                std::string hstr = options_history(h1);
                                std::cout << hstr << std::endl;
                            }
                        }
                        if (success)
                        {
                            success = h1.redo(opts);
                            std::cout
                                << "[5]. Loop-count change redone to 99\n\n"
                                << opts.debug_text() << std::endl
                                ;
                            if (show_history_list)
                            {
                                std::string hstr = options_history(h1);
                                std::cout << hstr << std::endl;
                            }
                        }
                    }
                    if (success)
                        success = opts.change_value("flux", "3.14159");

                    if (success)
                    {
                        std::cout
                            << "[6]. Flux set to 3.14159\n\n"
                            << opts.debug_text() << std::endl
                            ;

                        h1.push_undo(opts);                   /* push a new value */
                        if (show_history_list)
                        {
                            std::string hstr = options_history(h1);
                            std::cout << hstr << std::endl;
                        }
                    }

                    bool alertable = opts.boolean_value ("alertable");
                    int loopcount = opts.integer_value("loop-count");
                    double flux = opts.floating_value("flux");
                    success =
                    (
                        alertable == true &&
                        loopcount == 99 &&
                        cfg::almost_equal(3.14159, flux)    /* , 6) */
                    );
                    if (success)
                    {
                        if (success)
                            success = opts.change_value("flux", "2.7182818");

                        if (success)
                        {
                            std::cout
                                << "[7]. Flux set to 2.7182818\n\n"
                                << opts.debug_text() << std::endl
                                ;

                            h1.push_undo(opts);               /* push a new value */
                            if (show_history_list)
                            {
                                std::string hstr = options_history(h1);
                                std::cout << hstr << std::endl;
                            }

                            /*
                             * success = history [0].alertable == true and
                             * present() == 3
                             *
                             * Still need to work out present's shifting.
                             */
                        }
                    }
                    else
                        std::cerr << "Value-changes failed!" << std::endl;

                    // TODO Test range validation.
                }
            }
        }

    }
    if (success)
    {
        std::cout << "cfg::history C++ test succeeded" << std::endl;
        rcode = EXIT_SUCCESS;
    }
    else
        std::cerr << "cfg::history C++ test failed" << std::endl;

    return rcode;
}

/*
 * history_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

