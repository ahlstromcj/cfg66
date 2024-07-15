#if ! defined CFG66_CFG_RC_SPEC_HPP
#define CFG66_CFG_RC_SPEC_HPP

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
 * \file          rc_spec.hpp
 *
 *      Provides a way to hold all options from multiple INI-style files and
 *      multiple INI file sections.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-07-22
 * \updates       2024-07-15
 * \license       See above.
 *
 */

#include "cfg/inisections.hpp"          /* cfg::inisections class           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

/*------------------------------------------------------------------------
 * 'rc' file
 *------------------------------------------------------------------------*/

/*
 * Note that we use [misc] : last-used-dir instead of a section of that
 * name.
 */

inisection::specification rc_misc_data
{
    "[misc]",
    {
"A number of miscellaneous options. They fit into no obvious grouping.\n"
"\n"
"'sets-mode' affects set muting when moving to the next set. 'normal' leaves\n"
"the next set muted. 'auto-arm' unmutes it. 'additive' keeps the previous set\n"
"armed when moving to the next set. 'all-sets' arms all sets at once.\n"
"\n"
"'port-naming': 'short', 'pair', or 'long'. If 'short', the device name is\n"
"shown. If it  is generic, the client name is added for clarity. If 'pair',\n"
"the client:port number is prepended. If 'long', the full set of name items\n"
"is shown. If port-mapping is active (now the default), this does not apply.\n"
"\n"
"'init-disabled-ports' is experimental. It tries live toggle of port state."
    },
    {
        {
            "sets-mode",
            {
                options::code_null, options::kind::string, options::enabled,
                "normal", "normal", false, false,
                "Mode for handling arming/muting during play-set changes.",
                false
            }
        },
        {
            "port-naming",
            {
                options::code_null, options::kind::string, options::enabled,
                "short", "short", false, false,
                "Port amount-of-label showing.", false
            }
        },
        {
            "init-disabled-ports",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "An experimental option; not recommended.", false
            }
        },
        {
            "last-used-dir",
            {
                options::code_null, options::kind::filename, options::disabled,
                "", "", false, false,
                "Holds the last directory used with a MIDI file.", false
            }
        },
        {
            /*
             * Also, "no-reveal-ports" == "hide-ports"
             */

            "reveal-ports",             /* was called "show-system-ports"   */
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "false", false, false,
                "Show real system ports, not 'usr' port names.", false
            }
        },
        {
            "manual-ports",             /* "auto-ports" = "no-manual-ports" */
            {
                options::code_null, options::kind::boolean, options::enabled,
                "false", "false", false, false,
                "Show real system ports, not 'usr' port names.", false
            }
        }
    }
};

inisection::specification rc_meta_data
{
    "[midi-meta-events]",
    {
"Defines features of MIDI meta-event handling. Tempo events occur in the first\n"
"track (pattern 0), but one can move them elsewhere. It changes where tempo\n"
"events are recorded. The default is 0, the maximum is 1023. A pattern must\n"
"exist at this number."
    },
    {
        {
            "tempo-track",
            {
                options::code_null, options::kind::integer, options::enabled,
                "0", "0", false, false,
                "Specifies an alternate track to hold tempo events.", false
            }
        }
    }
};

/**
 *  This section contains a count that is used in an application-specific
 *  way to read or write a list. The contents of the list are handled
 *  separately.
 */

inisection::specification rc_midi_input_data
{
    "[midi-input]",
    {
"These MIDI ports are for input and control. JACK's view: these are\n"
"'playback' devices. The first number is the bus, the second number is the\n"
"input status, disabled (0) or enabled (1). The item in quotes is the full\n"
"input bus name."
    },
    {
        {
            "count",
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of discovered MIDI input ports.", false
            }
        }
    }
};

inisection::specification rc_midi_input_map_data
{
    "[midi-input-map]",
    {
"Defines alternate port numbers for MIDI input ports. This table is similar\n"
"to the [midi-clock-map] section, but the values are different:\n"
"-2 = unavailable; 0 = not inputing; 1 = enabled for inputing."
    },
    {
        {
            "count",
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a map of port numbers for MIDI input.", false
            }
        }
    }
};

inisection::specification rc_midi_clock_data
{
    "[midi-clock]",
    {
"These MIDI ports are for output/playback and display. JACK's view: these\n"
"are 'capture' devices. The first line shows the count of output ports.\n"
"Each line shows the bus number and clock status of that bus:\n"
"\n"
" -2 = The output port is not present on the system (unavailable).\n"
" -1 = The output port is disabled.\n"
"  0 = MIDI Clock is off. The output port is enabled.\n"
"  1 = MIDI Clock on; Song Position and MIDI Continue are sent.\n"
"  2 = MIDI Clock Modulo.\n"
"\n"
"With Clock Modulo, clocking doesn't begin until song position reaches the\n"
"start-modulo value [midi-clock-mod-ticks]. Ports that are unavailable\n"
"(because another portapplication, e.g. Windows MIDI Mapper, has exclusive\n"
"access to the device) are displayed ghosted.\n"
    },
    {
        {
            "count",
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a list of discovered MIDI output ports.", false
            }
        }
    }
};

inisection::specification rc_midi_clock_map_data
{
    "[midi-clock-map]",
    {
"Patterns use buss numbers, not names. This table provides virtual bus numbers\n"
"that match real devices and can be stored in each pattern. The bus number\n"
"is looked up in this table, the port nick-name is retrieved, and the true\n"
"bus number is obtained and used. Thus, if the ports change order in the MIDI\n"
"system, the pattern will use the proper port. The short nick-names work in\n"
"ALSA or JACK (a2jmidid bridge)."
    },
    {
        {
            "count",
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "", false, false,
                "Holds a map of port number for MIDI output ports.", false
            }
        }
    }
};

inisection::specification rc_midi_clock_mod_data
{
    "[midi-clock-mod]",                 /* was "[midi-clock-mod-ticks]"     */
    {
"'ticks' provides the Song Position (16th notes) at which clocking begins if\n"
"the bus is set to MIDI Clock Mod setting.\n"
"\n"
"'record-by-channel' allows the master MIDI bus to record/filter incoming MIDI\n"
"data by channel, adding each new MIDI event to the pattern that is set to\n"
"that channel. Option adopted from the Seq32 project at GitHub."
    },
    {
        {
            "ticks",
            {
                options::code_null, options::kind::integer, options::disabled,
                "64", "64", false, false,
                "The mod-ticks value for MIDI output ports.", false
            }
        },
        {
            "record-by-channel",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "If true, place input in pattern corresponding to channel.",
                false
            }
        }
    }
};

inisection::specification rc_midi_metro_data
{
    "[metronome]",
    {
"Provides for before-playback count-in and recording, and a metronome that\n"
"can play any sound on the major and minor beats."
    },
    {
        {
            "output-buss",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The output bus for the metronome.", false
            }
        },
        {
            "output-channel",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The output channel for the metronome.", false
            }
        },
        {
            "beats-per-bar",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The beats/measure for the metronome.", false
            }
        },
        {
            "beat-width",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The beat unit (width) for the metronome.", false
            }
        },
        {
            "main-patch",
            {
                options::code_null, options::kind::integer, options::disabled,
                "15", "15", false, false,
                "The patch/program for the highlight beat.", false
            }
        },
        {
            "main-note",
            {
                options::code_null, options::kind::integer, options::disabled,
                "75", "75", false, false,
                "The note value for the highlight beat.", false
            }
        },
        {
            "main-note-velocity",
            {
                options::code_null, options::kind::integer, options::disabled,
                "120", "120", false, false,
                "The note velocity for the highlight beat.", false
            }
        },
        {
            "main-note-length",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The note length for the highlight beat.", false
            }
        },
        {
            "sub-patch",
            {
                options::code_null, options::kind::integer, options::disabled,
                "33", "33", false, false,
                "The patch/program for the sub beat.", false
            }
        },
        {
            "sub-note",
            {
                options::code_null, options::kind::integer, options::disabled,
                "76", "76", false, false,
                "The note value for the sub beat.", false
            }
        },
        {
            "sub-note-velocity",
            {
                options::code_null, options::kind::integer, options::disabled,
                "84", "84", false, false,
                "The note velocity for the sub beat.", false
            }
        },
        {
            "sub-note-length",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The note length for the sub beat.", false
            }
        },
        {
            "count-in-active",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "If true, count-in at start of song.", false
            }
        },
        {
            "count-in-measures",
            {
                options::code_null, options::kind::integer, options::disabled,
                "1", "1", false, false,
                "The number of measures to count-in.", false
            }
        },
        {
            "count-in-recording",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "If true, record during count-in.", false
            }
        },
        {
            "recording-buss",
            {
                options::code_null, options::kind::integer, options::disabled,
                "3", "3", false, false,
                "The port number for count-in recording.", false
            }
        },
        {
            "recording-measures",
            {
                options::code_null, options::kind::integer, options::disabled,
                "1", "1", false, false,
                "The number of measures to count-in record.", false
            }
        },
        {
            "thru-buss",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The through bus for the metronome.", false
            }
        },
        {
            "thru-channel",
            {
                options::code_null, options::kind::integer, options::disabled,
                "0", "0", false, false,
                "The through channel for the metronome.", false
            }
        }
    }
};

inisection::specification rc_interaction_data
{
    "[interaction-method]",
    {
"Sets mouse usage for drawing/editing patterns. 'Fruity' mode is NOT\n"
"supported. Other settings are available:\n"
"\n"
"'snap-split' enables splitting song-editor triggers at a snap position\n"
"instead of in its middle. Split is done by a middle-click or ctrl-left click.\n"
"\n"
"'double-click-edit' allows double-click on a slot to open it in a pattern\n"
"editor. Set it to false if you don't like how it works."
    },
    {
        {
            "snap-split",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "If true, split trigger at snap point.", false
            }
        },
        {
            "double-click-edit",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "If true, allow double-click edit.", false
            }
        }
    }
};

/**
 *  For Seq66v2, these old command-line options will have to be combined and
 *  converted to string options, and the names and codes changed.
 *
 *  transport-type:
 *
 *      Gone:           jack-transport, j
 *      slave:          jack-slave, S
 *      none:           no-jack-transport, g
 *      master:         jack-master, J
 *      conditional:    jack-master-cond, C
 *
 *  user-sesson:        ('usr' file!)
 *
 *      jack:           jack-session, U
 *      none            (new)
 *      nsm             (new)
 *
 *  jack:               (replaces 'jack-midi' and 'no-jack-midi')
 *
 *      boolean:        jack-midi, t, jack, 1
 *
 *  auto-connect:       (replaces 'jack-auto-connect')
 *
 *      boolean:        no-jack-connect, w and jack-connect, W
 *
 *  We also need to add macros to disable/enable depending on
 *  settings and operating system support.
 *
 *  Note that the first line of the description should be short
 *  (up to about 72 characters), and should end with a period.
 */

inisection::specification rc_jack_transport_data
{
    "[jack-transport]",
    {
"'transport-type' enables JACK Transport.\n"
"'none':        No JACK Transport in use.\n"
"'slave':       Use JACK Transport as Slave.\n"
"'master':      Attempt to serve as JACK Transport Master.\n"
"'conditional': Serve as JACK master if no JACK master exists.\n"
"\n"
"'song-start-mode' playback is either Live, Song, or Auto:\n"
"'live': Muting & unmuting of loops in the main window.\n"
"'song': Playback uses Song (performance) editor data.\n"
"'auto': If the loaded tune has song triggers, use Song mode.\n"
"\n"
"'jack' sets/unsets JACK MIDI, separate from JACK transport.\n"
"\n"
"'auto-connect' sets connecting to ALSA/JACK ports found. Default = true; use\n"
"false to have a session manager make the connections.\n"
"\n"
"'jack-use-offset' attempts to calculate timestamp offsets to\n"
"improve accuracy at high-buffer sizes. Still a work in progress.\n"
"\n"
"'jack-buffer-size' allows for changing the frame-count, a power of 2."
    },
    {
        {
            "transport-type",
            {
                'J', options::kind::string, options::enabled,
                "none", "none", false, false,
                "Sets JACK transport to 'none', 'slave', "
                    "'master', or 'conditional'.", false
            }
        },
        {
            "song-start-mode",
            {
                options::code_null, options::kind::string, options::disabled,
                "auto", "auto", false, false,
                "Sets the song-mode to 'live', 'song', or 'auto'.", false
            }
        },
        {
            "jack",                     /* replaces "jack-midi"             */
            {
                'j', options::kind::boolean, options::enabled,
                "false", "false", false, false,
                "Indicates to use JACK MIDI instead of ALSA MIDI.", false
            }
        },
        {
            "auto-connect",             /* replaces "jack-auto-connect"     */
            {
                options::code_null, options::kind::boolean, options::enabled,
                "true", "true", false, false,
                "Indicates to automatically connect ports found in the system.",
                false
            }
        },
        {
            "jack-use-offset",
            {
                options::code_null, options::kind::boolean, options::enabled,
                "true", "true", false, false,
                "Adjust the JACK frame count.", false
            }
        },
        {
            "jack-buffer-size",
            {
                options::code_null, options::kind::integer, options::enabled,
                "0", "0", false, false,
                "Sets JACK frame-count to a power of 2 or "
                    "0 for internal default.", false
            }
        }
    }
};

inisection::specification rc_auto_save_data
{
    "[auto-option-save]",
    {
"'auto-save-rc' sets automatic saving of the  'rc' and other files. If set,\n"
"many command-line settings are saved to configuration files.\n"
"\n"
"'old-triggers' saves triggers in a format compatible with Seq24. Otherwise,\n"
"triggers are saved with an additional 'transpose' setting.\n"
"\n"
"'save-old-mutes' saves mute-groups as long legacy values instead of bytes."
    },
    {
        {
            "auto-save-rc",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "true", "true", false, false,
                "Specifies to always save the 'rc' file.", false
            }
        },
        {
            "save-old-triggers",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "Saves triggers in the legacy Seq24 format.", false
            }
        },
        {
            "save-old-mutes",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "Saves mute-groups in the legacy Seq24 format.", false
            }
        }
    }
};

inisection::specification rc_last_used_data
{
    "[last-used-dir]",
    {
"Holds the name of the directory for the last MIDI song that was loaded."
    },
    {
        {
            "last-used-file",
            {
                options::code_null, options::kind::section, options::disabled,
                "", "", false, false,
                "The directory holding the last-used tune.", false
            }
        }
    }
};

inisection::specification rc_recent_files_data
{
    "[recent-files]",
    {
"The most recently-loaded MIDI files. 'full-paths' = true means to show the\n"
"full file-path in the menu. The most recent file (top of list) can be loaded\n"
"via 'load-most-recent' at startup."
    },
    {
        {
            "full-paths",
            {
                options::code_null, options::kind::boolean, options::disabled,
                "false", "false", false, false,
                "Specifies to show the whole path to the file.", false
            }
        },
        {
            "load-most-recent",
            {
                'L', options::kind::boolean, options::enabled,
                "true", "true", false, false,
                "Specifies to load the most recent file at startup.", false
            }
        },
        {
            "count",
            {
                options::code_null, options::kind::list, options::disabled,
                "0", "0", false, false,
                "The number of files in the most-recent list.", false
            }
        }
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'rc' configuration
 *------------------------------------------------------------------------*/

inisection::specification rc_comments = inifile_comment_data;

inisections::specification rc_data
{
    "rc",           /* the file extension for any 'rc' file.                */
    "",             /* use value from appinfo's get_home_cfg_directory()    */
    "",             /* use value derived from appinfo's get_home_cfg_file() */
    "This 'rc' file defines some basic features, such as MIDI ports, the\n"
    "metronome function, JACK settings, recent-files list, and a few more."
    ,
    {
        std::ref(inifile_cfg66_data),       // std::ref(rc_cfg66_data)
        std::ref(rc_comments),
        std::ref(rc_misc_data),
        std::ref(rc_meta_data),
        std::ref(rc_midi_input_data),
        std::ref(rc_midi_input_map_data),
        std::ref(rc_midi_clock_data),
        std::ref(rc_midi_clock_map_data),
        std::ref(rc_midi_clock_mod_data),
        std::ref(rc_midi_metro_data),
        std::ref(rc_interaction_data),
        std::ref(rc_jack_transport_data),
        std::ref(rc_auto_save_data),
        std::ref(rc_last_used_data),
        std::ref(rc_recent_files_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_RC_SPEC_HPP

/*
 * rc_spec.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

