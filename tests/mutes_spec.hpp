#if ! defined CFG66_CFG_MUTES_SPEC_HPP
#define CFG66_CFG_MUTES_SPEC_HPP

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
 * \file          mutes_spec.hpp
 *
 *      Provides a way to hold the Seq66(v2) 'mutes' options.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-08-06
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

const std::string mutes_extension{"mutes"};

/*------------------------------------------------------------------------
 * 'mutes' file
 *------------------------------------------------------------------------*/

inisection::specification mutes_flags_data
{
    "[mutes-group-flags]",
    {
"load-mute-groups: set to 'none', or 'mutes' to load from the 'mutes'\n"
"file, 'midi' to load from the song, or 'both' to try to to read from\n"
"the 'mutes' first, then the 'midi' file.\n"
"\n"
"save-mutes-to: 'both' writes the mutes to the 'mutes' and MIDI file;\n"
"'midi' writes only to the MIDI file; and 'mutes' to the 'mutes' file.\n"
"\n"
"strip-empty: If true, all-zero mute-groups are not written to the\n"
"MIDI file.\n"
"\n"
"mute-group-rows and mute-group-columns: Specifies the size of the grid.\n"
"Keep these values at 4 and 8; mute-group-count is for sanity-checking.\n"
"\n"
"groups-format: 'binary' means write mutes as 0 or 1; 'hex' means write\n"
"them as hexadecimal numbers (e.g. 0xff), useful for larger set sizes.\n"
"\n"
"mute-group-selected: if 0 to 31, and mutes are available either from\n"
"this file or from the MIDI file, then this mute-group is applied at\n"
"startup; useful in restoring a session. Set to -1 to disable.\n"
"\n"
"toggle-active-only: when a mute-group is toggled off, all patterns,\n"
"even those outside the mute-group, are muted.  If this flag is set\n"
"to true, only patterns in the mute-group are muted. Any patterns\n"
"unmuted directly by the user remain unmuted."
    },
    {
        {
            "load-mute-groups",
            {
                options::code_null, "string", options::disabled,
                "midi", "", false, false,
                "From where to load store mute-groups.", false
            }
        },
        {
            "save-mute-groups",
            {
                options::code_null, "string", options::disabled,
                "midi", "", false, false,
                "Where to store mute-groups.", false
            }
        },
        {
            "strip-empty",
            {
                options::code_null, "boolean", options::disabled,
                "true", "", false, false,
                "Do not store all-zero mute-groups.", false
            }
        },
        {
            "mute-group-rows",
            {
                options::code_null, "integer", options::disabled,
                "4<=4<=4", "", false, false,
                "The rows of the mute-group set.", false
            }
        },
        {
            "mute-group-count",
            {
                options::code_null, "integer", options::disabled,
                "32<=32<=32", "", false, false,
                "The count of the mute-group set, sanity check.", false
            }
        },
        {
            "mute-group-selected",      /* "none" for -1?                   */
            {
                options::code_null, "integer", options::disabled,
                "-1", "", false, false,
                "The mute-group to apply at startup, if any.", false
            }
        },
        {
            "groups-format",
            {
                options::code_null, "string", options::disabled,
                "binary", "", false, false,
                "The format of the mute-group statuses.", false
            }
        },
        {
            "toggle-active-only",
            {
                options::code_null, "boolean", options::disabled,
                "false", "", false, false,
                "Toggle only the active patterns.", false
            }
        },
    }
};

inisection::specification mutes_groups_data
{
    "[mutes-groups]",
    {
"Mute-group values are saved in the 'mutes' file, even if all zeroes.\n"
"A hex number indicates each number is a bit-mask, not a single bit.\n"
"An optional quoted group name can be placed at the end of the line."
    },
    {
        {
            "count",
            {
                options::code_null, "list", options::disabled,
                "false", "", false, false,
                "The number of mute-groups.", false
            }
        },
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'mutes' configuration
 *------------------------------------------------------------------------*/

inisection::specification mutes_comments = inifile_comment_data;

inisections::specification mutes_data
{
    "mutes",        /* the file extension for any 'mutes' file.             */
    "",             /* use value from appinfo's get_home_cfg_directory()    */
    "",             /* use value derived from appinfo's get_home_cfg_file() */
    "Used in the [mute-group-file] section of the 'session' file, making it\n"
    "easier to manage multiple sets of mute groups. To use this file,\n"
    "specify it in [mute-group-file] file and set 'active = true'."
    ,
    {
        std::ref(inifile_cfg66_data),       // std::ref(mutes_cfg66_data)
        std::ref(mutes_comments),
        std::ref(mutes_flags_data),
        std::ref(mutes_groups_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_MUTES_SPEC_HPP

/*
* mutes_spec.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

