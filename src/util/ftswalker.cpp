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
 *      This module uses the "fts" functions (in Linux) to traverse a directory
 *      hierarchy and do a few things with it.
 *
 * \library       ftswalker
 * \author        Chris Ahlstrom
 * \date          2025-03-10
 * \updates       2025-04-13
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *      This module defines the util::ftswalker class to manage some aspects
 *      of file-tree walking, as well as a few free functions in the util
 *      and anonymous namespaces.
 */

#include <cerrno>                       /* #include <errno.h>               */
#include <cstring>                      /* std::strerror()                  */
#include <cstdlib>                      /* std::getenv(), std::rand()       */
#include <fts.h>                        /* directory-traversal functions    */

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
is_leaving_directory (const FTSENT * entry)
{
    return entry->fts_info == FTS_DP;
}

bool
is_other_file_type (const FTSENT * entry)
{
    return entry->fts_info == FTS_DEFAULT;
}

bool
is_fts_error (const FTSENT * entry)
{
    return
        entry->fts_info == FTS_ERR || entry->fts_info == FTS_DNR ||
        entry->fts_info == FTS_NS
        ;
}

/**
 *  We need to allow FTS_DP to indicate all subs in the directory
 *  have been traversed.
 */

bool
is_actionable_file (const FTSENT * entry)
{
    return ! is_fts_error(entry) && entry->fts_info != FTS_DOT;
}

/**
 *  This function maps all error returns to FTS::ERR.
 */

util::ftswalker::FTS
get_fts_type (const FTSENT * entry)
{
    util::ftswalker::FTS result;
    switch (entry->fts_info)
    {
        case FTS_D:

            result = util::ftswalker::FTS::D;
            break;

        case FTS_DC:

            result = util::ftswalker::FTS::DC;
            break;

        case FTS_DEFAULT:

            result = util::ftswalker::FTS::DEFAULT;
            break;

        case FTS_DOT:

            result = util::ftswalker::FTS::DOT;
            break;

        case FTS_DP:

            result = util::ftswalker::FTS::DP;
            break;

        case FTS_DNR:
        case FTS_ERR:
        case FTS_NS:

            result = util::ftswalker::FTS::ERR;
            break;

        case FTS_F:

            result = util::ftswalker::FTS::F;
            break;

        case FTS_NSOK:

            result = util::ftswalker::FTS::NSOK;
            break;

        case FTS_SL:

            result = util::ftswalker::FTS::SL;
            break;

        case FTS_SLNONE:

            result = util::ftswalker::FTS::SLNONE;
            break;

        default:

            result = util::ftswalker::FTS::UNKNOWN;
            break;
    }
    return result;
}

/**
 *  Gets a brief description of an FTS file type.
 */

std::string
get_fts_type_name (util::ftswalker::FTS typevalue)
{
    std::string result;
    switch (typevalue)
    {
        case util::ftswalker::FTS::D:

            result = "Directory";
            break;

        case util::ftswalker::FTS::DC:

            result = "Directory cycle";
            break;

        case util::ftswalker::FTS::DEFAULT:

            result = "Other file type";
            break;

        case util::ftswalker::FTS::DOT:

            result = "Dot file";
            break;

        case util::ftswalker::FTS::DP:

            result = "Postorder directory";
            break;

        case util::ftswalker::FTS::DNR:

            result = "Directory read error";
            break;

        case util::ftswalker::FTS::ERR:

            result = "Error";
            break;

        case util::ftswalker::FTS::NS:

            result = "No-stat error";
            break;

        case util::ftswalker::FTS::F:

            result = "File";
            break;

        case util::ftswalker::FTS::NSOK:

            result = "No stat request";
            break;

        case util::ftswalker::FTS::SL:

            result = "Symbolic link";
            break;

        case util::ftswalker::FTS::SLNONE:

            result = "Untargeted symbolic link";
            break;

        default:

            result = "Unknown";
            break;
    }
    return result;
}

}           // namespace

namespace util
{

/**
 *  Principal constructor.
 */

ftswalker::ftswalker (const std::string & path) :
    m_search_directories    (),
    m_paths                 (nullptr)
{
    m_search_directories.push_back(path);
    make_paths();

    // to do?
}

ftswalker::ftswalker (const lib66::tokenization & paths) :
    m_search_directories    (paths),
    m_paths                 (nullptr)
{
    make_paths();
}

ftswalker::~ftswalker ()
{
    delete_paths();
}

/**
 *  A generic search to build a list of locations for the target file.
 *  This function searches the path(s) provided during the construction
 *  of this object.
 *
 *  The "compar()" argument is NULL, therefore the directory traversal order
 *  is in the order listed in the root paths parameter, and in the order
 *  listed in the directory for everything else.
 *
 * \param target
 *      Provides the name of the file for which we're searching.
 *
 * \param [out] destination
 *      A vector of strings to hold the full path names of the files
 *      found that matched the target. This item is not cleared,
 *      so one theoretically do a number of searches.
 *
 * \return
 *      Returns true if no error occurred and any files were found.
 *      That is, the destination results can be used.
 */

bool
ftswalker::find_file
(
    const std::string & target,
    lib66::tokenization & destination
)
{
    bool result = not_nullptr(paths());
    if (result)
    {
        ::FTS * ftsp = ::fts_open(paths(), FTS_LOGICAL, NULL);
        if (ftsp == NULL)
        {
            util::error_message("fts_open() failed");
            result = false;
        }
        else
        {
            result = false;                             /* tentative        */
            for (;;)
            {
                ::FTSENT * ent = fts_read_entry(ftsp);  /* next file/dir    */
                if (ent == NULL)
                    break;

                if (is_actionable_file(ent))
                {
                    std::string base = util::filename_base(ent->fts_path);
                    if (util::strcompare(target, base))
                    {
                        std::string p = ent->fts_path;  /* target path  */
                        util::info_message("Path", p);
                        destination.push_back(p);
                        result = true;
                    }
                }
                else if (is_fts_error(ent))
                {
                    std::string errmsg = strerror(ent->fts_errno);
                    util::error_message(errmsg, ent->fts_path);
                }
            }
            if (::fts_close(ftsp) == (-1))
                util::error_message("fts_close() failed");
        }
    }
    return result;
}

/**
 *  This function searches the path(s) provided during the construction
 *  of this object. Unlike find_file() above, it does not look for a
 *  specific and collect the occurrences; instead it gets all the regular
 *  files in the directory and collects them.
 *
 * \param [out] destination
 *      A vector of strings to hold the full path names of the files
 *      found that matched the target. This item is not cleared,
 *      so one theoretically do a number of searches.
 *
 * \return
 *      Returns true if no error occurred and any files were found.
 *      That is, the destination results can be used.
 */

bool
ftswalker::find_regular_files (lib66::tokenization & destination)
{
    bool result = not_nullptr(paths());
    if (result)
    {
        ::FTS * ftsp = ::fts_open(paths(), FTS_LOGICAL, NULL);
        if (ftsp == NULL)
        {
            util::error_message("fts_open() failed");
            result = false;
        }
        else
        {
            result = false;                             /* tentative        */
            for (;;)
            {
                ::FTSENT * ent = fts_read_entry(ftsp);  /* next file/dir    */
                if (ent == NULL)
                    break;

                if (is_regular_file(ent))
                {
                    std::string p = ent->fts_path;      /* target path      */
                    util::info_message("File", p);
                    destination.push_back(p);
                    result = true;
                }
                else if (is_fts_error(ent))
                {
                    std::string errmsg = strerror(ent->fts_errno);
                    util::error_message(errmsg, ent->fts_path);
                }
            }
            if (::fts_close(ftsp) == (-1))
                util::error_message("fts_close() failed");
        }
    }
    return result;
}

/**
 *  As all the directories and files are traversed, this function calls
 *  the given ftswalker function.
 *
 * \param fn
 *      Provides the free function or static function to be called for
 *      every entry. It should at least handle the values in the
 *      ftswalker::FTS enumeration.
 *
 * \param target
 *      If not empty, then only matching files will be processed.
 *      The default is empty.
 *
 * \param cfn
 *      If not null (the default), this function is used in ordering the
 *      traversal. By default, the directory traversal order is in the
 *      order listed in the root paths, and in the order listed in the
 *      directory for everything else.
 *
 * \return
 *      If all the calls to \a fn return true, this function returns true.
 */

bool
ftswalker::process_files
(
    function fn,
    const std::string & target,
    comparator cfn
)
{
    bool result = not_nullptr(paths()); /* ! m_search_directories.empty();  */
    if (result)
    {
        ::FTS * ftsp = ::fts_open(paths(), FTS_LOGICAL, cfn);
        if (ftsp == NULL)
        {
            util::error_message("fts_open() failed");
            result = false;
        }
        if (result)
        {
            /*
             * util::info_message("Getting file list"); // TMI!
             */

            for (;;)
            {
                ::FTSENT * ent = fts_read_entry(ftsp);  /* next file/dir    */
                if (ent == NULL)
                    break;

                if (is_actionable_file(ent))
                {
                    bool process_it = true;
                    if (! target.empty())
                    {
                        std::string base = util::filename_base(ent->fts_path);
                        process_it = util::strcompare(target, base);
                    }
                    if (process_it)
                    {
                        std::string p = ent->fts_path;  /* target path      */
                        FTS ft = get_fts_type(ent);
                        result = fn(p, ft);
                        if (! result)
                            break;
                    }
                }
                else if (is_fts_error(ent))
                {
                    std::string errmsg = strerror(ent->fts_errno);
                    util::error_message(errmsg, ent->fts_path);
                }
            }
            if (::fts_close(ftsp) == (-1))
                util::error_message("fts_close() failed");
        }
    }
    return result;
}

/**
 *  Here is how the processing works. The path starts at "tests/data/fts",
 *  and the target is "build/tests" == $DEST:
 *
 *      1.  Get the current entry. Four examples are shown:
 *
 *          a.  "tests/data/fts"
 *          b.  "tests/data/fts/session.fts"
 *          c.  "tests/data/fts/session_2"
 *          d.  "tests/data/fts/session_2/session.fts"
 *
 *      2.  Assemble the destinations as encountered in "compare files
 *          before directories" order:
 *
 *          a.  "$DEST/fts". The source, "tests/data/fts" is a directory.
 *
 *              i.  Get the file-name base "fts" from the source by a
 *                  split with the "no-extension" option.
 *             ii.  Append it to $DEST.
 *            iii.  Create the directory.
 *             iv.  If successful, store "$DEST/fst" as the last-found
 *                  destination, $LASTDEST. This is done in process_file().
 *
 *          b.  "$DEST/fts/session.fts". This will be a file.
 *
 *              i. Split out the source path and file-name ("session.fts").
 *             ii. Verify that the destination directory ($LASTDEST) exists.
 *            iii.  Copy or process the file to the destination.
 *
 *          c.  "$DEST/fts/session_2". The source is a directory,
 *              "tests/data/fts/session_2".
 *
 *              i.  Get the file-name base "session_2" from the source by a
 *                  split with the "no-extension" option.
 *             ii.  Append it to $LASTDEST.
 *            iii.  Create the directory.
 *             iv.  If successful, store "$DEST/fst/session_2" as $LASTDEST.
 *
 *          d.  "$DEST/fts/session_2/session.fts". This will be a file.
 *
 *              i.  Split out the path and the filename.
 *             ii. Verify that the destination directory ($LASTDEST) exists.
 *            iii.  Copy the file to the destination.
 *
 * \param fn
 *      The function that takes the source file/directory as retrieved
 *      by fts_read_entry() and the target directory and does some cross
 *      processing, such as copying. Remember that the source directory
 *      comes from the paths() accessor.
 *
 * \param target
 *      The destination for the processed file. An example would be
 *      "build/tests".
 *
 * \param cfn
 *      If not null (the default), this function is used in ordering the
 *      traversal. By default, the directory traversal order is in the
 *      order listed in the root paths, and in the order listed in the
 *      directory for everything else.
 *
 * \return
 *      Returns true if no errors occurred.
 */

bool
ftswalker::process_files
(
    bifunction fn,
    const std::string & target,
    comparator cfn
)
{
    bool result = not_nullptr(paths()) && util::file_is_directory(target);
    if (result)
    {
        ::FTS * ftsp = ::fts_open(paths(), FTS_LOGICAL, cfn);
        if (ftsp == NULL)
        {
            util::error_message("fts_open() failed");
            result = false;
        }
        if (result)
        {
            bool exited_directory = false;
            std::string lastdest = target;
            for (;;)
            {
                ::FTSENT * ent = fts_read_entry(ftsp);  /* next file/dir    */
                if (ent == NULL)
                    break;

                if (is_actionable_file(ent))
                {
                    bool process_it = ! target.empty();
                    if (process_it)
                    {
                        std::string p = ent->fts_path;
                        FTS ft = get_fts_type(ent);
                        if (ft == FTS::D)
                        {
                            if (exited_directory)
                            {
                                exited_directory = false;
                                lastdest = util::get_parent_directory(lastdest);
                            }
                            lastdest = util::filename_target(p, lastdest);
                            if (lastdest.empty())
                            {
                                result = false;
                                break;
                            }
                        }
                        else if (ft == FTS::F || ft == FTS::SL)
                        {
                            // all work done in the fn() bifunction callback
                        }
                        else if (ft == FTS::DP)
                        {
                            exited_directory = true;
                        }
                        result = fn(p, lastdest, ft);    /* bifunction       */
                        if (! result)
                            break;
                    }
                }
                else if (is_fts_error(ent))
                {
                    std::string errmsg = strerror(ent->fts_errno);
                    util::error_message(errmsg, ent->fts_path);
                }
            }
            if (::fts_close(ftsp) == (-1))
                util::error_message("fts_close() failed");
        }
    }
    return result;
}

/**
 *  This private function creates the array of character pointers, with the
 *  last one null, needed by the fts_open() routine. The pointers are
 *  the c_str() values of the strings in the search-directories tokenization.
 */

void
ftswalker::make_paths ()
{
    int count = int(m_search_directories.size());
    delete_paths();
    if (count > 0)
    {
        char ** p = new (std::nothrow) char * [count + 1];  /* with nullptr */
        m_paths = p;
        if (not_nullptr(m_paths))
        {
            int i = 0;
            for (const auto & s : m_search_directories)
                m_paths[i++] = STR(s);

            m_paths[count] = nullptr;
        }
    }
}

/**
 *  Removes the array of character pointers (but not the pointers
 *  themselves :-)
 */

void
ftswalker::delete_paths ()
{
    if (not_nullptr(m_paths))
    {
        delete [] m_paths;
        m_paths = nullptr;
    }
}

/*-------------------------------------------------------------------------
 * Free functions in the util namespace
 *-------------------------------------------------------------------------*/

/**
 *  Helper function to save some lines of code.
 */

::FTSENT *
fts_read_entry (::FTS * ftsp)
{
    ::FTSENT * ent = ::fts_read(ftsp);  /* get next file/directory entry    */
    if (ent == NULL)
    {
        if (errno == 0)                 /* no more items, done              */
        {
            /*
             * Nothing to do.
             */
        }
        else
        {
            util::error_message("fts_read()", "failed");
        }
    }
    return ent;
}

/**
 *  Just a simple test callback function; see tests/ftswalker_test.
 */

bool
fts_show_target (const std::string & match, util::ftswalker::FTS ft)
{
    std::string t = get_fts_type_name(ft);
    std::string m = match.empty() ? "---" : match ;
    if (t.empty())
        t = "?";                        /* this should not happen           */

    util::status_message(t, m);
    return true;
}

/**
 *  An ftswalker::bifunction callback for copying a file or directory.
 *
 * \param source
 *      Provides the current file or directory obtained via fts_read().
 *
 * \param destination
 *      Provides the destination for the source file, which can be
 *      the full file-specification, or just the destination directory.
 *
 * \return
 *      Returns true if the operation succeeded.
 */

bool
fts_item_copy
(
    const std::string & source,
    const std::string & target,
    util::ftswalker::FTS ft
)
{
    bool result = true;                             /* no action is okay    */
    if (ft == util::ftswalker::FTS::D)              /* first directory      */
    {
        util::info_message("Entered directory", source);
        result = util::make_directory_path(target);
    }
    else if (ft == util::ftswalker::FTS::F)         /* a regular file       */
    {
        util::info_printf("Copying file %s to %s", V(source), V(target));
        result = util::file_copy_to_path(source, target);
    }
    else if (ft == util::ftswalker::FTS::SL)        /* a symbolic link      */
    {
        util::info_printf("Copying file %s to %s", V(source), V(target));
        result = util::file_copy_to_path(source, target);
    }
    else if (ft == util::ftswalker::FTS::SLNONE)    /* link with no target  */
    {
        util::info_message("Empty soft link", source);
    }
    else if (ft == util::ftswalker::FTS::DP)        /* last directory       */
    {
        // nothing yet
    }
    return result;
}

/**
 *  Copies a directory hierarchy to another directory. See the
 *  fts_copy_test() function in the ftswalker_test program
 *  for more explanation.
 */

bool
fts_copy_directory (const std::string & source, const std::string & dest)
{
    bool result = file_is_directory(source) && file_is_directory(dest);
    if (result)
    {
        util::ftswalker walker(source);
        result = walker.process_files
        (
            fts_item_copy, dest, util::compare_files_before_dirs
        );
    }
    return result;
}

/**
 *  An ftswalker::function callback for deleting a file, directory,
 *  and directory hierarchy.
 *
 *  Note that a directory cannot be deleted until all its children
 *  are deleted, which is done once the file type is FTS::DP.
 */

bool
fts_item_delete (const std::string & item, util::ftswalker::FTS ft)
{
    bool result = file_exists(item);
    if (result)
    {
        if (ft == util::ftswalker::FTS::D)              /* first directory  */
        {
            /*
             * TMI: util::info_message("Entered directory", item);
             */
        }
        else if (ft == util::ftswalker::FTS::DP)        /* last directory   */
        {
            util::info_message("Deleting directory", item);
            result = util::delete_directory(item);
        }
        else if (ft == util::ftswalker::FTS::F)         /* a regular file   */
        {
            util::info_message("Deleting file", item);
            result = util::file_delete(item);
        }
        else if (ft == util::ftswalker::FTS::SL)        /* a symbolic link  */
        {
            // TODO
        }
        else if (ft == util::ftswalker::FTS::SLNONE)    /* link w/no target */
        {
            util::info_message("Deleting ophan link", item);
            result = util::file_delete(item);
        }
    }
    return result;
}

/**
 *  This function removes the files and directories it encounters, including
 *  sub-directories.
 *
 *  In general, directories are visited two distinguishable times; in
 *  preorder (before any of their descendants are visited) and in
 *  postorder (after all of their descendants have been visited).
 *  Files are visited once.  It is possible to walk the hierarchy
 *  "logically" (visiting the files that symbolic links point to) or
 *  physically (visiting the symbolic links themselves), order the
 *  walk of the hierarchy or prune and/or revisit portions of the
 *  hierarchy.
 *
 *  Note that this function is not a callback.
 */

bool
fts_delete_directory (const std::string & path)
{
    bool result = file_is_directory(path);
    if (result)
    {
        util::ftswalker walker(path);
        result = walker.process_files(fts_item_delete);
    }
    return result;
}

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
    const ::FTSENT ** first,
    const ::FTSENT ** second
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
    ::FTS * ftsp = fts_open
    (
        paths, FTS_LOGICAL,
        compare_files_before_dirs           /* a comparator, defined above  */
    );
    if (ftsp == NULL)
    {
        util::error_message("fts_open() failed");
        return false;
    }

    /*
     * util::info_message("Getting file list"); // TMI!
     */

    /*
     * The loop will call fts_read() enough times to get each file.
     */

    ::FTSENT * currententry = NULL;
    for (;;)
    {
        ::FTSENT * ent = ::fts_read(ftsp);  /* get next file or directory   */
        if (ent == NULL)
            break;

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
                 * This code basically gets the base filename.
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
        else if (is_other_file_type(ent))
        {
            util::info_message
            (
                "Non-directory/file", std::string(ent->fts_path)
            );
        }
    }

    /*
     * Close fts and check for error from the closing.
     */

    if (::fts_close(ftsp) == (-1))
        util::error_message("fts_close() failed");

    /*
     * As a marker that all sessions were sent, reply with an empty string,
     * which is impossible to conflict with a session name.
     */

    return result;
}

#if defined THIS_CODE_IS_READY

#endif

}           // namespace util

/*
 * ftswalker.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */

