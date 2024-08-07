#if ! defined CFG66_HPP
#define CFG66_HPP

/*
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  cfg66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with cfg66; if not, write to the Free Software Foundation, Inc., 59
 *  Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          cfg66.hpp
 *
 *    This module provides a simple informational function.
 *
 * \library       Any application or library
 * \author        Chris Ahlstrom
 * \date          2024-04-15
 * \updates       2024-04-28
 * \license       GNU GPL v2 or above
 *
 */

#if defined __cplusplus

/*
 * namespace xyz
 *
 *      This module currently defines no namespace.
 */

#include <string>                       /* std::string class                */

/**
 *  Free functions.
 */

extern const std::string & cfg66_version () noexcept;

#endif          // defined __cplusplus

#endif          // CFG66_HPP

/*
 * cfg66.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
