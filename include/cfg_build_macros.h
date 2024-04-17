#if ! defined CFG66_BUILD_MACROS_H
#define CFG66_BUILD_MACROS_H

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
 * \file          cfg_build_macros.h
 *
 *  Macros that depend upon the build platform.
 *
 * \library       cfg66
 * \author        Gary P. Scavone; refactoring by Chris Ahlstrom
 * \date          2022-11-23
 * \updates       2022-11-23
 * \license       See above.
 *
 * Introduction:
 *
 *      We have two ways of configuring the build for the cfg66 library.  This
 *      file presents features that depend upon the build platform, and build
 *      options that might be considered permanent, such as the option to pick
 *      from various JACK-processing callbacks.
 */

#include "platform_macros.h"            /* generic detecting of OS platform */

/**
 *  To do: improve the naming
 */

#if defined CFG66_EXPORT

#if defined PLATFORM_WINDOWS || defined PLATFORM_CYGWIN

#define CFG66_DLL_PUBLIC   __declspec(dllexport)
#define CFG66_API          __declspec(dllexport)

#else

#if PLATFORM_GNUC >= 4
#define CFG66_DLL_PUBLIC   __attribute__((visibility("default")))
#define CFG66_API          __attribute__((visibility("default")))
#else
#define CFG66_API
#define CFG66_DLL_PUBLIC
#endif

#endif

#else                       /* not exporting    */

#define CFG66_API
#define CFG66_DLL_PUBLIC

#endif

#endif          // CFG66_BUILD_MACROS_H

/*
 * cfg_build_macros.h
 *
 * vim: sw=4 ts=4 wm=4 et ft=c
 */

