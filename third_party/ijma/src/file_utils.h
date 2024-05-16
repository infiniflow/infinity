/** \file file_utils.h
 * Definition of utility functions for file operations.
 *
 * \author Jun Jiang
 * \version 0.1
 * \date Sep 03, 2010
 */

#ifndef JMA_FILE_UTILS_H
#define JMA_FILE_UTILS_H

#include <cassert>
#include <cstdlib> // mkstemp, atoi
#include <cstring> // strchr
#include <fstream>
#include <iostream>
#include <string>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <windows.h> // GetTempPath, GetTempFileName, FindFirstFile, FindClose
#else
#include <dirent.h> // opendir, closedir
#include <unistd.h> // unlink
#endif

namespace jma {

/** the delimiter used to check whether is separator between directories and file names */
#define PATH_DELIMIT_CHECK "/\\"

/** the delimiter used to separate directories and file names */
#if defined(_WIN32) && !defined(__CYGWIN__)
#define PATH_DELIMIT_CREATE '\\'
#else
#define PATH_DELIMIT_CREATE '/'
#endif

/**
 * Create a unique temporary file and output its file name.
 * It is commented out as the temporary file is replaced with memory.
 * \param tempName the string to save the temporary file name
 * \return true for success and \e tempName is set as the temporary file name. false for fail and \e tempName is not modified.
 */
inline bool createTempFile(std::string &tempName) {
#if defined(_WIN32) && !defined(__CYGWIN__)
    // directory name for temporary file
    char dirName[MAX_PATH];

    // retrieve the directory path designated for temporary files
    DWORD dirResult = GetTempPath(MAX_PATH, dirName);
    if (dirResult == 0) {
        std::cerr << "fail in GetTempPath() to get the directory path for temporary file" << std::endl;
        return false;
    }

    // file name for temporary file
    char fileName[MAX_PATH];

    // create a unique temporary file
    UINT nameResult = GetTempFileName(dirName, "jma", 0, fileName);
    if (nameResult == 0) {
        std::cerr << "fail in GetTempFileName() to create a temporary file" << std::endl;
        return false;
    }

    tempName = fileName;
#else
    // file name for temporary file
    char fileName[] = "/tmp/jmaXXXXXX";

    // create a unique temporary file
    int tempFd = mkstemp(fileName);
    if (tempFd == -1) {
        std::cerr << "fail in mkstemp() to create a temporary file" << std::endl;
        return false;
    }

    tempName = fileName;
#endif

    return true;
}

/**
 * Delete the file on disk.
 * \param fileName the name of the file to be deleted
 * \return true for success and false for fail.
 */
inline bool removeFile(const std::string &fileName) {
    if (unlink(fileName.c_str()) == 0)
        return true;

    return false;
}

/**
 * Check whether the directory exists.
 * \param dirPath the directory path to be checked
 * \return true for the direcotory exists, false for not exists.
 */
inline bool isDirExist(const char *dirPath) {
    if (dirPath == 0)
        return false;

    bool result = false;

#if defined(_WIN32) && !defined(__CYGWIN__)
    WIN32_FIND_DATA wfd;
    HANDLE hFind;

    // the parameter string in function "FindFirstFile()" would be invalid if it ends with a trailing backslash (\) or slash (/),
    // so the trailing backslash or slash is removed if it exists
    std::string dirStr(dirPath);
    size_t len = dirStr.size();
    if (len > 0 && strchr(PATH_DELIMIT_CHECK, dirPath[len - 1])) {
        dirStr.erase(len - 1, 1);
    }

    hFind = FindFirstFile(dirStr.c_str(), &wfd);
    if (hFind != INVALID_HANDLE_VALUE) {
        result = true;
    }
    FindClose(hFind);
#else
    DIR *dir = opendir(dirPath);
    if (dir)
        result = true;

    closedir(dir);
#endif

    return result;
}

/**
 * Copy a file from source path to destination path.
 * \param src the source path
 * \param dest the destination path
 * \return true for copy successfully, false for copy failed.
 */
inline bool copyFile(const char *src, const char *dest) {
    assert(src && dest);

    // open files
    std::ifstream from(src, std::ios::binary);
    if (!from) {
        std::cerr << "cannot open source file: " << src << std::endl;
        return false;
    }

    std::ofstream to(dest, std::ios::binary);
    if (!to) {
        std::cerr << "cannot open destinatioin file: " << dest << std::endl;
        return false;
    }

    // copy characters
    char ch;
    while (from.get(ch))
        to.put(ch);

    // check file state
    if (!from.eof() || !to) {
        std::cerr << "invalid file state after copy from " << src << " to " << dest << std::endl;
        return false;
    }

    return true;
}

/**
 * Create the complete file path from directory path and file name.
 * \param dir the directory path
 * \param file the file name
 * \return the file path string consisting of directory path and file name.
 * \pre the file name \e file is assumed as non empty.
 */
inline std::string createFilePath(const char *dir, const char *file) {
    assert(file && "the file name is assumed as non empty");

    std::string result = dir;

    if (result.size() && strchr(PATH_DELIMIT_CHECK, result[result.size() - 1]) == NULL) {
        result += PATH_DELIMIT_CREATE;
    }

    result += file;
    return result;
}

/**
 * Extract file name from path string.
 * \param path the path such as "/usr/bin/man", "c:\\windows\\winhelp.exe", or "man"
 * \return the file name such as "man", "winhelp.exe"
 */
inline std::string getFileName(const std::string &path) {
    size_t last = path.find_last_of(PATH_DELIMIT_CHECK);

    if (last == std::string::npos)
        return path;

    return path.substr(last + 1);
}

/**
 * Extract directory path from file path.
 * \param filePath the file path such as "/usr/bin/man", "c:\\windows\\winhelp.exe", or "man"
 * \return the directory path such as "/usr/bin", "c:\\windows", "."
 */
inline std::string getDirPath(const std::string &filePath) {
    if (filePath.empty())
        return "";

    size_t last = filePath.find_last_of(PATH_DELIMIT_CHECK);
    if (last == std::string::npos)
        return ".";

    last = filePath.find_last_not_of(PATH_DELIMIT_CHECK, last);
    if (last == std::string::npos)
        return ".";

    return filePath.substr(0, last + 1);
}

/**
 * Normalize directory path.
 * \param dirPath the directory path such as "/usr/bin/", "/usr/bin", "c:\\windows\\", "c:\\windows", "./", "."
 * \return the normalized directory path such as "/usr/bin", "c:\\windows", "."
 */
inline std::string normalizeDirPath(const std::string &dirPath) {
    if (dirPath.size() && strchr(PATH_DELIMIT_CHECK, dirPath[dirPath.size() - 1])) {
        size_t last = dirPath.find_last_not_of(PATH_DELIMIT_CHECK);
        if (last != std::string::npos)
            return dirPath.substr(0, last + 1);
        else
            return ""; // in case of invalid input of "/////";
    }

    return dirPath;
}

} // namespace jma

#endif // JMA_FILE_UTILS_H
