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
 * \file          ftswalker.cpp
 *
 *    This module ...
 *
 * \library       ftswalker
 * \author        Chris Ahlstrom
 * \date          2025-03-10
 * \updates       2025-03-10
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *   To do.
 */

#include <cerrno>                       /* #include <errno.h>               */
#include <cstring>                      /* std::strerror()                  */
#include <cstdlib>                      /* std::getenv(), std::rand()       */
#include <fts.h>                        /* function to traverse directories */

#include "util/ftswalker.hpp"           /* file-tree traversal declarations */
#include "util/filefunctions.hpp"       /* cfg66: util::file_write_lines()  */
#include "util/msgfunctions.hpp"        /* cfg66: util::error_message() etc */
#include "util/strfunctions.hpp"        /* cfg66: util::strcompare() etc    */

namespace
{

/**
 *  The code from NSM incorrectly treats these as bits, but fts.h defines
 *  them as integers ranging from 1 to 14.
 */

bool
is_directory (const FTSENT * entry)
{
    return entry->fts_info == FTS_D;
}

bool
is_regular_file (const FTSENT * entry)
{
    return entry->fts_info == FTS_F;
}

bool
is_leaving_directory  (const FTSENT * entry)
{
    return entry->fts_info == FTS_DP;
}

}       // namespace

namespace util
{


/**
 *  The argument compare_whatever() specifies a user-defined function
 *  which can be used to order the traversal of the hierarchy.
 *
 *  It takes two pointers to pointers to FTSENT structures as arguments and
 *  should return one of the following values to indicate if the file
 *  referenced by its first argument comes before, in any order with respect
 *  to, or after, the file referenced by its second argument.
 *
 *      -   A negative value. First comes before second.
 *      -   Zero. First and second aren't ordered.
 *      -   A positive value. First comes after second.
 *
 *  The fts_accpath, fts_path, and fts_pathlen fields of the FTSENT
 *  structures are never to be used in this comparison.
 *
 *  If the fts_info field is set to FTS_NS or FTS_NSOK, the
 *  fts_statp field may not be used either.
 *
 *  If the compare_whatever() argument is NULL, the directory traversal order
 *  is in the order listed in path_argv for the root paths, and in the order
 *  listed in the directory for everything else.
 */

int
compare_files_before_dirs
(
    const FTSENT ** first,
    const FTSENT ** second
)
{
    if (is_regular_file(*first))
    {
        return (-1);                                    /* first    */
    }
    else if (is_regular_file(*second))
    {
        return 1;                                       /* last     */
    }
    else
    {
        return strcmp((*first)->fts_name, (*second)->fts_name);
    }
}

/**
 *  Parse the rootdir recursively for files with the given name.
 *
 *  Configurations can be structured with sub-directories. The file target
 *  will mark a real configuration file and should be a 'leaf' of the tree.
 *
 *  No other directories are allowed below a directory containing
 *  the target file.
 *
 *  We use fts to walk the root, plus an array of paths to traverse.
 *  Each path must be null terminated and the list must end with a NULL
 *  pointer.
 *
 *  fts():
 *
 *      Two  structures (and associated types) are defined in <fts.h>.
 *      The first is FTS, a structure representing the file hierarchyitself.
 *      The second type is FTSENT,the structure representing a file
 *      in the hierarchy. Normally, an FTSENT structure is returned for
 *      every file in the file hierarchy.
 *
 *      -   1st parameter: The list of paths to traverse. Here, we
 *          supply only one path, the rootdir parameter.
 *      -   2nd parameter: An options parameter. Must include either
 *          FTS_PHYSICAL or FTS_LOGICAL to change how symbolic links
 *          are handled.
 *      -   3rd parameter: a optional comparator to change the traversal
 *          of the filesystem hierarchy.
 *
 *      The comparator used here processes files before directories; we
 *      depend on that to remember if we are already in a
 *      configuration-directory.
 *
 * Handle Types of Files:
 *
 *      Given an entry, determine if it is a file or a directory via
 *      these bits:
 *
 *          -   FTS_D       "Entering a directory".
 *          -   FTS_DP      "Leaving a directory".
 *          -   FTS_F       "A regular file".
 *          -   FTS_SKIP    "Skip descendants of this directory".
 *
 * \param rootdir
 *      The single directory to traverse.
 *
 * \param target
 *      The base-name of the target file, which also marks the sub-directory
 *      as a leaf directory.
 *
 * \return
 *      Returns true if the traversal succeeced in finding a (first)
 *      match.
 */

/**
 *  This file traversal is meant to be used by NSM. It seems to have
 *  a weird goal, can't quite figure it out.
 */

bool
fts_find_file
(
    const std::string & rootdir,
    const std::string & target
)
{
    bool result = false;
    char * const paths [] =
    {
        STR(rootdir), nullptr
    };
    FTS * ftsp = fts_open
    (
        paths, FTS_LOGICAL,
        compare_files_before_dirs           /* defined above                */
    );
    if (ftsp == NULL)
    {
        util::error_message("fts_open() failed");
        return false;
    }
    util::info_message("Getting file list");

    /*
     * The loop will call fts_read() enough times to get each file.
     */

    FTSENT * currententry = NULL;
    for (;;)
    {
        FTSENT * ent = fts_read(ftsp);      /* get next file or directory   */
        if (ent == NULL)
        {
            if (errno == 0)                 /* no more items, done          */
            {
                break;
            }
            else
            {
                util::error_message("fts_read()", "failed");
                return false;
            }
        }
        if (is_directory(ent))              /* check file types; see banner */
        {
            if (currententry != NULL)
            {
                /*
                 * Set that no descendants of this file are visited.
                 */

                int err = fts_set(ftsp, ent, FTS_SKIP);
                if (err != 0)
                {
                    util::error_message("fts_set() failed");
                    return false;
                }
            }
        }
        else if (is_leaving_directory(ent))
        {
            util::info_message("Leaving directory", ent->fts_path);
            if (ent == currententry)
                currententry = NULL;
        }
        else if (is_regular_file(ent))
        {
            std::string base = util::filename_base(ent->fts_path);
            if (util::strcompare(target, base))
            {
                std::string p = ent->fts_path;          /* target path      */

                /*
                 * This code starts at the end of the session-root par of
                 * the file-name and copies the rest of the root to the
                 * beginning.
                 *
                 *      memmove
                 *      (
                 *          s, s + strlen( rootdir ) + 1,
                 *          (strlen( s ) - strlen( rootdir )) + 1
                 *      );
                 *
                 * It basically gets the base filename.
                 */

                std::string rest = p;
                rest = rest.substr(rootdir.length() + 1);
                util::info_printf("Path %s, file %s", V(p), V(rest));
                result = true;
                break;

                /*
                 * Save the parent directory entry, not the target entry.
                 *
                 * currententry = ent->fts_parent;
                 */
            }
        }
    }

    /*
     * Close fts and check for error from the closing.
     */

    if (fts_close(ftsp) == (-1))
        util::error_message("fts_close() failed");

    /*
     * As a marker that all sessions were sent, reply with an empty string,
     * which is impossible to conflict with a session name.
     */

    return result;
}

}           // namespace seq66

/*
 * ftswalker.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

