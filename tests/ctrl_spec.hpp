#if ! defined CFG66_CFG_CTRL_SPEC_HPP
#define CFG66_CFG_CTRL_SPEC_HPP

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
 * \file          ctrl_spec.hpp
 *
 *      Provides a way to hold the Seq66(v2) 'ctrl' options.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-08-05
 * \updates       2024-07-15
 * \license       See above.
 *
 *
 */

#include "cfg/inisections.hpp"          /* cfg::inisections class           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

const std::string ctrl_extension{"ctrl"};

/*------------------------------------------------------------------------
 * 'ctrl' file
 *------------------------------------------------------------------------*/

inisection::specification ctrl_control_data
{
    "[midi-control-settings]",
    {
"Input settings to control the application. 'control-buss' goes from 0 to the\n"
"highest system input buss. If set and enabled, that buss can send MIDI\n"
"control. 255 (0xFF) means any enabled input device can send control.\n"
#if defined PLATFORM_LINUX          // or ALSA_SUPPORT
"ALSA provides an extra 'announce' buss, altering port numbering vice JACK.\n"
#endif
"With port-mapping enabled, the port nick-name can be provided.\n"
"\n"
"'control-enabled' applies to the MIDI controls; keystroke controls are always\n"
"active. Supported keyboard layouts are 'qwerty' (default), 'qwertz', and\n"
"'azerty'. AZERTY turns off auto-shift for group-learn."
    },
    {
        {
            "drop-empty-controls",          /* OBSOLETE */
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Drops empty controls from the control list (obsolete).", false
            }
        },
        {
            /*
             * This value used to use 0xff for "none", or a port number.
             * However, we also want to support port names.
             *
             * Do we need yet another integer/string type???
             */

            "control-buss",
            {
                options::code_null, options::kind::string, options::disabled,
                "0xff", "", false, false,
                "The number or name of a MIDI control port.", false
            }
        },
        {
            "control-enabled",          /* replaces "midi-enabled"          */
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enables the specified MIDI control port.", false
            }
        },
        {
            "keyboard-layout",
            {
                options::code_null, options::kind::string, options::disabled,
                "qwerty", "", false, false,
                "The keyboard layout, qwerty, quertz, or azerty.", false
            }
        }

        /*
         * Obsolete options?  Investigate Seq66 usage, if any!
         *
         *  button-offset = 0
         *  button-rows = 4
         *  button-columns = 8
         */
    }
};

inisection::specification ctrl_loop_data
{
    "[loop-control]",
    {
"A control stanza sets key and MIDI control. Keys support 'toggle', and\n"
"key-release is 'invert'. The leftmost number on each line is the loop number\n"
"(0 to 31), mutes number (same range), or an automation number. 3 groups of\n"
"of bracketed numbers follow, each providing a type of control:\n"
"\n"
"   Normal:         [toggle]    [on]        [off]\n"
"   Increment/Decr: [increment] [increment] [decrement]\n"
"   Playback:       [pause]     [start]     [stop]\n"
"   Playlist/Song:  [by-value]  [next]      [previous]\n"
"\n"
"In each group, there are 5 numbers:\n"
"\n"
"   [invert status d0 d1min d1max]\n"
"\n"
"A valid status (> 0x00) enables the control; 'invert' (1/0) inverts the,\n"
"the action, but not all support this.  'status' is the MIDI event to match\n"
"(channel is NOT ignored); 'd0' is the status value (eg. if 0x90, Note On,\n"
"d0 is the note number; d1min to d1max is the range of d1 values detectable.\n"
"Hex values can be used; precede with '0x'.\n"
"\n"
" ------------------------ Loop/group/automation-slot number\n"
"|    -------------------- Name of key (see the key map)\n"
"|   |      -------------- Inverse\n"
"|   |     |    ---------- MIDI status/event byte (eg. Note On)\n"
"|   |     |   |   ------- d0: Data 1 (eg. Note number)\n"
"|   |     |   |  |  ----- d1max: Data 2 min (eg. Note velocity)\n"
"|   |     |   |  | |   -- d1min: Data 2 max\n"
"|   |     |   |  | |  |\n"
"v   v     v   v  v v  v\n"
"0 \"F1\"   [0 0x90 0 1 127] [0 0x00 0 0 0] [0 0x00 0 0 0]\n"
"            Toggle           On              Off\n"
"\n"
"MIDI controls often send a Note On upon a press and a Note Off on release.\n"
"To use a control as a toggle, define only the Toggle stanza. For the control\n"
"to act only while held, define the On and Off stanzas with appropriate\n"
"statuses for press-and-release."
    },
    {
        {
            "count",                    /* slight difference from original  */
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of controls for patterns/loops.", false
            }
        }
    }
};

inisection::specification ctrl_mutes_data
{
    "[mute-group-control]",
    {
"Similar to loop-control, but for mute groups. Same kind of layout."
    },
    {
        {
            "count",                    /* slight difference from original  */
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of controls for mute-groups.", false
            }
        }
    }
};

inisection::specification ctrl_automation_data
{
    "[automation-control]",
    {
"Similar to loop-control, but for application automation. Same kind of layout."
    },
    {
        {
            "count",                    /* slight difference from original  */
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of controls for automation.", false
            }
        }
    }
};

inisection::specification ctrl_control_out_data
{
    "[midi-control-out-settings]",
    {
"Similar to midi-control-settings, but for output. One important used for\n"
"control output is to display application and pattern statuses to a device\n"
"such as the Novation LaunchPad."
    },
    {
        {
            /*
             * This value used to use 0xff for "none", or a port number.
             * However, we also want to support port names.
             *
             * Do we need yet another integer/string type???
             */

            "display-buss",             /* replaces "output-buss"           */
            {
                options::code_null, options::kind::string, options::disabled,
                "0xff", "", false, false,
                "The number or name of a MIDI display port.", false
            }
        },
        {
            "display-enabled",          /* replaces "midi-enabled"          */
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enables the specified MIDI control port.", false
            }
        },

        /*
         * Obsolete options?  Investigate Seq66 usage, if any!
         *
         *  set-size = 32
         *  button-offset = 0
         *  button-rows = 4
         *  button-columns = 8
         */
    }
};

inisection::specification ctrl_display_data
{
    "[midi-display]",                   /* replaces "[midi-control-out]"    */
    {
"This section determines how pattern statuses are to be displayed on a\n"
"control/display device like the LaunchPad.\n"
"\n"
"  ---------------- Pattern or device-button number)\n"
" |     ----------- MIDI status+channel (eg. Note On)\n"
" |    |    ------- data 1 (eg. note number)\n"
" |    |   |  ----- data 2 (eg. velocity)\n"
" |    |   | |\n"
" v    v   v v\n"
"31 [ 0x00 0 0 ] [ 0x00 0 0 ] [ 0x00 0 0 ] [ 0x00 0 0]\n"
"     Armed        Muted        (Un)queued   Empty/Deleted\n"
"\n"
"A test of the status byte determines the enabled status, and channel is\n"
"included in the status."
    },
    {
        {
            "count",                    /* slight difference from original  */
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of MIDI display events for patterns/loop status.",
                false
            }
        }
    }
};

inisection::specification ctrl_display_mutes_data
{
    "[mute-display-out]",               /* replaces "[mute-control-out]"    */
    {
"The format of the mute and automation output events is similar:\n"
"\n"
" ----------------- mute-group number\n"
"|    ------------- MIDI status+channel (eg. Note On)\n"
"|   |    --------- data 1 (eg. note number)\n"
"|   |   |  ------- data 2 (eg. velocity)\n"
"|   |   | |\n"
"v   v   v v\n"
"1 [0x00 0 0 ] [0x00 0 0] [0x00 0 0]\n"
"      On         Off      Empty (dark)\n"
"\n"
"The mute-controls have an additional stanza for non-populated (\"deleted\")\n"
"mute-groups.\n"
    },
    {
        {
            "count",                    /* slight difference from original  */
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of MIDI display events for mute-group status.",
                false
            }
        }
    }
};

inisection::specification ctrl_display_automation_data
{
    "[automation-display-out]",         /* was "[automation-control-out]"   */
    {
"This format is similar to [mute-control-out], but the first number is an\n"
"active-flag, not an index number. The stanzas are are on / off / inactive,\n"
"except for 'snap', which is store /  restore / inactive."
    },
    {
        {
            "count",                    /* slight difference from original  */
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of MIDI display events for automation status.",
                false
            }
        }
    }
};

inisection::specification ctrl_macro_data
{
    "[macro-control-out]",
    {
"The format is 'macroname = [ hex bytes | macro-references]'. References\n"
"are macro-names preceded by a '$'.  Some values will always be defined, even\n"
"if empty: footer, header, reset, startup, and shutdown."
    },
    {
        {
            "footer",
            {
                options::code_null, options::kind::string, options::disabled,
                "", "", false, false,
                "Defines a command that informs a device to change status.",
                false
            }
        },
        {
            "header",
            {
                options::code_null, options::kind::string, options::disabled,
                "", "", false, false,
                "Defines a command that terminates a status command.",
                false
            }
        },
        {
            "reset",
            {
                options::code_null, options::kind::string, options::disabled,
                "", "", false, false,
                "Defines a command that resets a device to normal status.",
                false
            }
        },
        {
            "startup",
            {
                options::code_null, options::kind::string, options::disabled,
                "", "", false, false,
                "Defines a command to send at application startup.",
                false
            }
        },
        {
            "shutdown",
            {
                options::code_null, options::kind::string, options::disabled,
                "", "", false, false,
                "Defines a command to send at application shutdown.",
                false
            }
        }
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'ctrl' inisection
 *------------------------------------------------------------------------*/

inisection::specification ctrl_comments = inifile_comment_data;
const std::string ctrl_intro =
    "Sets up MIDI I/O control. Set it active in 'ctrl' [midi-control-file]\n"
    "section. It adds loop, mute, & automation buttons, MIDI display, new\n"
    "settings, and macros."
    ;

inisections::specification ctrl_data
{
    "ctrl",         /* file extension, applies for any 'ctrl file           */
    "~/",           /* directory: use appinfo's get_home_cfg_directory()    */
    "",             /* basename: use appinfo's get_home_cfg_file()          */
    ctrl_intro,     /* description                                          */
    {
        std::ref(inifile_cfg66_data),       // std::ref(ctrl_cfg66_data)
        std::ref(ctrl_comments),
        std::ref(ctrl_control_data),
        std::ref(ctrl_mutes_data),
        std::ref(ctrl_automation_data),
        std::ref(ctrl_control_out_data),
        std::ref(ctrl_display_data),
        std::ref(ctrl_display_mutes_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_CTRL_SPEC_HPP

/*
* ctrl_spec.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

