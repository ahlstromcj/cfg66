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
 * \file          filewalker_test.cpp
 *
 *      A test-file for the filewalker file-tree traversal module.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2026-04-03
 * \updates       2026-04-11
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout, set::cerr             */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/filewalker.hpp"          /* util::filewalker big-endian code  */
#include "util/filefunctions.hpp"       /* util::file_exists(), etc.        */
#include "util/msgfunctions.hpp"        /* util::file_message(), etc.       */
#include "util/strfunctions.hpp"        /* util::glob_to_regex(), etc.      */

namespace                               /* anonymous namespace              */
{

/*
 * Application information.
 */

cfg::appinfo s_application_info
{
    cfg::appkind::test,                 // "test"
    "filewalker_test",                  // _app_name (mandatory!)
    "0.1",                              // _app_version
    "[fw]",                             // _main_cfg_section_name
    "",                                 // _home_cfg_directory
    "fw.bogus",                         // _home_cfg_file
    "fw",                               // _client_name (fake session wart)
    "",                                 // _app_tag
    "",                                 // _arg_0
    "CFG66",                            // _package_name
    "",                                 // _session_tag
    "",                                 // _app_icon_name (empty by default)
    "",                                 // _app_version_text derived
    "",                                 // _api_engine (empty by default)
    "0.4",                              // _api_version (empty by default)
    "",                                 // _gui_version (bogus here)
    "fw66",                             // _client_name_short
    "tag"                               // _client_name_tag
};

/*
 * Explanation text.
 */

const std::string s_help_intro
{
    "Not all of the above options are fully supported.\n\n"
    "This test program illustrates/tests the util::filewalker class.\n"
    "To see all of the files operated on, use the --verbose option.\n"
    "For a list of build and run-time details, use the --description\n"
    "command-line option.\n"
};

const std::string s_desc_intro
{
    "This test exercises the util::filewalker file traversal code.\n"
};

/**
 *  A simple recursive traversal of a directory tree. If "verbose",
 *  we use the built-in show_directory_entry() function, otherwise
 *  we provide a function to count entries.
 */

int s_directory_count   { 0 };
int s_file_count        { 0 };
int s_other_count       { 0 };

bool
count_entries (const std::string & /* desc */, std::filesystem::file_type ft)
{
    if (ft == std::filesystem::file_type::directory)
        ++s_directory_count;
    else if (ft == std::filesystem::file_type::regular)
        ++s_file_count;
    else
        ++s_other_count;

    return ft != std::filesystem::file_type::none;
}

bool
fw_traversal_test (bool isverbose)
{
    const std::string rootdir { "tests/data/fts" };
    util::filewalker walker(rootdir);
    bool result { false };
    if (isverbose)
    {
        result = walker.traverse(util::file::show_directory_entry, rootdir);
        if (! result)
            std::cerr << "Showing directory entries failed." << std::endl;
    }
    else
    {
        s_directory_count = s_file_count = s_other_count = 0;
        result = walker.traverse(count_entries, rootdir);
        if (result)
        {
            std::cout
                << s_directory_count << " directories; "
                << s_file_count << " regular files; and "
                << s_other_count << " other kinds of entries."
                << std::endl
                ;
        }
        else
            std::cerr << "Entry counting failed." << std::endl;
    }
    return result;
}

/**
 *  The session.fts file can be found in tests/data/fts,
 *  tests/data/session_2, and tests/data/session_3.
 */

bool
fw_get_file_list_test ()
{
    const std::string rootdir { "tests/data/fts" };
    const std::string target { "session.fts" };
    bool result = util::file::find_file(rootdir, target);
    return result;
}

bool
fw_callback_test ()
{
    const std::string rootdir { "tests/data/fts" };
    util::filewalker walker(rootdir);
    bool result = walker.process_files(util::file::show_target, rootdir);
    util::info_message("Default directory traversal", rootdir);
    if (result)
    {
        std::string nul;
        util::info_message("Compare-files-before-directories traversal.");
        result = walker.process_files
        (
            util::file::show_target, nul,
            util::file::compare_files_before_dirs
        );
    }
    return result;
}

/**
 *  A test of copying a directory hierarchy, which we need to recreate the test
 *  data inside the build directory.  There are two stratagies for directory
 *  traversal. Here is the ordering of each; for brevity, D == directory,
 *  F = File, and DP = Postorder:
 *
 * Copying tests/data/fts to DEST (a path) to result in DEST/fts/....:
 *
 * [fts] Default directory traversal....
 * D:  tests/data/fts                             Make DEST/fts directory
 * D:  tests/data/fts/session_2                   Make DEST/fts/session_2 directory
 * F:  tests/data/fts/session_2/session.fts       Copy session.fts to DEST/fts/session_2
 * DP: tests/data/fts/session_2                   ------
 * D:  tests/data/fts/session_3                   Make DEST/fts/session_3 directory
 * D:  tests/data/fts/session_3/set_1             Make DEST/fts/session_3/set_1 directory
 * F:  tests/data/fts/session_3/set_1/session.fts Copy session.fts to DEST/fts/session_3/set_1
 * DP: tests/data/fts/session_3/set_1             ------
 * D:  tests/data/fts/session_3/set_2             Make DEST/fts/session_3/set_2 directory
 * F:  tests/data/fts/session_3/set_2/session.fts Copy session.fts to DEST/fts/session_3/set_2
 * DP: tests/data/fts/session_3/set_2             ------
 * DP: tests/data/fts/session_3                   ------
 * F:  tests/data/fts/session.fts                 Copy session.fts to DEST/fts
 * DP: tests/data/fts                             ------
 *
 * [fts] Compare-files-before-directories traversal....
 * D:  tests/data/fts                             Make DEST/fts directory
 * F:  tests/data/fts/session.fts                 Copy session.fts to DEST/fts
 * D:  tests/data/fts/session_2                   Make DEST/fts/session_2 directory
 * F:  tests/data/fts/session_2/session.fts       Copy session.fts to DEST/fts/session_2
 * DP: tests/data/fts/session_2                   ------
 * D:  tests/data/fts/session_3                   Make DEST/fts/session_3 directory
 * D:  tests/data/fts/session_3/set_1             Make DEST/fts/session_3/set_1 directory
 * F:  tests/data/fts/session_3/set_1/session.fts Copy session.fts to DEST/fts/session_3/set_1
 * DP: tests/data/fts/session_3/set_1             ------
 * D:  tests/data/fts/session_3/set_2             Make DEST/fts/session_3/set_2 directory
 * F:  tests/data/fts/session_3/set_2/session.fts Copy session.fts to DEST/fts/session_3/set_2
 * DP: tests/data/fts/session_3/set_2             ------
 * DP: tests/data/fts/session_3                   ------
 * DP: tests/data/fts                             ------
 *
 *  Using the second method looks to be a tad more straight-forward.
 *
 *  Note that this code is essentially the same as the free function
 *  fw_copy_directory() function in the filewalker module.
 *
 *  "build/tests/data" becomes "build/tests/data/fts/..."
 */

bool
fw_copy_test ()
{
    const std::string rootdir { "tests/data/fts" };
    const std::string destdir { "build/tests/data" };   /* -> "build/tests/fts/..." */
    util::filewalker walker(rootdir);
    bool result { util::file_exists(destdir) };
    if (! result)
        result = util::make_directory_path(destdir);

    if (result)
    {
        result = walker.process_bi_files
        (
            util::file::item_copy, destdir,
            util::file::compare_files_before_dirs
        );
        if (result)
            result = util::file_is_directory("build/tests/data/fts");
    }

    return result;
}

/**
 *  A test of deleting the files and directories created in fw_copy_test().
 *  We disabled the filewalker direct implementation with
 *  USE_FILEWALKER_DELETION because it cannot properly iterated through
 *  a file-tree where some files have already disappeared.
 */

bool
fw_delete_test ()
{
    const std::string rootdir { "build/tests/data/fts" };
    std::string nul;                            /* no target/regex to match */

#if defined USE_FILEWALKER_DELETION

    util::filewalker walker(rootdir);
    bool result = walker.process_files
    (
        util::file::item_delete, nul,
        util::file::compare_files_before_dirs
    );

#else

    bool result = util::file::delete_directory_tree(rootdir);

#endif

    if (result)
        result = ! util::file_exists("build/tests/data/fts");

    return result;
}

/**
 *  This is the set of files we expect to find.
 */

lib66::tokenization s_expected_results
{
    "tests/data/fts/a_fake_file.midi",
    "tests/data/fts/b_fake_file.midi",
    "tests/data/fts/c_fake_file.midi",
    "tests/data/fts/session_2/a_fake_file.midi",
    "tests/data/fts/session_3/b_fake_file.midi",
    "tests/data/fts/session_3/set_1/c_fake_file.midi",
    "tests/data/fts/session_3/set_2/a_fake_file.midi",
    "tests/data/fts/session_3/set_2/b_fake_file.midi",
    "tests/data/fts/session_3/set_2/c_fake_file.midi",
};

/**
 *  This is the set of files actually found. It can be compared by using
 *  util::compare_tokenizations() to sort and equate it with the
 *  expected results.
 */

lib66::tokenization s_actual_results;

/**
 *  This filewalker-compatible function takes the string that matched
 *  in fw_file_list_test_by_pattern() and adds it to s_actual_results.
 */

bool
fw_collect_matches
(
    const std::string & match,
    std::filesystem::file_type ft
)
{
    std::size_t sz { s_actual_results.size() };
    bool result { false };
    if (! match.empty())
    {
        std::string msg { util::file::get_type_name(ft) };
        msg += " file match";
        util::info_message(msg, match);
        s_actual_results.push_back(match);
        result = s_actual_results.size() == (sz + 1);
    }
    return result;
}

/**
 *  This function passes the fw_collect_matches() function above and
 *  uses filewalker::process_files() to find files that match the
 *  glob "*.midi", which is illegal regex and must be converted to a
 *  regex.
 */

bool
fw_file_list_test_by_pattern ()
{
    const std::string rootdir { "tests/data/fts" };
    util::filewalker walker(rootdir);
    std::string target { "*.midi" };                /* this is NOT a regex  */
    bool result                                     /* verify that fact     */
    {
        ! util::string_has_regex(target)            /* tries to make regex  */
    };
    if (result)
    {
        std::cout
            << "NOTE: this 'Invalid regex' error is *expected*"
            << std::endl
            ;
        util::info_message("Default directory traversal", rootdir);
        std::string rgx                             /* this is a regex      */
        {
            util::glob_to_regex(target)
        };
        util::info_message("Converted '*.midi' glob to regex", V(rgx));
        s_actual_results.clear();
        result = walker.process_files(fw_collect_matches, rgx);
        if (result)
        {
            /*
             * This can't happen directly because of traversal order.
             *
             *      result = s_actual_results == s_expected_results;
             *
             * TODO: SORT THE RESULTS and EXPECTED.
             */

            result = util::compare_tokenizations
            (
                s_actual_results, s_expected_results
            );
        }
    }
    else
    {
        std::cerr
            << "  We did not get the regex error we expected!"
            << std::endl
            ;
    }
    return result;
}

/**
 *  This test is identical to fw_file_list_test_by_pattern(), but
 *  it uses a free function from the filewalker module, and requires
 *  no callback function. It also doesn't test string_has_regex().
 */

bool
find_files_by_pattern ()
{
    const std::string rootdir { "tests/data/fts" };
    std::string rgx { util::glob_to_regex("*.midi") };
    lib66::tokenization collected;
    lib66::tokenization paths;
    util::info_message("Testing 'find_files_by_pattern()'", rgx);
    paths.push_back(rootdir);

    bool result = util::file::find_files_by_regex(collected, paths, rgx);
    if (result)
        result = util::compare_tokenizations(collected, s_expected_results);

    return result;
}

/**
 *  This is another set of files we expect to find.
 */

lib66::tokenization s_expected_results_2
{
    "tests/data/fts/session_3/b_fake_file.midi",
    "tests/data/fts/session_3/set_1/c_fake_file.midi",
    "tests/data/fts/session_3/set_2/a_fake_file.midi",
    "tests/data/fts/session_3/set_2/b_fake_file.midi",
    "tests/data/fts/session_3/set_2/c_fake_file.midi",
};

/**
 *  This test uses another, more complex glob.
 */

bool
find_files_by_pattern_2 ()
{
    const std::string rootdir { "tests/data/fts" };
    std::string target { "*session_3/*.midi" };
    std::string rgx { util::glob_to_regex(target) };
    lib66::tokenization collected;
    lib66::tokenization paths { rootdir };
    util::info_message("Testing 'find_files_by_pattern()' 2", rgx);

    bool result = util::file::find_files_by_regex(collected, paths, rgx);
    if (result)
        result = util::compare_tokenizations(collected, s_expected_results_2);

    return result;
}

/**
 *  This is the set of files we expect to find.
 */

lib66::tokenization s_expected_results_3
{
    "tests/data/fts/session_2/a_fake_file.midi",
    "tests/data/fts/session_3/b_fake_file.midi",
    "tests/data/fts/session_3/set_1/c_fake_file.midi",
    "tests/data/fts/session_3/set_2/a_fake_file.midi",
    "tests/data/fts/session_3/set_2/b_fake_file.midi",
    "tests/data/fts/session_3/set_2/c_fake_file.midi",
};

/**
 *  This test uses two search paths and the simple glob.
 */

bool
find_files_by_pattern_3 ()
{
    const std::string rootdir { "tests/data/fts" };
    std::string target { "*.midi" };
    std::string rgx { util::glob_to_regex(target) };
    lib66::tokenization collected;
    lib66::tokenization paths
    {
        "tests/data/fts/session_2",
        "tests/data/fts/session_3"
    };
    util::info_message("Testing 'find_files_by_pattern()' 3", rgx);

    bool result = util::file::find_files_by_regex(collected, paths, rgx);
    if (result)
        result = util::compare_tokenizations(collected, s_expected_results_3);

    return result;
}

/**
 * -------------------------------------------------------------------
 *  NEXT: add a util::searchpath test. Also test the "icase:" feature.
 * -------------------------------------------------------------------
 */

}       // namespace anonymous

/*
 *  main() routine. Rather than call cfg::set_client_name(),
 *  cfg::set_app_version(), etc., we use a structure to set the application
 *  information.
 */

int
main (int argc, char * argv [])
{
    int rcode = EXIT_FAILURE;
    cli::parser clip;                   /* provides global/stock options    */
    bool success = clip.parse(argc, argv);
    if (success)
        success = cfg::initialize_appinfo(s_application_info, argv[0]);

    if (success)
    {
        if (clip.show_information_only())
        {
            if (clip.help_request())
            {
                std::cout << s_help_intro << std::endl;
            }
            if (clip.description_request())
            {
                std::cout
                    << s_desc_intro << "\n"
                    << "Build details: \n\n" << cfg::get_build_details()
                    << "Run-time details: \n\n" << cfg::get_runtime_details()
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
        {
            util::status_message("fw_copy_test()...");
            success = fw_copy_test();
            if (! success)
                util::error_message("... failed");

            if (success)
            {
                util::status_message("fw_traversal_test()...");
                success = fw_traversal_test(clip.verbose());
                if (! success)
                    util::error_message("... failed");
            }
            if (success)
            {
                util::status_message("fw_get_file_list_test()...");
                success = fw_get_file_list_test();
                if (! success)
                    util::error_message("... failed");
            }
            if (success)
            {
                /*
                 * Basic test of the filewalker callback mechanism.
                 * Very similar to the traversal test.
                 */

                util::status_message("fw_callback_test()...");
                success = fw_callback_test();
                if (! success)
                    util::error_message("... failed");
            }
#if 0
            if (success)
            {
                util::status_message("fw_copy_test()...");
                success = fw_copy_test();
                if (! success)
                    util::error_message("... failed");
            }
#endif
            if (success)
            {
                util::status_message("fw_delete_test()...");
                success = fw_delete_test();
                if (! success)
                    util::error_message("... failed");
            }
            if (success)
            {
                util::status_message("fw_file_list_test_by_pattern()...");
                success = fw_file_list_test_by_pattern();
                if (! success)
                    util::error_message("... failed");
            }
            if (success)
            {
                util::status_message("find_files_by_pattern()...");
                success = find_files_by_pattern();
                if (! success)
                    util::error_message("... failed");
            }
            if (success)
            {
                util::status_message("find_files_by_pattern_2()...");
                success = find_files_by_pattern_2();
                if (! success)
                    util::error_message("... failed");
            }
            if (success)
            {
                util::status_message("find_files_by_pattern_3()...");
                success = find_files_by_pattern_3();
                if (! success)
                    util::error_message("... failed");
            }
            if (success)
            {
                /*
                 * This test works only with nsmd running.
                 */

                const std::string rootdir { "/run/user/1000/nsm" };
                util::filewalker walker(rootdir);
                lib66::tokenization results;
                util::status_message("find_regular_files() test...");
                (void) walker.find_regular_files(results);
                std::cout
                    << "  Note that " << rootdir
                    << " exists only if nsmd is running."
                    << std::endl
                    ;
            }
        }
        if (success)
        {
            std::cout << "util::filewalker C++ test succeeded" << std::endl;
            rcode = EXIT_SUCCESS;
        }
        else
            std::cerr << "util::filewalker C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * filewalker_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp nowrap
 */

