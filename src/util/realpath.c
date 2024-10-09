/*
 * realpath.c -- canonicalize pathname by removing symlinks
 *
 * Copyright (C) 1993 Rick Sladkey <jrs@world.std.com>
 * Copyright (C) 2000-2006 Erik Andersen <andersen@uclibc.org>
 *
 * Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
 *
 *  This file is part of cfg66.
 *
 *  cfg66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU Lesser Public License as published by the Free Software
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
 * \file          realpath.c
 *
 *      Provides an implementation of realpath() to use with non-GNU extensions,
 *      for example in CygWin/MSYS2.
 *
 * \library       cfg66
 * \author        Chris Ahlstrom
 * \date          2024-10-08
 * \updates       2024-10-08
 * \version       $Revision$
 *
 *      Returns the canonicalized absolute path name.
 *
 *
 */

#include <errno.h>
#include <limits.h>                     /* for the PATH_MAX                 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>                  /* for the MAXPATHLEN macro         */
#include <sys/stat.h>                   /* for the S_IFLNK value            */
#include <sys/types.h>
#include <unistd.h>

#include "c_macros.h"                   /* not_nullptr() macro              */

#if ! defined PATH_MAX
#if defined _POSIX_VERSION
#define PATH_MAX _POSIX_PATH_MAX        /* compare to filefunctions.cpp     */
#else
#if defined MAXPATHLEN
#define PATH_MAX MAXPATHLEN
#else
#define PATH_MAX 1024
#endif
#endif
#endif

#define MAX_READLINKS 32

static int
__set_errno (int n)
{
    errno = n;
    return (-1);
}

/*
 *  Find the real name of path, by removing all ".", ".." and symlink
 *  components.  Returns (resolved) on success, or nullptr on failure, in which
 *  case the path which caused trouble is left in the got_path [] parameter.
 */

char *
realpath (const char * path, char got_path [])
{
    char copy_path[PATH_MAX];
    char * max_path;
    char * new_path;
    char * allocated_path;
    size_t path_len;

#if defined S_IFLNK
    int readlinks = 0;
    int link_len;
#endif

    if (is_nullptr(path))
    {
        __set_errno(EINVAL);
        return nullptr;
    }
    if (*path == '\0')
    {
        __set_errno(ENOENT);
        return nullptr;
    }

    /*
     *  Make a copy of the source path since we may need to modify it.
     *  Then copy so that path is at the end of copy_path[].
     */

    path_len = strlen(path);
    if (path_len >= PATH_MAX - 2)
    {
        __set_errno(ENAMETOOLONG);
        return nullptr;
    }
    strcpy(copy_path + (PATH_MAX-1) - path_len, path);
    path = copy_path + (PATH_MAX-1) - path_len;
    allocated_path = got_path ? nullptr : (got_path = malloc(PATH_MAX));
    max_path = got_path + PATH_MAX - 2; /* points to last non-NUL char */
    new_path = got_path;
    if (*path != '/')
    {
        /*
         *  If it's a relative pathname use getcwd for starters.
         */

        if (! getcwd(new_path, PATH_MAX - 1))
            goto copyerr;

        new_path += strlen(new_path);
        if (new_path[-1] != '/')
            *new_path++ = '/';
    }
    else
    {
        *new_path++ = '/';
        path++;
    }

    /*
     *  Expand each slash-separated pathname component.
     */

    while (*path != '\0')
    {
        if (*path == '/')               /* Ignore stray "/"                 */
        {
            path++;
            continue;
        }
        if (*path == '.')
        {
            if (path[1] == '\0' || path[1] == '/')
            {
                path++;
                continue;
            }
            if (path[1] == '.')         /* Ignore "."                       */
            {
                if (path[2] == '\0' || path[2] == '/')
                {
                    path += 2;
                    if (new_path == got_path + 1)   /* Ignore ".." at root  */
                        continue;

                    while ((--new_path)[-1] != '/') /* Handle ".."; back up */
                        ;
                    continue;
                }
            }
        }

        /*
         *  Safely copy the next pathname component.
         */

        while (*path != '\0' && *path != '/')
        {
            if (new_path > max_path)
            {
                __set_errno(ENAMETOOLONG);
 copyerr:
                free(allocated_path);
                return nullptr;
            }
            *new_path++ = *path++;
        }

#if defined S_IFLNK

        /*
         *  Protect against infinite loops
         */

        if (readlinks++ > MAX_READLINKS)
        {
            __set_errno(ELOOP);
            goto copyerr;
        }
        path_len = strlen(path);

        /*
         *  See if last (so far) pathname component is a symlink.
         *  EINVAL means the file exists, but isn't a symlink.
         */

        *new_path = '\0';
        {
            int sv_errno = errno;
            link_len = readlink(got_path, copy_path, PATH_MAX - 1);
            if (link_len < 0)
            {
                if (errno != EINVAL)
                {
                    goto copyerr;
                }
            }
            else
            {
                if (path_len + link_len >= PATH_MAX - 2)    /* safety check */
                {
                    __set_errno(ENAMETOOLONG);
                    goto copyerr;
                }

                /*
                 *  readlink() doesn't add the null byte.
                 *
                 * copy_path[link_len] = '\0';      // we don't need it either
                 */

                if (*copy_path == '/')      /* restart for absolute symlink */
                {
                    new_path = got_path;
                }
                else                        /* back up over this component  */
                {
                    while (*(--new_path) != '/')
                        ;
                }

                /*
                 *  Prepend symlink contents to path.
                 */

                memmove
                (
                    copy_path + (PATH_MAX-1) - link_len - path_len,
                    copy_path, link_len
                );
                path = copy_path + (PATH_MAX-1) - link_len - path_len;
            }
            __set_errno(sv_errno);
        }
#endif                            /* S_IFLNK */

        *new_path++ = '/';
    }

    /*
     *  Delete trailing slash but don't whomp a lone slash.
     */

    if (new_path != got_path + 1 && new_path[-1] == '/')
        new_path--;

    *new_path = '\0';                   /* make sure it's null terminated   */
    return got_path;
}

/*
 * realpath.c
 *
 * vim: sw=4 ts=4 wm=4 et ft=c
 */

