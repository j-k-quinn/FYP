#ifndef SL_IOT_UTILS_HPP
#define SL_IOT_UTILS_HPP

#include <ctime>
#include <iomanip>
#include <sstream> // stringstream
#include <string>
#include <chrono>
#include <vector>
//#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <random>
#include "randutils.hpp"
#include <iostream>
#include <sstream>
#include <experimental/filesystem>

#include <sys/stat.h>
#include <sys/types.h>
#ifdef _WIN32
#else
#include <sys/stat.h>
#endif
#include <sl_hub/utils/tinydir.hpp>
//#include <curl/curl.h>
#include <sl_hub/utils/json.hpp>

//#define _VERBOSE_

struct put_data {
    char *data;
    long progress;
    size_t len;
};

struct merge_data {
    char* merger;
    size_t rest;
};

struct FileDescription
{
    bool validFile;
    sl_hub::json metadata;
};

class FileMeta {
public:
    std::string filename; //, creation_date;
    unsigned long long fileSize_byte;

    FileMeta(std::string filename_, unsigned long long fileSize_byte_/*, std::string creation_date_*/) {
        filename = filename_;
        fileSize_byte = fileSize_byte_;
        //creation_date = creation_date_;
    }
};

inline bool checkFileExist(std::string filePath) {
    std::ifstream f(filePath.c_str());
    return f.good();
}

inline bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

inline int is_regular_file_stat(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) != 0)
        return 0;
    return S_ISREG(path_stat.st_mode);
}

inline int is_dir_stat(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

inline bool appendLineToFile(const std::string line, const std::string& filepath){
    std::ofstream log(filepath, std::ios_base::app | std::ios_base::out);
    log << (line + "\n");
    return true;
}

//https://stackoverflow.com/a/24586587/7036639

inline std::string random_string(std::string::size_type length) {
    static auto& chrs = "0123456789"
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    thread_local static std::mt19937 rg{randutils::auto_seed_128{}.base()};
    thread_local static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof (chrs) - 2);

    std::string s;
    s.reserve(length);

    while (length--)
        s += chrs[pick(rg)];

    return s;
}

inline std::string url_encode(const std::string &value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (std::string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
	std::string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << std::nouppercase;
    }

    return escaped.str();
}

inline int listDirectory(std::string directoryPath, std::vector<std::string> &files) {
    tinydir_dir dir;
    int res = tinydir_open(&dir, directoryPath.c_str());

    while (dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&dir, &file);

        //printf("%s", file.name);
        if (file.is_dir) {
            //printf("/");
        } else files.emplace_back(file.name);
        //printf("\n");

        tinydir_next(&dir);
    }
    tinydir_close(&dir);
    return res;
}

inline long getFileSize(std::string filename) {
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : 0;
}

inline size_t dirSize(const std::string dir) {
  if (!is_dir_stat(dir.c_str()))
      return 0;
  size_t folder_size = 0;
  for (auto &p : std::experimental::filesystem::recursive_directory_iterator(
           dir, std::experimental::filesystem::directory_options::skip_permission_denied)) {
    std::string absolute_path = std::experimental::filesystem::absolute(p.path());
    if (is_dir_stat(absolute_path.c_str())) 
    {
      // skip : the folder are already taken recusrsively
      // folder_size += dirSize(absolute_path);
    } 
    else if (is_regular_file_stat(absolute_path.c_str())) 
    {
      try {
          folder_size += std::experimental::filesystem::file_size(p);
      } catch (std::exception& e) {

      }
    }
  }

  return folder_size;
}


inline int listDirectory(std::string directoryPath, std::vector<FileMeta> &files) {

    tinydir_dir dir;
    int res = tinydir_open(&dir, directoryPath.c_str());

    while (dir.has_next) {
        tinydir_file file;
        tinydir_readfile(&dir, &file);
        if (!file.is_dir) {
            if (hasEnding(std::string(file.name),std::string("svo"))
            || hasEnding(std::string(file.name),std::string("mp4"))
            || hasEnding(std::string(file.name),std::string("jpg"))
            || hasEnding(std::string(file.name),std::string("json"))
            || hasEnding(std::string(file.name),std::string("vtt"))){
                files.emplace_back(file.name, getFileSize(directoryPath + "/" + file.name)/*, date_*/);
                #ifdef _VERBOSE_
                std::cout<<"Add file.name "<<std::string(file.name)<<std::endl;
                #endif
            }

        }
        tinydir_next(&dir);
    }
    tinydir_close(&dir);
    return res;
}

inline bool renameFile(std::string filePath_old, std::string filePath_new) {
    rename(filePath_old.c_str(), filePath_new.c_str());
    return false;
}

inline int removeFile(std::string filePath) {
    int err  = std::remove(filePath.c_str());
    return err;
}

inline bool createDirectory(std::string directoryPath) {
#ifdef _WIN32
    CreateDirectory(directoryPath.c_str(), NULL);
#else
    try {
      std::experimental::filesystem::create_directories(directoryPath);
      std::experimental::filesystem::permissions(directoryPath,
                                                 std::experimental::filesystem::perms::owner_all
                                                 | std::experimental::filesystem::perms::group_all
                                                 | std::experimental::filesystem::perms::others_read
                                                 | std::experimental::filesystem::perms::others_exec);

    } catch (const std::experimental::filesystem::filesystem_error& e)
    {
      return false;
    }
#endif
    return true;
}

inline bool createDirectoryIfNotExists(std::string directoryPath) {
    struct stat info;
    if (stat(directoryPath.c_str(), &info) != 0) {
        createDirectory(directoryPath);
    }
    else if (info.st_mode & S_IFDIR) { // S_ISDIR() doesn't exist on my windows

    } else {
        removeFile(directoryPath);
        createDirectory(directoryPath);
    }
    return true;
}

inline std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

inline std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

inline std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ") {
    return ltrim(rtrim(str, chars), chars);
}

inline std::string getFileNameFromPath(std::string path) {
    const size_t last_slash_idx = path.find_last_of("\\/");
    if (std::string::npos != last_slash_idx) path.erase(0, last_slash_idx + 1);
    return path;
}

inline std::vector<std::string> split(std::string& str, const std::string& delim) {
    str.erase(std::remove(str.begin(), str.end(), '"'), str.end());

    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;
    do {
        pos = str.find(delim, prev);
        if (pos == std::string::npos) pos = str.length();
        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) tokens.push_back(token);
        prev = pos + delim.length();
    } while (pos < str.length() && prev < str.length());
    return tokens;
}

inline void removeChar(std::string &str, char delim) {
    str.erase(std::remove(str.begin(), str.end(), delim), str.end());
}

/*
 * Erase First Occurrence of given  substring from main string.
 */
inline void eraseSubStr(std::string & mainStr, std::string toErase) {
    // Search for the substring in string
    size_t pos = mainStr.find(toErase);
    if (pos != std::string::npos) {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}

inline void eraseAllSubStr(std::string & mainStr, const std::string & toErase)
{
    size_t pos = std::string::npos;

    // Search for the substring in string in a loop untill nothing is found
    while ((pos  = mainStr.find(toErase) )!= std::string::npos)
    {
        // If found then erase it from string
        mainStr.erase(pos, toErase.length());
    }
}

inline std::string get_parent_path(const std::string &path)
{
    auto parent_path = std::experimental::filesystem::path(path).parent_path().string();

    if (parent_path.back() != '/') parent_path += "/";

    return parent_path;
}

inline std::string get_base_name(const std::string &path)
{
    auto base_name = std::experimental::filesystem::path(path).filename().string();

    if (base_name.back() != '/') base_name += "/";

    return base_name;
}

inline void getSystemDate(int &year, int &month, int &day) {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    struct tm *parts = std::localtime(&now_c);

    year = 1900 + parts->tm_year;
    month = 1 + parts->tm_mon;
    day = parts->tm_mday;
}


inline std::string toTimeCode(unsigned long long ts_ms)
{
    int ms_ = ts_ms%1000;
    int seconds_ = (ts_ms/1000)%60;
    int minutes_ = (ts_ms/(1000*60))%60;
    int hours_ = (ts_ms/(1000*60*60))%24;

    std::stringstream ss_ms,ss_secs, ss_mins, ss_hours;
    ss_ms << std::dec << std::setw(3) << std::setfill('0')  << ms_;
    ss_secs <<std::dec << std::setw(2) << std::setfill('0') << seconds_;
    ss_mins << std::dec << std::setw(2) << std::setfill('0') << minutes_;
    ss_hours <<std::dec << std::setw(2) << std::setfill('0')  << hours_;

    std::string tc_str = ss_hours.str() +":"+  ss_mins.str() + ":"+ss_secs.str()+"."+ss_ms.str();
    return tc_str;

}
///////////////////////////////////////////////////

inline int countSubstring(const std::string& str, const std::string& sub)
{
    if (sub.length() == 0) return 0;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
     offset = str.find(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}

inline std::vector<std::string> split(const std::string &s, char delim) {
  std::stringstream ss(s);
  std::string item;
  std::vector<std::string> elems;
  while (std::getline(ss, item, delim)) {
    elems.push_back(item);
    // elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
  }
  return elems;
}

inline int parseParamJson(std::string parameter_name, sl_hub::json &parameter_value, sl_hub::json &parameters_in) {
    if (parameters_in.empty()) return 1;

    int ret = 0;
    parameter_value.clear();
    try {
        auto it = parameters_in.find(parameter_name);
        if (it != parameters_in.end()) parameter_value = it.value();
        else ret = 1;
    } catch (std::exception& e) {
        ret = 1;
    }
    return ret;
}

inline int parseParamJson(std::string parameter_name, std::string &parameter_value, sl_hub::json &parameters_in) {
    if (parameters_in.empty()) return 1;

    int ret = 0;
    parameter_value.clear();
    try {
        auto it = parameters_in.find(parameter_name);
        if (it != parameters_in.end()) parameter_value = it->get<std::string>();
        else ret = 1;
    } catch (std::exception& e) {
        ret = 1;
    }
    return ret;
}

inline int parseParamJson(std::string parameter_name, int &parameter_value, sl_hub::json &parameters_in) {
    if (parameters_in.empty()) return 1;
    int ret = 0;
    try {
        auto it = parameters_in.find(parameter_name);
        if (it != parameters_in.end()) {
            if (it.value().is_string()) parameter_value = std::atoi(it->get<std::string>().c_str());
            else parameter_value = it.value();
        } else ret = 1;
    } catch (std::exception& e) {
        ret = 1;
    }
    return ret;
}

inline int parseParamJson(std::string parameter_name, float &parameter_value, sl_hub::json &parameters_in) {
    if (parameters_in.empty()) return 1;
    int ret = 0;
    try {
        auto it = parameters_in.find(parameter_name);
        if (it != parameters_in.end()) {
            if (it.value().is_string()) parameter_value = std::atof(it->get<std::string>().c_str());
            else parameter_value = it.value();
        } else ret = 1;
    } catch (std::exception& e) {
        ret = 1;
    }
    return ret;
}

inline uint64_t getCurrentTS_nano() {
    return (uint64_t) std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline uint64_t getCurrentTS_micro() {
    return (uint64_t) std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline uint64_t getCurrentTS_milli() {
    return (uint64_t) std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline std::string formatPrecision(float f, int precision = 2) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << f;
    return stream.str();
}

//std::string getCurrentExeName() ;
std::string computeSHA1File(std::string filePath);

time_t getTimeFromFileName(const std::string &file_name);

#endif
