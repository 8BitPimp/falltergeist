/*
 * Copyright 2012-2015 Falltergeist Developers.
 *
 * This file is part of Falltergeist.
 *
 * Falltergeist is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Falltergeist is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Falltergeist.  If not, see <http://www.gnu.org/licenses/>.
 */

// C++ standard includes

#include <algorithm>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <SDL.h>

#if defined(__unix__) || defined(__APPLE__)
    #include <sys/param.h>
    #include <dirent.h>
#endif

#if defined(__MACH__)
    #include <mach/mach_time.h>
#else
    #include <time.h>
#endif

#if defined(__linux__)
    #include <mntent.h>
#endif

#if defined (__APPLE__) || defined(BSD)
    #include <sys/ucred.h>
    #include <sys/mount.h>
#endif

#if defined(__unix__) || defined(__APPLE__)
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#endif

// Falltergeist includes
#include "Platform.h"
#include "../Exception.h"
#include "../Logger.h"

// Third party includes

namespace Falltergeist
{

// static members initialization
std::string CrossPlatform::_version;
std::string CrossPlatform::_falloutDataPath;
std::string CrossPlatform::_falltergeistDataPath;
std::vector<std::string> *CrossPlatform::_dataFiles = 0;
const std::vector<std::string> CrossPlatform::necessaryDatFiles = {"master.dat", "critter.dat"};


CrossPlatform::CrossPlatform()
{
}

CrossPlatform::~CrossPlatform()
{
}

std::string CrossPlatform::getVersion()
{
    if (_version.length() > 0) return _version;

    _version = "Falltergeist 0.2.0";
#if defined(__linux__)
    _version += " (Linux)";
#elif defined(__APPLE__)
    _version += " (Apple)";
#elif defined(BSD)
    _version += " (BSD)";
#endif
    return _version;
}

std::string CrossPlatform::getHomeDirectory()
{
    char *cwd = getenv("HOME");
    return std::string(cwd);
}

std::string CrossPlatform::getExecutableDirectory()
{
    char* buffer = SDL_GetBasePath();
    std::string path(buffer);
    SDL_free(buffer);
    return path;
}

std::vector<std::string> CrossPlatform::getCdDrivePaths()
{
    std::vector<std::string> result;
#if defined(__linux__)
    FILE *mtab = setmntent("/etc/mtab", "r");
    struct mntent *m;
    struct mntent mnt;
    char strings[4096];
    while ((m = getmntent_r(mtab, &mnt, strings, sizeof(strings))))
    {
        std::string directory = mnt.mnt_dir;
        std::string type = mnt.mnt_type;
        if (type == "iso9660")
        {
            result.push_back(directory);
        }
    }
    endmntent(mtab);
#elif defined (BSD)
    struct statfs *mntbuf;
    int mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
    for ( int i = 0; i < mntsize; i++ )
    {
        std::string directory = ((struct statfs *)&mntbuf[i])->f_mntonname;
        std::string type = ((struct statfs *)&mntbuf[i])->f_fstypename;
        if (type == "cd9660")
        {
            result.push_back(directory);
        }
    }
#else
    throw Exception("CD-ROM detection not supported");
#endif
    return result;
}

// This method is trying to find out where are the DAT files located
std::string CrossPlatform::findFalloutDataPath()
{
    if (_falloutDataPath.length() > 0)
        return _falloutDataPath;
    Logger::info() << "Looking for Fallout data files" << std::endl;
    std::vector<std::string> directories;
    directories.push_back(getExecutableDirectory());

    for (auto &directory : getDataPaths())
    {
        directories.push_back(directory);
    }

    try
    {
        std::vector<std::string> cdDrives = getCdDrivePaths();
        directories.insert(directories.end(), cdDrives.begin(), cdDrives.end());
    }
    catch (Exception e)
    {
        Logger::error() << e.what() << std::endl;
    }

    for (auto &directory : directories)
    {
        if (std::all_of(
                necessaryDatFiles.begin(),
                necessaryDatFiles.end(),
                [directory](const std::string& file) {
                    std::ifstream stream(directory + "/" + file);
                    if (stream)
                    {
                        Logger::info() << "Searching in directory: " << directory << " " << file << " [FOUND]" << std::endl;
                        return true;
                    }
                    else
                    {
                        Logger::info() << "Searching in directory: " << directory << " " << file << " [NOT FOUND]" << std::endl;
                        return false;
                    }
                })
                )
        {
            _falloutDataPath = directory;
            return _falloutDataPath;
        }
    }

    throw Exception("Fallout data files are not found!");
}

std::string CrossPlatform::findFalltergeistDataPath()
{
    if (_falltergeistDataPath.length() > 0)
        return _falltergeistDataPath;
    Logger::info() << "Looking for Falltergeist data files" << std::endl;
    std::vector<std::string> directories;
    directories.push_back(getExecutableDirectory());

    for (auto &directory : getDataPaths())
    {
        directories.push_back(directory);
    }

    for (auto &directory : directories)
    {
        std::ifstream stream(directory + "/data/movies.lst");
        if (stream)
        {
            Logger::info() << "Searching in directory: " << directory << "data/movies.lst [FOUND]" << std::endl;
            _falltergeistDataPath = directory;
            return _falltergeistDataPath;
        }
        else
        {
            Logger::info() << "Searching in directory: " << directory << "data/movies.lst [NOT FOUND]" << std::endl;
        }
    }

    throw Exception("Falltergeist data files are not found!");
}

// this method looks for available dat files
std::vector<std::string> *CrossPlatform::findFalloutDataFiles()
{
    if (_dataFiles)
        return _dataFiles;

    // looking for all available dat files in directory
    DIR *pxDir = opendir(CrossPlatform::findFalloutDataPath().c_str());
    if (!pxDir)
    {
        throw Exception("Can't open data directory: " + CrossPlatform::findFalloutDataPath());
    }
    _dataFiles = new std::vector<std::string>(necessaryDatFiles);
    struct dirent *pxItem = 0;
    while ((pxItem = readdir(pxDir)))
    {
        std::string filename(pxItem->d_name);
        if (filename.length() > 4) // exclude . and ..
        {
            std::string ext = filename.substr(filename.size() - 4, 4);
            if (ext == ".dat")
            {
                if (filename.length() == 12 && filename.substr(0, 5) == "patch")
                    _dataFiles->insert(_dataFiles->begin(),filename);
            }
        }
    }
    closedir(pxDir);

    return _dataFiles;
}

bool CrossPlatform::_createDirectory(const char *dir)
{
#if defined(__unix__) || defined(__APPLE__) // Linux, OS X, BSD
    struct stat st;

    if (stat(dir, &st) == 0)
    {
        // Directory already exists
        if (S_ISDIR(st.st_mode))
            return false;

        throw std::runtime_error("Path `" + std::string(dir) + "' already exists and is not a directory");
    }
    else
    {
        if (mkdir(dir, S_IRWXU) == 0)
            return true;

        throw std::runtime_error(strerror(errno));
    }
#else
    #error Platform not supported: CrossPlatform::_createDirectory not implemented
#endif
    return false;
}

void CrossPlatform::createDirectory(std::string path)
{
    // Start index of 1 means that we skip root directory.
    for (auto dirDelim = path.find_first_of('/', 1U); dirDelim != std::string::npos;
         dirDelim = path.find_first_of('/', dirDelim + 1)) {
        path[dirDelim] = '\0';
        _createDirectory(path.c_str());
        path[dirDelim] = '/';
    }
    _createDirectory(path.c_str());
}

std::string CrossPlatform::getConfigPath()
{
#if defined(__unix__)
    char *maybeConfigHome = getenv("XDG_CONFIG_HOME");
    if (maybeConfigHome == nullptr || *maybeConfigHome == '\0')
        return getHomeDirectory() + "/.config/falltergeist";
    return std::string(maybeConfigHome) + "/falltergeist";
#elif defined(__APPLE__)
    return getHomeDirectory() + "/Library/Application Support/falltergeist";
#else
    #error Platform not supported: CrossPlatform::getConfigPath not implemented
#endif
}

std::vector<std::string> CrossPlatform::getDataPaths()
{
    std::vector<std::string> _dataPaths;
#if defined(__unix__)
    char *maybeDataHome = getenv("XDG_DATA_HOME");
    if (maybeDataHome == nullptr || *maybeDataHome == '\0')
        _dataPaths.push_back(getHomeDirectory() + "/.local/share/falltergeist");
    else
        _dataPaths.push_back(std::string(maybeDataHome) + "/falltergiest");

    std::string sharedir = getExecutableDirectory()+"/../share/falltergeist";

    DIR *pxDir = opendir(sharedir.c_str());
    if(pxDir)
    {
        _dataPaths.push_back(sharedir.c_str());
    }
    closedir(pxDir);
#else
    _dataPaths.push_back(getConfigPath());
#endif
    return _dataPaths;
}

void CrossPlatform::getTime(TimeInfo * out) {
    if (!out)
        throw Exception("nullptr passed into getTime");
#if defined(__MACH__)
    static mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    uint64_t time;
    time = mach_absolute_time();
    double nseconds = ((double)time * (double)timebase.numer) / ((double)timebase.denom);
    double seconds = ((double)time * (double)timebase.numer) / ((double)timebase.denom * 1e9);
    out->_sec = seconds;
    out->_nano = nseconds;
#else
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
    {
        out->_sec = ts.tv_sec;
        out->_nano = ts.tv_nsec;
    }
    else
    {
        throw Exception("call to clock_gettime failed");
    }
#endif
}
}