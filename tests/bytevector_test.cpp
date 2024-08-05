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
 * \file          bytevector_test.cpp
 *
 *      A test-file for the bytevector big-endian data class.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-05-17
 * \updates       2024-08-05
 * \license       See above.
 *
 */

#include <cstdlib>                      /* EXIT_SUCCESS, EXIT_FAILURE       */
#include <iostream>                     /* std::cout, set::cerr             */

#include "cfg/appinfo.hpp"              /* cfg::appinfo functions           */
#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "util/bytevector.hpp"          /* util::bytevector big-endian code */
#include "util/msgfunctions.hpp"        /* util::file_message(), etc.       */

/*
 * Application information.
 */

static cfg::appinfo s_application_info
{
    cfg::appkind::test,                 // "test"
    "bytevector_test",                  // _app_name (mandatory!)
    "0.0",                              // _app_version
    "[Byte66]",                         // _main_cfg_section_name
    "",                                 // _home_cfg_directory
    "bvectest.bogus",                   // _home_cfg_file
    "bvectest",                         // _client_name (fake session wart)
    "",                                 // _app_tag
    "",                                 // _arg_0
    "CFG66",                            // _package_name
    "",                                 // _session_tag
    "",                                 // _app_icon_name (empty by default)
    "",                                 // _app_version_text derived
    "",                                 // _api_engine (empty by default)
    "0.3",                              // _api_version (empty by default)
    "",                                 // _gui_version (bogus here)
    "bvec",                             // _client_name_short
    "tag"                               // _client_name_tag
};

/*
 * Explanation text.
 */

static const std::string s_help_intro
{
    "Not all of the above options are fully supported.\n\n"
    "This test program illustrates/tests the util::bytevector class.\n"
    "For a list of build and run-time details, use the --description\n"
    "command-line option.\n"
};

static const std::string s_desc_intro
{
    "This test exercises the util::bytevector big-endian file I/O.\n"
    "The file '1Bar.midi' is a short sample MIDI file to illustrate\n"
    "the process.  This test reads that file; we still need to write\n"
    "code to write the file.\n"
};

/*
 *  String I/O test.
 */

static bool
basic_string_io ()
{
    std::string fname{"tests/data/1Bar.midi"};
    util::bytevector bv0;
    bv0.assign(fname);

    std::string all{bv0.peek_string()};
    bool result = all == fname;
    std::cout << "The whole string: '" << all << "'" << std::endl;
    if (result)
    {
        util::bytevector bv1{fname};
        std::string all{bv1.peek_string()};
        bool result = all == fname;
        if (result)
        {
            std::string data{bv1.peek_string(6, 4)};
            result = data == "data";
        }

        // MORE TESTS TO COME
    }
    return result;
}

/*
 *  File I/O test using a MIDI file. MIDI files, like network data, are
 *  big-endian. (Intel processors are little endian, Motorola processors are
 *  big-endian).
 */

static bool
big_endian_file_io ()
{
    static const util::ulong c_mthd_tag  = 0x4D546864;  /* magic no. 'MThd' */
    static const util::ulong c_mtrk_tag  = 0x4D54726B;  /* magic no. 'MTrk' */
    std::string fname{"tests/data/1Bar.midi"};
    util::bytevector bv0;
    bool result = bv0.read(fname);
    if (result)
    {
        util::file_message("Read", fname);
        util::ulong ID = bv0.get_long();                /* hdr chunk        */
        util::ulong hdrlength = bv0.get_long();         /* MThd length      */
        util::ushort format;
        util::ushort trackcount;
        util::ushort fppqn;
        result = ID == c_mthd_tag && hdrlength == 6;
        if (result)
        {
            format = bv0.get_short();
            trackcount = bv0.get_short();
            fppqn = bv0.get_short();
            result = format == 1 && trackcount == 1 && fppqn == 192;
        }
        if (result)
        {
            util::ulong trkID = bv0.get_long();         /* get 'MTrk'       */
            util::ulong tracklen = bv0.get_long();      /* get track length */
            result = trkID == c_mtrk_tag && tracklen == 309;
            if (result)
            {
                util::bytevector trkbytes;
                trkbytes.assign(bv0, bv0.position(), tracklen);

                util::ulong delta = trkbytes.get_varinum();  /* improve!!! */
                util::byte bstatus = trkbytes.peek_byte();
                result = delta == 0 && bstatus == 0xff;
                if (result)
                {
                    util::bytevector bv1;
                    bv1.put_long(ID);
                    bv1.put_long(hdrlength);
                    bv1.put_short(format);
                    bv1.put_short(trackcount);
                    bv1.put_short(fppqn);
                    bv1.put_long(trkID);
                    bv1.put_long(tracklen);
                    bv1.put_varinum(delta);
                    bv1.put_byte(bstatus);

                    std::string outname{"tests/data/1Bar-out.midi"};
                    result = bv1.write(outname);
                }
            }
        }
    }
    else
        util::file_error("Failed to read", fname);

    return result;
}

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
            success = basic_string_io();
            if (success)
                success = big_endian_file_io();
        }
        if (success)
            std::cout << "util::bytevector C++ test succeeded" << std::endl;
        else
            std::cerr << "util::bytevector C++ test failed" << std::endl;
    }
    return rcode;
}

/*
 * bytevector_test.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

