#if ! defined CFG66_CFG_PALETTE_SPEC_HPP
#define CFG66_CFG_PALETTE_SPEC_HPP

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
 * \file          palette_spec.hpp
 *
 *      Provides a way to hold the Seq66(v2) 'palette' options.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2023-08-06
 * \updates       2023-08-08
 * \license       See above.
 *
 */

#include "cfg/inisections.hpp"          /* cfg::inisections class           */

/*
 * Do not document the namespace; it breaks Doxygen.
 */

namespace cfg
{

const std::string palette_extension{"palette"};

/*------------------------------------------------------------------------
 * 'palette' file
 *------------------------------------------------------------------------*/

inisection::specification palette_pattern_data
{
    "[palette]",
    {
"This section defines the colors used in drawing each pattern's grid slots.\n"
"The first integer is the color number, ranging from 0 to 31. The first\n"
"string is the name of the background color.  The first stanza (in square\n"
"brackets) are the ARGB values for the background.  The second set\n"
"provides the foreground (lines and text) color.  The alpha values are\n"
"not important here, but should be set to FF."
    },
    {
        {
            "count",
            {
                options::code_null, "list", options::disabled,
                "0", "", false, false,
                "The number of entries in the palette.", false
            }
        }
    }
};

inisection::specification palette_ui_data
{
    "[palette]",
    {
"Similar to the [palette] section, but applies to UI elements such as the\n"
"piano rolls, and to the --inverse color option.  The first integer is the\n"
"color number, ranging from 0 to 11. The names are feature names, not color\n"
"names."
    },
    {
        {
            "count",
            {
                options::code_null, "list", options::disabled,
                "0", "", false, false,
                "The number of entries in the UI palette.", false
            }
        }
    }
};

/*
 * There can be multiple "[palette]" sections.
 */

inisection::specification palette_brush_data
{
    "[brushes]",
    {
"This section defines brush styles to use.  The names are based on the\n"
"names in the Qt::BrushStyle enumeration. The supported names are:\n"
"\n"
"    nobrush, solid, dense1, dense2, dense3, dense4, dense5, dense6,\n"
"    dense7, horizontal, vertical, cross, bdiag, fdiag, diagcross,\n"
"    lineargradient, radialgradient, and conicalgradient.\n"
"\n"
"For 'empty', best to just use 'solid' (try others and see why).  For\n"
"'note', use 'solid' or the default, 'lineargradient'. These also apply\n"
"to the progress box and triggers."
    },
    {
        {
            "empty",
            {
                options::code_null, "string", options::disabled,
                "solid", "", false, false,
                "Defines the brush for empty (white) regions.",
                false
            }
        },
        {
            "notey",
            {
                options::code_null, "string", options::disabled,
                "lineargradient", "", false, false,
                "Defines the brush for notes, triggers, and progress boxes.",
                false
            }
        },
        {
            "scale",
            {
                options::code_null, "string", options::disabled,
                "dense3", "", false, false,
                "Defines the brush for the scale bars.",
                false
            }
        },
        {
            "backseq",
            {
                options::code_null, "string", options::disabled,
                "dense2", "", false, false,
                "Defines the brush for the background pattern.",
                false
            }
        }
    }
};

/*------------------------------------------------------------------------
 * All sections of the 'palette' inifile
 *------------------------------------------------------------------------*/

inisection::specification palette_comments = inifile_comment_data;

inifile::specification palette_data
{
    "palette",      /* the file extension for any 'palette' file.           */
    "",             /* use value from appinfo's get_home_cfg_directory()    */
    "",             /* use value derived from appinfo's get_home_cfg_file() */
    "This file defines two palettes and some brushes, which can change\n"
    "the colors used by patterns and in some parts of the user-interface. The\n"
    "palettes apply to the fonts, piano rolls, names and data panes, and the\n"
    "grid slot progress boxes. Each pattern refers to a color by an integer\n"
    "from 0 to 31. The rest of the user-interface follows the window manager\n"
    "theme."
    ,
    {
        std::ref(inifile_cfg66_data),       // std::ref(palette_cfg66_data)
        std::ref(palette_comments),
        std::ref(palette_pattern_data),
        std::ref(palette_ui_data),
        std::ref(palette_brush_data)
    }
};

}           // namespace cfg

#endif      // CFG66_CFG_PALETTE_SPEC_HPP

/*
* palette_spec.hpp
*
* vim: sw=4 ts=4 wm=4 et ft=cpp
*/

