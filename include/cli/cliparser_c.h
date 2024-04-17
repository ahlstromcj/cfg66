#if ! defined CFG66_CLIPARSER_C_H
#define CFG66_CLIPARSER_C_H

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
 * \file          cliparser_c.h
 *
 *  Type definitions pulled out for the needs of the refactoring.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2022-06-21
 * \updates       2024-01-15
 * \license       See above.
 *
 */

#include <stdbool.h>                    /* macros related to booleans       */
#include <stddef.h>                     /* NULL and other macros            */

#include "c_macros.h"                   /* EXTERN_C_DEC and EXTERN_C_END    */

EXTERN_C_DEC                            /* the whole module is extern "C"   */

/**
 *  This C structure almost exactly matches the cfg::options::spec class's
 *  layout. The name and the spec are copied into an std::pair of
 *  type cfg::options::option.
 *
 *  It is needed for interfacing with C code.
 */

typedef struct
{
    const char * option_name;           /**< The key, long option name.     */
    unsigned char option_code;          /**< The single-character name.     */
    const char * option_kind;           /**< "boolean", other type string.  */
    bool option_cli_enabled;            /**< Normally true; false disables. */
    const char * option_default;        /**< Either a string or true/false  */
    const char * option_value;          /**< The actual value as parsed.    */
    bool option_modified;               /**< Changed since read/save.       */
    const char * option_desc;           /**< The actual value as parsed.    */

} options_spec;                         /* C version of cli::options::spec  */

extern bool create_option_list (const options_spec * opts, int optcount);
extern void reset (void);
extern void initialize (void);
extern bool parse_option_list (int argc, char * argv []);
extern bool change_value (const char * name, const char * value, bool fromcli);
extern bool value (const char * name, char * value, size_t szvalue);
extern const char * help_text (void);
extern const char * debug_text (void);
extern bool help_request (void);
extern bool version_request (void);

EXTERN_C_END

#endif          // CFG66_CLIPARSER_C_H

/*
 * cliparser_c.h
 *
 * vim: sw=4 ts=4 wm=4 et ft=c
 */

