#if ! defined CFG66_CFG_PLAYLIST_SPEC_HPP
#define CFG66_CFG_PLAYLIST_SPEC_HPP

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
 * \file          playlist_spec.hpp
 *
 *      Provides a way to hold the Seq66(v2) 'playlist' options.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-08-05
 * \updates       2024-06-14
 * \license       See above.
 *
 */

#include "cfg/inisections.hpp"          /* cfg::inisections class           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

const std::string playlist_extension{"playlist"};

/*------------------------------------------------------------------------
 * 'playlist' file
 *------------------------------------------------------------------------*/

inisection::specification playlist_options_data
{
    "[playlist-options]",
    {
"These options affect the handling of song in the play-list."
    },
    {
        {
            "unmute-new-song",
            {
                options::code_null, "boolean", options::disabled,
                "false", "", false, false,
                "When the next song is selected, unmute its patterns.", false
            }
        },
        {
            "auto-play",            /* BACKPORT to Seq66 playlistfile.cpp */
            {
                options::code_null, "boolean", options::disabled,
                "false", "", false, false,
                "When the next song is selected, start is playing.", false
            }
        },
        {
            "auto-advance",         /* BACKPORT to Seq66 playlistfile.cpp */
            {
                options::code_null, "boolean", options::disabled,
                "false", "", false, false,
                "When the current song ends, load the next song.", false
            }
        },
        {
            "deep-verify",
            {
                options::code_null, "boolean", options::disabled,
                "false", "", false, false,
                "When loading the play-list, verify each song opens.",
                false
            }
        }
    }
};

/*
 * There can be multiple "[playlist]" sections.
 */

inisection::specification playlist_list_data
{
    "[playlist]",
    {
"First provide the playlist settings, its default storage folder, and list\n"
"each tune with its control number. The playlist number is arbitrary but\n"
"unique. 0 to 127 recommended for use with the MIDI playlist control. Similar\n"
"for the tune numbers. Each tune can include a path; it overrides the base\n"
"directory."
    },
    {
        {
            "number",
            {
                options::code_null, "integer", options::disabled,
                "0<=0<=127", "", false, false,
                "Defines the number (and MIDI control value) for this list.",
                false
            }
        },
        {
            "name",
            {
                options::code_null, "string", options::disabled,
                "", "", false, false,
                "Defines the display name for the play-list.",
                false
            }
        },
        {
            "directory",
            {
                options::code_null, "string", options::disabled,
                "", "", false, false,
                "Provides an option base directory for the play-list tunes.",
                false
            }
        },
        {
            "count",
            {
                options::code_null, "list", options::disabled,
                "0", "", false, false,
                "The number of files in the play-list.", false
            }
        }
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'playlist' configuration
 *------------------------------------------------------------------------*/

inisection::specification playlist_comments = inifile_comment_data;

inisections::specification playlist_data
{
    "playlist",     /* the file extension for any 'playlist' file.          */
    "",             /* use value from appinfo's get_home_cfg_directory()    */
    "",             /* use value derived from appinfo's get_home_cfg_file() */
    "This file holds multiple playlists, each in a [playlist] section. Each has\n"
    "a user-specified number for sorting and MIDI control, ranging from 0 to 127.\n"
    "Next comes a quoted name for this list, followed by the quoted name\n"
    "of the song directory using the UNIX separator ('/').\n"
    "\n"
    "Next is a list of tunes, each starting with a MIDI control number and the\n"
    "quoted name of the MIDI file, sorted by control number. They can be simple\n"
    "'base.midi' file-names; the playlist directory is prepended to access the\n"
    "song file. If the file-name has a path, that will be used."
    ,
    {
        std::ref(inifile_cfg66_data),       // std::ref(playlist_cfg66_data)
        std::ref(playlist_comments),
        std::ref(playlist_options_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_PLAYLIST_SPEC_HPP

/*
* playlist_spec.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

