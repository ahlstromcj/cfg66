#if ! defined CFG66_CFG_USR_SPEC_HPP
#define CFG66_CFG_USR_SPEC_HPP

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
 * \file          usr_spec.hpp
 *
 *      Provides a way to hold the Seq66(v2) 'usr' options.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-08-04
 * \updates       2024-07-15
 * \license       See above.
 *
 *
 *      One big difference is that the port and instrument naming section
 *      is not present. We will either move it to a 'names' file or
 *      a MIDINAM file.  TBD.
 */

#include "cfg/inisections.hpp"          /* cfg::inisections class           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

const std::string usr_extension{"usr"};

/*------------------------------------------------------------------------
 * 'usr' file
 *------------------------------------------------------------------------*/

inisection::specification usr_interface_data
{
    "[user-interface-settings]",
    {
"Configures user-interface elements. Also see [user-ui-tweaks]. The grid\n"
"holds Qt push-buttons... for styling, use Qt themes/style-sheets.\n"
"\n"
"'swap-coordinates' swaps numbering so pattern numbers vary fastest by column\n"
"instead of rows. This setting applies to the live grid, mute-group buttons,\n"
"and set-buttons.\n"
"\n"
"'mainwnd-rows' and 'mainwnd-columns' (option --sets=RxC) specifies\n"
"rows/columns in the main grid. R ranges from 4 to 8, C from 4 to 12.\n"
"\n"
"'mainwnd-spacing' for grid buttons; from 0 to 16 pixels, default = 2.\n"
"'default-zoom' is the initial zoom for piano rolls. From 1 to 512, default\n"
"= 2. Larger PPQNs require larger zoom to look good. Zoom is adapted to the\n"
"PPQN if set to 0. Unit of zoom is ticks/pixel.\n"
"\n"
"'global-seq-feature' applies the key, scale, and background pattern to all\n"
"patterns versus separately to each.  These values are then stored in the\n"
"MIDI file in the global SeqSpec versus in each track.\n"
"\n"
"'progress-bar-thick specifies a thicker progress bar.  Default is 1 pixel;\n"
"thick is 2 if set to true. Also thickens the progress box border and bolds\n"
"the slot font.\n"
"\n"
"'inverse-colors' (option -K/--inverse) specifies use of an inverse color\n"
"palette. Palettes are for drawing areas, not for Qt widgets. The palette\n"
"can be reconfigured via a 'palette' file.\n"
"\n"
"'dark-theme' specifies that a dark theme is active.\n"
"\n"
"'window-redraw-rate' specifies the base window redraw rate for all windows.\n"
"From 10 to 100; default = 40 ms (25 ms for Windows).\n"
"\n"
"Window-scale (option --scale=m.n[xp.q]') specifies scaling the main window\n"
"at startup. Defaults to 1.0 x 1.0. If between 0.5 and 3.0, it changes the\n"
"size of the main window proportionately.\n"
"\n"
"'enable-learn-confirmation' can be set to false to disable the prompt that\n"
"the mute-group learn action succeeded."
    },
    {
        /*
         * Name, Name, Code, Kind, Cli-Enabled, Default, Value,
         * FromCli, Dirty, Description, Built-in
         */

        {
            "enable-learn-confirmation",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "true", "", false, false,
                "Show the message box for a successful learn.", false
            }
        },
        {
            "global-seq-feature",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Store the pattern features in the global SeqSpec.", false
            }
        },
        {
            "mainwnd-rows",
            {
                options::code_null, options::kind::integer, options::disabled,
                "4", "", false, false,
                "Number of rows in a set; use --set-size to change.", false
            }
        },
        {
            "mainwnd-columns",
            {
                options::code_null, options::kind::integer, options::disabled,
                "8", "", false, false,
                "Number of columns in a set; use --set-size to change.", false
            }
        },
        {
            "mainwnd-spacing",
            {
                options::code_null, options::kind::integer, options::disabled,
                "2", "", false, false,
                "Number of pixels between pattern buttons.", false
            }
        },
        {
            "window-scale",
            {
                options::code_null, options::kind::floating, options::disabled,
                "1.0", "", false, false,
                "X-axis scaling of the main window; change with --scale.",
                false
            }
        },
        {
            "window-scale-y",
            {
                options::code_null, options::kind::floating, options::disabled,
                "1.0", "", false, false,
                "Y-axis scaling of the main window; change with --scale.",
                false
            }
        },
        {
            "default-zoom",
            {
                options::code_null, options::kind::integer, options::disabled,
                "2", "", false, false,
                "The default starting horizontal zoom.", false
            }
        },
        {
            "scale",
            {
   /* TODO */   options::code_null, options::kind::floatpair, options::enabled,
                "1.0x1.0", "", false, false,
                "The X,Y scaling of the application window size.", false
            }
        },
        {
            "window-redraw-rate",
            {
                options::code_null, options::kind::integer, options::disabled,
#if defined PLATFORM_WINDOWS
                "25", "", false, false,
#else
                "40", "", false, false,
#endif
                "Time (msec) between refreshes of data displays.", false
            }
        },
        {
            "inverse-colors",
            {
                'K', options::kind::boolean, options::enabled,
                "false", "false", false, false,
                "Invert the colors of the panes and piano rolls.", false
            }
        },
        {
            "dark-theme",
            {
   /* TODO */   options::code_null, options::kind::boolean, options::enabled,
                "false", "false", false, false,
                "Compensate for a dark desktop theme.", false
            }
        },
        {
            "progress-bar-thick",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "Enhance the progress bar and the slot font.", false
            }
        },
        {
            "swap-coordinates",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "Make pattern number vary fastest by columns.", false
            }
        }
    }
};

inisection::specification usr_ppqn_data
{
    "[user-midi-ppqn]",
    {
"Separates file PPQN from application PPQN. 'default-ppqn' specifies the\n"
"PPQN, from 32 to 19200, default = 192. 'use-file-ppqn' (recommended)\n"
"indicates to use file PPQN. Otherwise the file is scaled to the the default\n"
"PPQN when read."
    },
    {
        {
            "ppqn",                     /* replaces"default-ppqn"           */
            {
                'q', options::kind::integer, options::disabled,
                "192", "", false, false,
                "Set the default PPQN.", false
            }
        },
        {
            "use-file-ppqn",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "true", "", false, false,
                "Use the PPQN stored in the MIDI file.", false
            }
        }
    }
};

inisection::specification usr_midi_data
{
    "[user-midi-settings]",
    {
"Specifies MIDI-specific variables. -1 means the value isn't used.\n"
" Item                 Default   Range         Function\n"
"'convert-to-smf-1'   true      true/false.   Converts SMF 0 to SMF 1.\n"
"'beats-per-bar'      4         1 to 32.\n"
"'beats-per-minute'   120.0     2.0 to 600.0.\n"
"'beat-width'         4         1 to 32.\n"
"'buss-override'     -1 (none) -1 to 48.      Overrides all pattern ports.\n"
"'velocity-override' -1 (Free) -1 to 127.     Sets velocity override for notes.\n"
"'bpm-precision'      0         0 to 2.       Precision of beats/minute.\n"
"'bpm-step-increment' 1.0       0.01 to 25.0. Small increment for BPM.\n"
"'bpm-page-increment' 1.0       0.01 to 25.0. Large increment for BPM.\n"
"'bpm-minimum'        0.0       127.0         Low range for tempo drawing.\n"
"'bpm-maximum'        0.0       127.0         High range for tempo drawing.\n"
"'buss-override' will be saved if you save the MIDI file!\n"
"'bpm-step-increment' For 1 decimal, 0.1 is good. For 2, 0.01 is good\n"
"\n"
"Set 'bpm-page-increment' larger than the step-increment; used with\n"
"Page-Up/Page-Down keys in spinner.\n"
"BPM minimum/maximum sets the range in tempo graphing. Decrease it for\n"
"a magnified view of tempo."
    },
    {
        {
            "convert-to-smf-1",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "true", "", false, false,
                "Convert SMF 0 files to SMF 1.", false
            }
        },
        {
            "beats-per-bar",
            {
                options::code_null, options::kind::integer, options::disabled,
                "4", "", false, false,
                "Set the default beats/bar.", false
            }
        },
        {
            "beat-width",
            {
                options::code_null, options::kind::integer, options::disabled,
                "4", "", false, false,
                "Set the default beat length.", false
            }
        },
        {
            "beats-per-minute",
            {
                options::code_null, options::kind::floating, options::disabled,
                "120.0", "", false, false,
                "Set the default beats/minute.", false
            }
        },
        {
            "bpm-precision",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0<=0<=2", "", false, false,
                "Set precision of beats/minute display and changes.", false
            }
        },
        {
            "bpm-step-increment",
            {
                options::code_null, options::kind::floating, options::disabled,
                "1.0", "", false, false,
                "Set small increment of beats/minute.", false
            }
        },
        {
            "bpm-page-increment",
            {
                options::code_null, options::kind::floating, options::disabled,
                "10.0", "", false, false,
                "Set large increment of beats/minute.", false
            }
        },
        {
            "bpm-minimum",
            {
                options::code_null, options::kind::floating, options::disabled,
                "2.0", "", false, false,
                "Set minimum value of beats/minute.", false
            }
        },
        {
            "bpm-maximum",
            {
                options::code_null, options::kind::floating, options::disabled,
                "600.0", "", false, false,
                "Set maximum value of beats/minute.", false
            }
        },
        {
            "buss-override",            /* IDEA replace -1 by "none"|"n/a"  */
            {
                options::code_null, options::kind::integer, options::disabled,
                "-1", "", false, false,
                "Modify each pattern to use the same bus/port.", false
            }
        },
        {
            "velocity-override",        /* IDEA replace -1 by "none"|"n/a"  */
            {
                options::code_null, options::kind::integer, options::disabled,
                "-1", "", false, false,
                "Force notes to have a given velocity.", false
            }
        }
    }
};

/**
 *  The --log option is now a common option for all programs. See the options
 *  class.
 */

inisection::specification usr_options_data
{
    "[user-options]",
    {
"These settings specify -o or --option switch values.  'daemonize' is used\n"
"in CLI apps to indicate the app should run as a service.\n"
    },
    {
        {
            "daemonize",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Use the PPQN stored in the MIDI file.", false
            }
        },
        {
            "pdf-viewer",               /* need to make OS-sprecific        */
            {
                options::code_null, options::kind::filename, options::disabled,
#if defined PLATFORM_WINDOWS
                "evince.exe", "", false, false,
#else
                "/usr/bin/zathura", "", false, false,
#endif
                "Set the PDF viewing application.", false
            }
        },
        {
            "browser",                  /* need to make OS-sprecific        */
            {
                options::code_null, options::kind::filename, options::disabled,
#if defined PLATFORM_WINDOWS
                "edge.exe", "", false, false,
#else
                "/usr/bin/firefox", "", false, false,
#endif
                "Set the browser application.", false
            }
        },
    }
};

inisection::specification usr_tweaks_data
{
    "[user-ui-tweaks]",
    {
"'key-height' specifies the initial height (before vertical zoom) of pattern\n"
"editor keys.  Defaults to 10 pixels, ranges from 6 to 32.\n"
"\n"
"'key-view' specifies the default for showing labels for each key:\n"
"  'octave-letters' (default), 'even-letters', 'all-letters',\n"
"  'even-numbers', and 'all-numbers'.\n"
"\n"
"'note-resume' causes notes-in-progress to resume when the pattern toggles on.\n"
"\n"
"'style-sheet' (e.g. 'qseq66.qss') is applied at startup if note \"\". It\n"
"can be a base-name or a file-path.\n"
"\n"
"'fingerprint-size' is a condensation of note events in a long track, to reduce\n"
"the time drawing the pattern in the buttons. Ranges from 32 (default) to\n"
"128. 0 = don't use a fingerprint; show all events.\n"
"\n"
"'progress-box-width' and '-height' settings change the scaled size of the\n"
"progress box in the live-grid buttons.  Width ranges from 0.50 to 1.0, and\n"
"the height from 0.10 to 1.0.  If either is 'default', defaults (0.8 x 0.3)\n"
"are used.\n"
"\n"
"'progress-box-shown' controls if the boxes are shown at all.\n"
"\n"
"'progress-box-width' and height settings change the scaled size of the\n"
"progress box in the live-loop grid buttons.  Width ranges from 0.50\n"
"to 1.0; the height from 0.10 to 0.50.  If either is 0, then the box\n"
"isn't drawn.  If either is 'default', defaults are used.\n"
"\n"
"'progress-note-min' and -max set the progress-box note\n"
"range so that notes aren't centered in the\n"
"box, but shown at their position by pitch.\n"
"\n"
"'lock-main-window' prevents accidental change of size of the main window."
    },
    {
        {
            "key-height",
            {
                options::code_null, options::kind::integer, options::disabled,
                "10", "", false, false,
                "Sets the initial height of piano keys and grid (pixels).",
                false
            }
        },
        {
            "key-view",
            {
                options::code_null, options::kind::string, options::disabled,
                "octave-letters", "", false, false,
                "Sets the initial representation of the piano keys.",
                false
            }
        },
        {
            "note-resume",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enables the 'resume note on' feature.", false
            }
        },
        {
            "style-sheet-active",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enables usage of a Qt style-sheet.", false
            }
        },
        {
            "style-sheet",
            {
                options::code_null, options::kind::filename, options::disabled,
                "flat-rounded.qss", "", false, false,
                "Enables usage of a Qt style-sheet.", false
            }
        },
        {
            "fingerprint-size",
            {
  /* TODO */    options::code_null, options::kind::integer, options::enabled,
                "32", "", false, false,
                "Set the number of events shown in grid slot (0=all).",
                false
            }
        },
        {
            "progress-box-width",
            {
                options::code_null, options::kind::floating, options::disabled,
                "0.8", "", false, false,
                "Width fraction of the grid slot progress box.", false
            }
        },
        {
            "progress-box-height",
            {
                options::code_null, options::kind::floating, options::disabled,
                "0.3", "", false, false,
                "Height fraction of the grid slot progress box.", false
            }
        },
        {
            "progress-box-shown",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "true", "", false, false,
                "Show the progress box in grid slots.", false
            }
        },
        {
            "progress-note-min",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "", false, false,
                "Vertical note minimum scaling in the progress box.", false
            }
        },
        {
            "progress-note-min",
            {
                options::code_null, options::kind::integer, options::disabled,
                "127", "", false, false,
                "Vertical note maximum scaling in the progress box.", false
            }
        },
        {
            "lock-main-window",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Prevent resizing of the main window.", false
            }
        }
    }
};

inisection::specification usr_session_data
{
    "[user-session]",
    {
"The session manager to use, if any. 'session' is 'none' (default), 'nsm'\n"
"(Non/New Session Manager), or 'jack'. 'url' can be set to the value set by\n"
"nsmd when run by command-line. Set 'url' if running nsmd stand-alone; use\n"
"the --osc-port number. Seq66 detects if started in NSM. The visibility flag\n"
"is used only by NSM to restore visibility."
    },
    {
        {
            "session",
            {
                options::code_null, options::kind::string, options::disabled,
                "non", "", false, false,
                "Specify the putative session manager.", false
            }
        },
        {
            "url",
            {
                options::code_null, options::kind::string, options::disabled,
                "", "", false, false,
                "Specify the URL for the session manager.", false
            }
        },
        {
            "visibility",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "true", "", false, false,
                "Holds the current state of window at startup.", false
            }
        }
    }
};

inisection::specification usr_pattern_data
{
    "[pattern-editor]",                 /* replaces "[new-pattern-editor]"  */
    {
"Setup values for play/recording when a new pattern is opened. A new pattern\n"
"means that the loop has the default name 'Untitled' and no events. These\n"
"values save time during a live recording session. The valid values for\n"
"record-style are 'merge' (default), 'overwrite', 'expand', and 'one-shot'.\n"
"'wrap-around', if true, allows recorded notes to wrap around to the\n"
"pattern start."
    },
    {
        {
            "armed",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Startup state of patterns in pattern editor.", false
            }
        },
        {
            "thru`",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enable MIDI Thru in pattern editor.", false
            }
        },
        {
            "record`",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enable Record in pattern editor.", false
            }
        },
        {
            "qrecord`",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enable Quantized Record in pattern editor.", false
            }
        },
        {
            "record-style",
            {
                options::code_null, options::kind::string, options::disabled,
                "non", "", false, false,
                "Specify the recording style in pattern editor.", false
            }
        },
        {
            "wrap-around`",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "", false, false,
                "Enable wrap-around recording in pattern editor.", false
            }
        }
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'usr' configuration
 *------------------------------------------------------------------------*/

inisection::specification usr_comments = inifile_comment_data;

inisections::specification usr_data
{
    "usr",          /* the file extension for any 'usr' file.               */
    "",             /* use value from appinfo's get_home_cfg_directory()    */
    "",             /* use value derived from appinfo's get_home_cfg_file() */
    "'usr' file. Edit it and put it in ~/.config/seq66. It allows naming\n"
    "each MIDI bus/port, channel, and control code."
    ,
    {
        std::ref(inifile_cfg66_data),       // std::ref(usr_cfg66_data)
        std::ref(usr_comments),
        std::ref(usr_interface_data),
        std::ref(usr_ppqn_data),
        std::ref(usr_midi_data),
        std::ref(usr_options_data),
        std::ref(usr_tweaks_data),
        std::ref(usr_session_data),
        std::ref(usr_pattern_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_USR_SPEC_HPP

/*
 * usr_spec.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

