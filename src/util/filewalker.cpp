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
 * \file          filewalker.cpp
 *
 *      This module uses the <filesystem> functions to traverse a
 *      directory hierarchy and do a few things with it.
 *
 * \library       filewalker
 * \author        Chris Ahlstrom
 * \date          2026-04-02
 * \updates       2026-05-20
 * \version       $Revision$
 * \license       GNU GPL v2 or above
 *
 *      This module defines the util::filewalker class to manage some aspects
 *      of file-tree walking, as well as a few free functions in the
 *      util and anonymous namespaces.
 *
 *      Implementing the GNU File Tree Walk (FTS) API using std::filesystem
 *      involves using std::filesystem::recursive_directory_iterator to
 *      emulate the fts_open(), fts_read(), and fts_close() paradigm,
 *      to provide high-level directory traversal.
 *
 *      -   fts_open() / fts_read() are replaced by
 *          std::filesystem::recursive_directory_iterator.
 *      -   FTSENT * (entry info) is replaced by
 *          std::filesystem::directory_entry (accessed via *iterator).
 *      -   Depth Information is accessed via iterator.depth().
 *      -   Error Handling is managed using
 *          std::filesystem::filesystem_error exceptions.
 *      -   Use std::filesystem::path for portable paths.
 *      -   Use symlink_options in the iterator constructor to mimic
 *          FTS_LOGICAL or FTS_PHYSICAL.
 *      -   Requires C++17 or above.
 *
 *  file_status (const path & p (string_type && s, format = auto_format))
 *
 *      where string_type is a C-type string, a basic_string, a
 *      basic_string_view, or a pair of iterators.
 *
 * FTS review:
 *
 *      FTS_D       A directory being visited in preorder.
 *      FTS_DC      A directory that causes a cycle in the tree.
 *      FTS_DEFAULT An FTSENT that represents an "other type" of file.
 *      FTS_DNR     A directory which cannot be read. This is an error return.
 *      FTS_DOT     A file named "."  or ".."  not specified in fts_open().
 *      FTS_DP      A directory being visited in postorder.
 *      FTS_ERR     An error return, fts_errno set.
 *      FTS_F       A regular file.
 *      FTS_NS      A file with no stat(2) information available; an error.
 *      FTS_NSOK    A file for which no stat(2) information was requested.
 *      FTS_SL      A symbolic link.
 *      FTS_SLNONE Symbolic link with a nonexistent target.
 *
 * filewalker function survey:
 *
 *  -   Class util::filewalker functions:
 *      -   Constructors. Adds one or more paths to be searched.
 *      -   find_files(). Looks for files in the search-paths that
 *          match a target (which could be a regex), and adds them
 *          to a string vector.
 *      -   find_regular_files(). Looks for all regular files (except
 *          "." and "..") in the search-paths and adds them to a vector.
 *      -   traverse(). Iterates through a directory, calling a
 *          filewalking::function for each file.
 *      -   process_path(). Iterates through one path, calling
 *          filewalker::function for each file matching a target.
 *      -   process_files(). Calls process_path() for each search-path.
 *      -   process_bi_path(). Iterates through one path, calling
 *          a filewalker::bifunction for each file, to transform or copy
 *          one file to another.
 *      -   process_bi_files(). Calls process_bi_path() for each search-path.
 *
 *  -   Free functions in the util::file namespace
 *      -   compare_files_before_dirs(). An FTS concept not yet used.
 *      -   is_actionable_file(). Is an entry regular or directory?
 *      -   Useful callback functions:
 *          -   show_target(). Shows the file-name and type.
 *          -   show_directory_entry(). Shows the file-name and type.
 *          -   item_copy(). Given a source and a target, copies
 *              the file or makes the directory at the target.
 *          -   item_delete(). Deletes the file or removes the directory
 *              (if empty).
 *      -   copy_directory_tree(). Collects file and directory names,
 *          makes the directories at the target, then copies the files.
 *      -   delete_directory(). Simply calls remove_all() on the path.
 *      -   delete_directory_tree(). Collects the files then calls
 *          delete_collection().
 *      -   collect_files_from_path(). Copies the directory/file-names
 *          into a filewalker::pairs collection. Includes root path by
 *          default.
 *      -   collection_to_string(). Copies all entries to a string.
 *      -   delete_collection(). First deletes the files in the collection,
 *          then removes directories upwardly.
 *      -   find_file(). Determines if a file/directory exists in the path.
 *      -   find_files_by_regex(). Searchs all given paths, and collects into
 *          a string vector all file/regex matching items.
 *      -   get_type_name(). Converts a filesystem::file_type to a name.
 *      -   get_last_directory(). Returns the last sub-directory in a path
 *          name.
 *      -   build_destination_path(). Given a full path and a root
 *          subdirectory name, plus a destination directory, assembles the
 *          corresponding destination directory name.
 */

#include <iostream>                     /* std::cout, std::cerr             */

#include "platform_macros.h"            /* PLATFORM_DEBUG macro             */
#include "util/filewalker.hpp"          /* file-tree traversal declarations */
#include "util/filefunctions.hpp"       /* cfg66: util::file_write_lines()  */
#include "util/msgfunctions.hpp"        /* cfg66: util::error_message() etc */
#include "util/strfunctions.hpp"        /* cfg66: util::strcompare() etc    */

/**
 */

namespace util
{

/**
 *  Principal constructor.
 */

filewalker::filewalker (const std::string & path) :
    m_search_paths    ()
{
    m_search_paths.push_back(path);
}

filewalker::filewalker (const lib66::tokenization & paths) :
    m_search_paths    (paths)
{
    // no code
}

/**
 *  A generic search to build a list of locations for the target file.
 *  This function searches the path(s) provided during the construction
 *  of this object.
 *
 * \param target
 *      Provides the base-name of the file for which we're searching.
 *      This can also be a regular expression. It can be a base-name
 *      or a full path-name. If the latter, then only one item
 *      can be found.
 *
 *      If target satisfies the util::string_has_regex() function, then
 *      the full path is checked. Otherwise, the base-name is used for
 *      an exact match.
 *
 * \param [out] destination
 *      A vector of strings to hold the full path names of the files
 *      found that matched the target. These are absolute path-names.
 *      This item is not cleared, so one can theoretically do a number of
 *      searches to accumulate the results.
 *
 * \param notregex
 *      If true (the default is false), then the caller knows that
 *      the target is not a regular expression. The presence of a
 *      dot or other regex characters in a UNIX file-name can be
 *      misleading.
 *
 * \return
 *      Returns true if no error occurred and any files were found.
 *      That is, the destination results can be used.
 */

bool
filewalker::find_files
(
    const std::string & target,
    lib66::tokenization & destination,
    bool notregex
)
{
    bool result { ! target.empty() && search_paths().size() > 0 };
    if (result)
    {
        bool rgxtarget { util::string_has_regex(target) };
        if (notregex)
            rgxtarget = false;

        result = false;
        for (const auto & path : search_paths())
        {
            try
            {
                for
                (
                    const auto & entry :
                        std::filesystem::recursive_directory_iterator(path)
                )
                {
                    std::filesystem::path p { entry.path() };
                    std::filesystem::path fp { std::filesystem::absolute(p) };
                    std::string relpath { p.string() };
                    std::string fullpath { fp.string() };
                    std::string basename { p.filename().string() };
                    const std::string & t { rgxtarget ? fullpath : basename };
                    bool match { util::regex_match(target, t) };

#if defined PLATFORM_DEBUG_TMI
                    printf
                    (
                        "Item: %s [%s]\n", CSTR(basename), CSTR(fullpath)
                    );
#endif
                    if (match)
                    {
                        destination.push_back(relpath); // .push_back(fullpath)
                        result = true;
                    }
                }
            }
            catch (const std::filesystem::filesystem_error & e)
            {
                util::error_message("find_files() failed", e.what());
                result = false;
                break;
            }
        }
    }
    return result;
}

/**
 *  This function searches the path(s) provided during the construction
 *  of this object. Unlike find_file() above, it does not look for a
 *  specific target and collect the occurrences; instead it gets all
 *  the regular files in the directory and collects them.
 *
 * A no-dots regex (to skip "." and ".."):
 *
 *      ^(?!\.{1,2}$)(.+)
 *
 *          ^       Anchor to the start of the string.
 *          (       Begin grouping
 *          ?       Optional (zero or one)
 *          !       Negation (see below)
 *          \.      Dot
 *          {1,2}   At least 1 dot, at most 2 dots
 *          $       End of string
 *          )       End grouping
 *          (.+)    Matches a string of one or more characters
 *
 *      (?!...)     Negative look-ahead, the "..." is not matched
 *      (?=...)     Positive look-ahead, the "..." is matched
 *
 *  Note in the regex in the function body that '(R"( ... )")' is the
 *  raw string literal that allows using a single backslash for regex
 *  escapes.
 *
 * \param [out] destination
 *      A vector of strings to hold the full path names of the files
 *      found that are regular files. This item is not cleared,
 *      so one theoretically do a serial number of searches.
 *
 * \return
 *      Returns true if no error occurred and any files were found.
 *      That is, the destination results can be used.
 */

bool
filewalker::find_regular_files (lib66::tokenization & destination)
{
    bool result { search_paths().size() > 0 };
    if (result)
    {
        static const std::string s_no_dots_regex { R"(^(?!\.{1,2}$)(.+))" };
        result = false;
        for (const auto & path : search_paths())
        {
            try
            {
                for
                (
                    const auto & entry :
                        std::filesystem::recursive_directory_iterator(path)
                )
                {
                    std::filesystem::file_type ft { entry.status().type() };
                    if (ft == std::filesystem::file_type::regular)
                    {
                        std::filesystem::path p { entry.path() };
                        std::string basename { p.filename().string() };
                        bool match
                        {
                            util::regex_match(s_no_dots_regex, basename)
                        };
                        if (match)
                        {
                            std::filesystem::path fp
                            {
                                std::filesystem::absolute(p)
                            };
                            std::string fullpath { fp.string() };
                            destination.push_back(fp.string()); /* full path */
                            result = true;
                        }
                    }
                }
            }
            catch (const std::filesystem::filesystem_error & e)
            {
                util::error_message("find_regular_files() failed", e.what());
                result = false;
                break;
            }
        }
    }
    return result;
}

/**
 *  The traverse() function simply shows the files and type in the
 *  given path.
 *
 *  The recursive iterator handles the "depth-first" traversal. The type
 *  of each item iterated is std::filesystem::directory_entry, similar
 *  to FTSENT.
 */

bool
filewalker::traverse (function fn, const std::string & path) const
{
    bool result { std::filesystem::exists(path) };
    if (result)
    {
        try
        {
            /*
             * We must use an iterator to get the depth(). Feh!
             * Note the tricky use of a free end() function.
             */

            for
            (
                auto it = std::filesystem::recursive_directory_iterator(path);
                it != std::filesystem::end(it); ++it
            )
            {
                /*
                 * Get file attributes similar to fts_ent. One trick
                 * is that the iterator, not the directory entry,
                 * has a depth() function.
                 */

                const auto & entry = *it;
                std::filesystem::file_type ft { entry.status().type() };
                std::string indent { std::string(it.depth() * 4, ' ') };
                std::string dep { std::to_string(it.depth()) };
                std::string desc
                {
                    dep + " " + indent + entry.path().filename().string()
                };
                result = fn(desc, ft);
            }
        }
        catch (const std::filesystem::filesystem_error & e)
        {
            util::error_message("traverse() failed", e.what());
            result = false;
        }
    }
    return result;
}

/**
 *  Processes the files found in the given path. Remember that the paths
 *  are set up in the filewalker constructor.
 *
 * increment():
 *
 *      Advances the iterator to the next entry. Invalidates all copies of
 *      the previous value of *this.
 *
 *      If there are no more entries left in the currently iterated
 *      directory, the iteration is resumed over the parent directory. The
 *      process is repeated if the parent directory has no sibling entries
 *      that can to be iterated on. If the parent of the directory hierarchy
 *      that has been recursively iterated on is reached (there are no
 *      candidate entries at depth() == 0), *this is set to an end iterator.
 *
 *      Otherwise, if *this refers to a directory, it is iterated into if the
 *      following conditions are met:
 *
 *      -   disable_recursion_pending() has not been called before this
 *          increment, i.e. recursion_pending() == true.
 *      -   The directory is not a symlink or following symlinks is enabled,
 *          qi.e., at least one of the following is true:
 *
 *          ! is_symlink((*this)->symlink_status()).
 *
 *          (options() & directory_options::follow_directory_symlink) !=
 *              qdirectory_options::none)
 *
 * Anyway, the problem we have is when deleting directories containing
 * files. The increment() call seems not to work as we expect. So, we
 * have to provide different methods for deletion. For a known, single
 * directory, we can call util::file::delete_directory(), which also
 * removes the contents of the directory.
 *
 * For deleting items selected by a regular expression, this function
 * can be called with the item_delete() callback.
 *
 * For iterating through an unknown tree of directories, we provide
 * the member function delete_tree().
 *
 * \param path
 *      A path into which to descend and examine or process files.
 *
 * \param fn
 *      A single-string function to call for each match.
 *
 * \param target
 *      A filename or a regular expression to be matched.
 *
 * \param cfn
 *      A comparator function. Currently not used.
 *
 * \return
 *      Returns true if the processing can be run and if all matches
 *      can be successfully processed.
 */

bool
filewalker::process_path
(
    const std::string & path,
    function fn,
    const std::string & target,
    comparator cfn
)
{
    bool result { not_nullptr(fn) };
    (void) cfn;                         /* not yet used                     */
    if (result)
    {
        try
        {
#if defined PLATFORM_DEBUG_TMI
            int count { 0 };
#endif
            std::error_code ec;
            for                         /* get file attributes a la fts_ent */
            (
                auto it = std::filesystem::recursive_directory_iterator(path);
                it != std::filesystem::end(it); // ++it
                it.increment(ec)
            )
            {
                const auto & entry = *it;
                if (util::file::is_actionable_file(entry))
                {
                    bool process_it { true };
                    std::filesystem::path p { entry.path() };
                    const std::filesystem::file_status & fs { entry.status() };
                    std::filesystem::file_type ft { fs.type() };
                    std::filesystem::path fp { std::filesystem::absolute(p) };

                    /*
                     * Hmmmm, fullpath does not match the p.string()'s
                     * expected entries.
                     */

                    std::string relpath { p.string() };
                    std::string fullpath { fp.string() };
                    if (! target.empty())
                    {
                        process_it = util::regex_match(target, fullpath);
                    }
                    if (process_it)
                    {
#if defined PLATFORM_DEBUG_TMI
                        std::string ftname { file::get_type_name(ft) };
                        printf
                        (
                            "[%2d] item = '%s'; %s\n",
                            (count + 1), CSTR(fullpath), CSTR(ftname)
                        );
#endif
                        result = fn(relpath, ft); // result = fn(fullpath, ft);
                        if (! result)
                            break;
                    }
#if defined PLATFORM_DEBUG_TMI
                    ++count;
#endif
                }
            }
#if defined PLATFORM_DEBUG_TMI
            printf("process_path() count = %d\n", count);
#endif
        }
        catch (const std::filesystem::filesystem_error & e)
        {
            util::error_message("process_path() failed", e.what());
            result = false;
        }
    }
    return result;
}

/**
 *  As all the directories and files are traversed, this function calls
 *  the given filewalker function.
 *
 * \param fn
 *      Provides the free function or static function to be called for
 *      every entry. It should at least handle the values in the
 *      std::filesystem::file_type enumeration.
 *
 * \param target
 *      If not empty, then only matching files will be processed.
 *      The default is empty. NEW: the file base ("xyz.txt") is no longer
 *      extracted for the match. Use regular expressions, as
 *      described in util::regex_match() in the strfunctions module.
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
filewalker::process_files
(
    function fn,
    const std::string & rgx,
    comparator cfn
)
{
    bool result { not_nullptr(fn) && ! search_paths().empty() };
    if (result)
    {
        for (const auto & path : search_paths())
        {
            result = process_path(path, fn, rgx, cfn);
            if (! result)
                break;
        }
    }
    return result;
}

/**
 *  Processes the files found in the given path. Remember that the path(s)
 *  are set up in the filewalker constructor.
 *
 *  std::filesystem::recursive_directory_iterator() skips the "." and ".."
 *  entries.
 *
 *  One issue with std::filesystem::recursive_directory_iterator() is is that
 *  it returns a directory entry where only the base name is given. So we need
 *  to concatenate the path and the directory/filename provided by the
 *  iterator. So we have to make the absolute path using
 *  std::filesystem::absolute().
 *
 * \param path
 *      A path into which to descend and examine or process files.
 *      It is assumed to end with a directory name, not a regular
 *      file-name.
 *
 * \param fn
 *      A single-string function to call for each match.
 *
 * \param destination
 *      The directory where the matched files are to go. It can be
 *      an absolute or relative path.
 *
 * \param cfn
 *      A comparator function. Currently not used.
 *
 * \return
 *      Returns true if the processing can be run and if all matches
 *      can be successfully processed.
 */

bool
filewalker::process_bi_path
(
    const std::string & path,
    bifunction bfn,
    const std::string & destination,
    comparator cfn
)
{
    bool result { not_nullptr(bfn) };
    (void) cfn;                         /* not yet used                     */
    if (result)
    {
        std::string lastdir { util::file::get_last_directory(path) };
        std::string basedest { destination };
        if (! lastdir.empty())
            basedest = util::filename_concatenate(destination, lastdir);

        try
        {
#if defined PLATFORM_DEBUG_TMI
            int count { 0 };
#endif
            for                         /* Get file attributes a la fts_ent */
            (
                auto it = std::filesystem::recursive_directory_iterator(path);
                it != std::filesystem::end(it); ++it
            )
            {
                const auto & entry = *it;
                if (util::file::is_actionable_file(entry))
                {
                    std::filesystem::path p { entry.path() };
                    const std::filesystem::file_status & fs { entry.status() };
                    std::filesystem::file_type ft { fs.type() };
                    std::filesystem::path fp { std::filesystem::absolute(p) };
                    std::string fullpath { fp.string() };
                    std::string dest = util::file::build_destination_path
                    (
                        lastdir, fullpath, basedest
                    );
#if defined PLATFORM_DEBUG_TMI
                    std::string ftname { file::get_type_name(ft) };
                    printf
                    (
                        "[%2d] src = '%s'; dest = '%s'; %s\n",
                        (count + 1), CSTR(fullpath), CSTR(dest),
                        CSTR(ftname)
                    );
#endif
                    result = bfn(fullpath, dest, ft);
                    if (! result)
                        break;

#if defined PLATFORM_DEBUG_TMI
                    ++count;
#endif
                }
            }
#if defined PLATFORM_DEBUG_TMI
            printf("process_bi_path() count = %d\n", count);
#endif
        }
        catch (const std::filesystem::filesystem_error & e)
        {
            util::error_message("process_bi_path() failed", e.what());
            result = false;
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
 * \param bfn
 *      The function that takes the source file/directory as retrieved
 *      by fts_read_entry() and the target directory and does some cross
 *      processing, such as copying. Remember that the source directory
 *      comes from the path_ptrs() accessor.
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
filewalker::process_bi_files
(
    bifunction bfn,
    const std::string & destination,
    comparator cfn
)
{
    bool result
    {
        not_nullptr(bfn) && util::file_is_directory(destination)
    };
    if (result)
    {
        for (const auto & path : search_paths())
        {
            result = process_bi_path(path, bfn, /*target,*/ destination, cfn);
            if (! result)
                break;
        }
    }
    return result;
}

namespace file
{

/*-------------------------------------------------------------------------
 * Free functions in the util::file namespace
 *-------------------------------------------------------------------------*/

/**
 *  The argument compare_whatever() specifies a user-defined function
 *  which can be used to order the traversal of the hierarchy.
 *
 *  It return ones of the following values to indicate if the file
 *  referenced by its first argument comes before, in any order with respect
 *  to, or after, the file referenced by its second argument.
 *
 *      -   A negative value. First comes before second.
 *      -   Zero. First and second aren't ordered.
 *      -   A positive value. First comes after second.
 *
 *  If the compare_whatever() argument is NULL, the directory traversal order
 *  is in the order listed in path_argv for the root paths, and in the order
 *  listed in the directory for everything else.
 *
 *      return strcmp((*first)->fts_name, (*second)->fts_name);
 */

int
compare_files_before_dirs
(
    const std::filesystem::directory_entry & afirst,
    const std::filesystem::directory_entry & asecond
)
{
    if (afirst.is_regular_file())
    {
        return (-1);                                    /* first    */
    }
    else if (asecond.is_regular_file())
    {
        return 1;                                       /* last     */
    }
    else
    {
        const std::filesystem::path & p1 { afirst.path() };
        const std::filesystem::path & p2 { asecond.path() };
        return p1.string().compare(p2.string());
    }
}

/**
 *  Some files need to be ignored. Note sure about symlinks yet.
 *
 *      bool bad
 *      {
 *          entry.is_none() ||
 *          entry.is_not_found() ||
 *          entry.is_block() ||
 *          entry.is_symlink() ||
 *          entry.is_character() ||
 *          entry.is_fifo() ||
 *          entry.is_socket() ||
 *          entry.is_unknown()
 *      };
 */

bool
is_actionable_file (const std::filesystem::directory_entry & entry)
{
    return entry.is_regular_file() || entry.is_directory();
}

/**
 *  Just a simple test callback function; see tests/filewalker_test.
 *  Must apply the --verbose flag to see the output.
 */

bool
show_target
(
    const std::string & match,
    std::filesystem::file_type ft
)
{
    std::string t { get_type_name(ft) };
    std::string m { match.empty() ? "---" : match };
    if (t.empty())
        t = "?";                        /* this should not happen           */

    util::info_message(t, m);           /* show message if --verbose        */
    return true;
}

/**
 *  This filewalker::function just shows information about the
 *  directory entry as compiled by the traverse() function.
 */

bool
show_directory_entry
(
    const std::string & description,
    std::filesystem::file_type ft
)
{
    bool result { ft != std::filesystem::file_type::none };
    if (result)
    {
        std::string ftname { get_type_name(ft) };
        std::string msg { description + "  [" + ftname + "]" };
        util::info_message(msg);    /* show message if --verbose        */
    }
    return result;
}

/**
 *  Copies a file or creates a directory.
 *
 * \param source
 *      Provides a file-specification (full path) for either a directory
 *      or a file that is the item to be created or copied in the
 *      target directory.
 *
 * \param target
 *      Provides a file-specification (full path) for either a directory
 *      or a file that is the destination for the source item.
 *
 * \param ft
 *      Provides the file-type for the source item.
 *
 * \return
 *      Returns true if the operation succeeded.
 */

bool
item_copy
(
    const std::string & source,
    const std::string & target,
    std::filesystem::file_type ft
)
{
    bool result { true };                               /* no action ok     */
    if (ft == std::filesystem::file_type::directory)    /* 1st directory    */
    {
        util::info_message("Entered directory", source);
        result = util::make_directory_path(target);     /* like 'mkdir -p'  */
    }
    else if (ft == std::filesystem::file_type::regular) /* regular file     */
    {
        util::info_printf("Copying %s to %s", V(source), V(target));

        /*
         * result = util::file_copy_to_path(source, target);
         */

        result = util::file_copy(source, target);
    }
    else if (ft == std::filesystem::file_type::symlink) /* a symbolic link  */
    {
        util::info_printf("Copying %s to %s", V(source), V(target));
        result = util::file_copy_to_path(source, target);
    }
#if 0
    else if (ft == std::filesystem::file_type::symlink) /* link w/no target */
    {
        util::info_message("Empty soft link", source);
    }
    else if (ft == std::filesystem::file_type::directory) /* last directory */
    {
        // nothing yet
    }
#endif
    return result;
}

/**
 *  An ftswalker::function callback for deleting a file, directory,
 *  and directory hierarchy.
 *
 *  Note that a directory cannot be deleted until all its children
 *  are deleted.
 */

bool
item_delete (const std::string & item, std::filesystem::file_type ft)
{
    bool result { ! util::file_exists(item) };          /* already gone?    */
    if (! result)
    {
        if (ft == std::filesystem::file_type::directory) /* 1st directory   */
        {
            util::info_message("Deleting directory", item);
            result = util::file::delete_directory(item);
        }
#if 0
        else if (ft == util::ftswalker::FTS::SLNONE)    /* link w/no target */
        {
            util::info_message("Deleting ophan link", item);
            result = util::file_delete(item);
        }
#endif
        else if (ft == std::filesystem::file_type::regular) /* regular file */
        {
            util::info_message("Deleting", item);
            result = util::file_delete(item);
        }
        else if (ft == std::filesystem::file_type::symlink) /* soft link    */
        {
            // TODO
        }
    }
    if (! result)
        util::error_message("could not delete item", item);

    return result;
}

/**
 *  Copies a directory hierarchy to another directory. See the
 *  fw_copy_test() function in the filewalker_test program.
 *
 *  The old method is not suitable, as different systems iterate through
 *  directories in indeterminate order.
 *
 *  In the new method, we collect the sources and then order them, and
 *  the recreate the destination.
 *
 * \param source
 *      Provides the relative or absolute path to the source directory.
 *      It must exist.
 *
 * \param dest
 *      Provides the relative or absolute path to the destination directory.
 *      It does not need to exist; it will be made.
 *
 * \return
 *      Returns true if the copying succeeded in full.
 */

bool
copy_directory_tree
(
    const std::string & source,
    const std::string & destination
)
{
#if defined USE_PROCESS_BI_FILES

    /*
     * This does not work on some systems due to the order of recursive
     * iteration.
     */

    bool result { file_is_directory(source) && file_is_directory(destination) };
    if (result)
    {
        util::filewalker walker(source);    /* lock in source directory     */
        result = walker.process_bi_files
        (
            item_copy, destination          /* compare_files_before_dirs    */
        );
    }

#else

    bool result { file_is_directory(source) };
    if (result)
    {
        std::string lastdir { util::file::get_last_directory(source) };
        std::string basedest { destination };
        if (! lastdir.empty())
            basedest = util::filename_concatenate(destination, lastdir);

        filewalker::pairs collection;
        result = collect_files_from_path(source, collection);
        if (result)
        {
#if defined PLATFORM_DEBUG_TMI
            std::string collstring { collection_to_string(collection) };
            std::cout << collstring;
#endif
            /*
             * Iterate through the collection to get the directories first.
             */

            for (const auto & entry : collection)
            {
                const filewalker::pair & p { entry.second };
                if (p.pfs_type == std::filesystem::file_type::directory)
                {
                    std::string dest = util::file::build_destination_path
                    (
                        lastdir, p.pfs_name, basedest
                    );
                    result = item_copy(p.pfs_name, dest, std::filesystem::file_type::directory);
                }
            }

            /*
             * Iterate through the collection to get the regular files.
             */

            for (const auto & entry : collection)
            {
                const filewalker::pair & p { entry.second };
                if (p.pfs_type == std::filesystem::file_type::regular)
                {
                    std::string dest = util::file::build_destination_path
                    (
                        lastdir, p.pfs_name, basedest
                    );
                    result = item_copy(p.pfs_name, dest, std::filesystem::file_type::regular);
                }
            }
        }
    }

#endif  // defined USE_PROCESS_BI_FILES

    return result;
}

/**
 *  This function removes the files and directories it encounters, including
 *  sub-directories, all in one call.
 *
 *  In general, directories are visited two distinguishable times; in preorder
 *  (before any of their descendants are visited) and in postorder (after all
 *  of their descendants have been visited).  Files are visited once.  It is
 *  possible to walk the hierarchy "logically" (visiting the files that
 *  symbolic links point to) or physically (visiting the symbolic links
 *  themselves), order the walk of the hierarchy or prune and/or revisit
 *  portions of the hierarchy.
 *
 *  Note that this function is not a filewalker::function callback.
 *
 *  We replace the original implementation with a simple call to remove_all().
 *  Compare this function to delete_directory_tree().
 *
 *  #if defined USE_FILEWALKER_METHOD
 *      util::filewalker walker(path);
 *      result = walker.process_files
 *      (
 *          item_delete, path // , std::filesystem::file_type::directory
 *      );
 *  #else ....
 *
 * \param path
 *      The directory to delete. It is checked that it is a directory.
 *
 * \return
 *      Returns true if the directory was deleted. If the path was not a
 *      directory, false is returned.
 */

bool
delete_directory (const std::string & path)
{
    bool result { ! util::file_exists(path) };      /* already gone?        */
    if (! result)                                   /* not yet              */
        result =  util::file_is_directory(path);

    if (result)
    {
        const std::uintmax_t s_errcount { static_cast<std::uintmax_t>(-1) };
        std::error_code ec;                         /* use noexcept call    */
        std::uintmax_t count { std::filesystem::remove_all(path, ec) };
        result = count != s_errcount;
        if (! result)
            util::error_message("could not delete directory", path);
    }
    return result;
}

/**
 *  First, we collect the directories and files in the path, and
 *  then we systematically, carefully, delete them.
 */

bool
delete_directory_tree (const std::string & path)
{
    bool result { ! path.empty() };
    if (result)
    {
        filewalker::pairs collection;
        result = collect_files_from_path(path, collection);
        if (result)
        {
#if defined PLATFORM_DEBUG_TMI
            std::string collstring { collection_to_string(collection) };
            std::cout << collstring;
#endif
            result = delete_collection(collection);
        }
    }
    return result;
}

/**
 *  Collects files and directories in the path. Do we need this?
 *  See delete_directory_tree().
 *
 *  The issue is that the directory iterator cannot deal with
 *  changes in the file tree while iterating, and deletion does
 *  that. Therefore, we collect the tree (which reduces the period
 *  of vulnerability as well) to deal with "later".
 *
 *  We should also collect the provided path itself, which has no
 *  depth, and should be the last directory deleted.
 *
 * \param path
 *      Provides the full or relative path to the directory to be
 *      assessed. We currently do not verify that it is a directory.
 *
 * \param [out] collection
 *      Provides a container to hold information on the files and
 *      directories in the path.
 *
 * \param includeroot
 *      If true (the default), then path itself is included in the
 *      collection.
 *
 * \return
 *      Returns true if at least item got collected.
 */


bool
collect_files_from_path
(
    const std::string & path,
    filewalker::pairs & collection,
    bool includeroot
)
{
    bool result { ! path.empty() };
    if (result)
    {
        try
        {
            for                         /* get file attributes a la fts_ent */
            (
                auto it = std::filesystem::recursive_directory_iterator(path);
                it != std::filesystem::end(it); ++it
            )
            {
                const auto & entry = *it;
                if (is_actionable_file(entry))
                {
                    std::filesystem::path p { entry.path() };
                    const std::filesystem::file_status & fs { entry.status() };
                    std::filesystem::file_type ft { fs.type() };
                    std::filesystem::path fp { std::filesystem::absolute(p) };
                    std::string fullpath { fp.string() };
                    int fdepth { it.depth() };
                    filewalker::pair fpair { ft, fullpath };
                    auto insertpair { std::make_pair(fdepth, fpair) };
                    (void) collection.insert(insertpair);
                }
            }
            if (includeroot)
            {
                int fdepth { -1 };
                filewalker::pair fpair
                {
                    std::filesystem::file_type::directory, path
                };
                auto insertpair { std::make_pair(fdepth, fpair) };
                (void) collection.insert(insertpair);
            }
            result = collection.size() > 0;
        }
        catch (const std::filesystem::filesystem_error & e)
        {
            util::error_message("collect_files_from_path() failed", e.what());
            result = false;
        }
    }
    return result;
}

std::string
collection_to_string (const filewalker::pairs & collection)
{
    std::string result { "Collection:\n" };
    for (const auto & entry : collection)
    {
        std::string depth { std::to_string(entry.first) };
        const filewalker::pair & p { entry.second };
        std::string tname { get_type_name(p.pfs_type) };
        result += "   [";
        result += depth;
        result += "] '";
        result += p.pfs_name;
        result += "': ";
        result += tname;
        result += "\n";
    }
    return result;
}

/**
 *  Two ways to go:
 *
 *      1.  Do remove_all() on all directories in the collection, then
 *          iterate through the collection deleting any regular files that
 *          still exist. But what of sub-directories that have disappeared?
 *          So....
 *
 *      2.  Make a pass to delete all the regular files regardless of
 *          depth, then remove each directory in reverse order of depth.
 *
 * \param collection
 *      Provides a tree of file items as built by
 *      collection_files_from_path().
 *
 * \return
 *      Returns true if successful.
 */

bool
delete_collection (const filewalker::pairs & collection)
{
    bool result { collection.size() > 0 };
    if (result)
    {
        /*
         * Get regular files only, no worries about depth here.
         * Will handle symlinks later.
         */

        for (const auto & entry : collection)
        {
            const filewalker::pair & p { entry.second };
            if (p.pfs_type == std::filesystem::file_type::regular)
            {
                result = item_delete(p.pfs_name, p.pfs_type);
                if (! result)
                    break;
            }
        }

        /*
         * Get the directories starting at the lowest depth, and
         * delete them.
         */

        for
        (
            auto rit = collection.crbegin(); rit != collection.crend(); ++rit
        )
        {
            const filewalker::pair & p { rit->second };
            if (p.pfs_type == std::filesystem::file_type::directory)
            {
                result = delete_directory(p.pfs_name);
                if (! result)
                    break;
            }
        }
    }
    return result;
}

/**
 *  Determines if a file is found in the provided directory. This
 *  function assumed the target is a file-name, *not* a regular
 *  expression.
 *
 * \param path
 *      Provides the path in which to conduct the search.
 *
 * \param target
 *      Provides the base-name of the file to find. This is *not*
 *      treated as a regular expression.
 *
 * \return
 *      Returns true if the exact base file-name was found.
 */

bool
find_file
(
    const std::string & path,
    const std::string & target
)
{
    bool result { ! path.empty() && ! target.empty() };
    if (result)
    {
        util::filewalker fw(path);              /* just one search-path     */
        lib66::tokenization collected;          /* wasteful, but easy       */
        bool not_a_regex { true };
        result = fw.find_files(target, collected, not_a_regex);
    }
    return result;
}

/**
 *  Inspired by a similarly-named function in the Ardour source code.
 *  However, it does not directly use a searchpath; instead, created the
 *  desired search and use it as noted below.
 *
 *  It is a more hardwired version of filewalker::process_files(). It
 *  does not call an filewalker::function. Instead, it just copies
 *  path to the 'collected' parameter.
 *
 *  We'll depend on regex to filter regular files
 *  versus directories. Compare to item_copy().
 *
 *          FTS ft = get_fts_type(ent);
 *          result = fn(p, ft);
 *          if (! result) break;
 *
 *  Another option is bind an filewalker::function to a non-auto
 *  lib66::tokenization object into which the findings are pushed.
 *
 * \param [out] collected
 *      Holds the files that were found. Use it if true is returned.
 *
 * \param paths
 *      Holds all of the paths to be searched. The searchpath class
 *      can be used to get all of the paths in a "PATH"-type variable,
 *      and then the searchpath::paths() function can be used for
 *      this parameter.
 *
 * \param rgx
 *      Holds a regular expression, which must be valid, to define the
 *      objects of the search. If a glob is needed, first convert it to
 *      a regex via the util::glob_to_regex() function in the strfunctions
 *      module.
 *
 * \return
 *      Returns true if no errors occurred and at least one
 *      file was found.
 */

bool
find_files_by_regex
(
    lib66::tokenization & collected,
    const lib66::tokenization & paths,
    const std::string & rgx,
    filewalker::comparator cfn
)
{
    (void) cfn;


    bool result { paths.size() > 0 && ! rgx.empty() };
    if (result)
    {
        util::filewalker fw(paths);     /* adds paths to search_paths()     */
        result = fw.find_files(rgx, collected);
    }
    return result;
}

/**
 *  Gets a brief description of an <filesystem> file type.
 */

std::string
get_type_name (std::filesystem::file_type ft)
{
    std::string result;
    switch (ft)
    {
        case std::filesystem::file_type::none:

            result = "undetermined/error";
            break;

        case std::filesystem::file_type::not_found:

            result = "file not found";
            break;

        case std::filesystem::file_type::regular:

            result = "regular file";
            break;

        case std::filesystem::file_type::directory:

            result = "directory";
            break;

        case std::filesystem::file_type::symlink:

            result = "symbolic link";
            break;

        case std::filesystem::file_type::block:

            result = "block device";
            break;

        case std::filesystem::file_type::character:

            result = "character device";
            break;

        case std::filesystem::file_type::fifo:

            result = "FIFO/pipe";
            break;

        case std::filesystem::file_type::socket:

            result = "socket";
            break;

        case std::filesystem::file_type::unknown:

            result = "unknown file type";
            break;

#if 0
        case util::filewalker::FTS::SLNONE:
            result = "Untargeted symbolic link";
        case util::filewalker::FTS::NSOK:
            result = "No stat request";
        case util::filewalker::FTS::DC:
            result = "Directory cycle";
        case util::filewalker::FTS::DOT:
            result = "Dot file";
        case util::filewalker::FTS::DP:
            result = "Postorder directory";
        case util::filewalker::FTS::DNR:
            result = "Directory read error";
        case util::filewalker::FTS::ERR:
            result = "Error";
        case util::filewalker::FTS::NS:
            result = "No-stat error";
#endif

        default:

            result = "Unknown";
            break;
    }
    return result;
}

/**
 *  Peels off the last directory name in a path specification.
 *  This sub-directory is meant to be the root directory of a source
 *  for files and sub-directories.
 *
 *  Which method to use? The first builds a vector of path names,
 *  which the second just uses an iterator to get to the last sub-directory
 *  name. We use the second method.
 *
 * \param pathspec
 *      Provides a directory name, such as "tests/data/fts". This function
 *      assumes it is actually a directory. Should we check that?
 *
 * \return
 *      Returns the last sub-directory, e.g. "fts" in the example above.
 */

std::string
get_last_directory (const std::string & pathspec)
{
    std::filesystem::path p { pathspec };

#if defined USE_LIB55_TOKENIZATION_VECTOR

    lib66::tokenization subdirs;
    for (const auto & s : p)
        subdirs.push_back(s);

    return subdirs.empty() ? pathspec : subdirs.back();

#else

    std::filesystem::path::const_iterator sit;
    for (auto it = p.begin(); it != p.end(); ++it)
        sit = it;

    if (sit != p.end())
        return *sit;
    else
        return pathspec;

#endif
}

/**
 *  Given a complete path and a root subdirectory name, plus a root
 *  destination directory, this function assembles the corresponding
 *  destination directory.
 *
 * Example:
 *
 *  Assume the Source directory starts as "tests/data/fts" and
 *  the Destination directory starts as "build/tests/data".
 *  The Root Source subdirectory is found by get_last_directory() to
 *  be "fts". The Root Destination directory becomes "build/tests/data/fts".
 *
 *  Now assume we've recursed to the file
 *
 *      tests/data/fts/session_3/set_1/c_fake_file.midi
 *
 *  We iterate through this file-specification using an
 *  std::filesystem::path() until we find the Root Source "fts".
 *  Then we re-concatenate the remaining sub-directories, and
 *  append the to the Destination directory.
 *
 * \param rootsource
 *      Provides the root sub-directory, such as "fts".
 *
 * \param srcitem
 *      Provides the source directory or file, such as
 *      "tests/data/fts/session_3/set_1/c_fake_file.midi".
 *
 * \param rootdest
 *      Provides the destination root directory, such as
 *      "build/tests/data/fts".
 *
 * \return
 *      Returns the updated destination file or directory.
 *      In the examples above, the result would be
 *      "build/tests/data/fts/session_3/set_1/c_fake_file.midi".
 */

std::string
build_destination_path
(
    const std::string & rootsource,
    const std::string & srcitem,
    const std::string & rootdest
)
{
    std::string result { rootdest };
    bool good
    {
        ! rootsource.empty() && ! srcitem.empty() && ! rootdest.empty()
    };
    if (good)
    {
        bool collect { false };
        std::filesystem::path p { srcitem };
        lib66::tokenization collection;
        for (auto it = p.begin(); it != p.end(); ++it)
        {
            if (*it == rootsource)
            {
                collect = true;
            }
            else
            {
                if (collect)
                    collection.push_back(*it);
            }
        }
        if (collection.size() > 0)
        {
            for (const auto & s : collection)
                result = util::filename_concatenate(result, s);
        }
    }
    return result;
}

}           // namespace file

}           // namespace util

/*
 * filewalker.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
