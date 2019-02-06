#include "functions.h"

#include <sys/stat.h>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <string>

std::string get_dirname(std::string path) {
#ifndef __MINGW32__
  while (*path.rbegin() == '/') {
    path.erase(path.size() - 1);
  }
  std::string slash = "/", dot = ".";
  std::string::size_type last_slash, last_dot;
  last_slash = path.find_last_of(slash);
  if (last_slash == std::string::npos) {
    // last_slash = 0;
    return path;
  }
  return path.substr(last_slash + 1);
#else
  while (*path.rbegin() == '\\') {
    path.erase(path.size() - 1);
  }
  std::string slash = "\\", dot = ".";
  std::string::size_type last_slash, last_dot;
  last_slash = path.find_last_of(slash);
  if (last_slash == std::string::npos) {
    // last_slash = 0;
    return path;
  }
  return path.substr(last_slash + 1);
#endif
}

#ifdef __MINGW32__
#define mkdir(a, b) mkdir((a))
#endif

void create_all(std::string date_suffix, struct src_set sst) {
  std::string dest_dirname;
  for (auto src : sst.srcs) {
    if (src.type == 2) {
      dest_dirname =
          src.name + "/../" + get_dirname(src.name) + date_suffix + "/";
      if (mkdir(dest_dirname.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | /* rwx */
                                          S_IRGRP | S_IWGRP |
                                          S_IXGRP |                    /* rwx */
                                          S_IROTH | S_IXOTH | S_IXOTH) /* rwx */
          == 0) {
        std::string sub_dirname;
        for (auto dir : src.dir.dirlist) {
          sub_dirname = dest_dirname + dir;
          if (mkdir(sub_dirname.c_str(),
                    S_IRUSR | S_IWUSR | S_IXUSR |     /* rwx */
                        S_IRGRP | S_IWGRP | S_IXGRP | /* rwx */
                        S_IROTH | S_IXOTH | S_IXOTH)  /* rwx */
              == 0) {
            // OK
          } else {
            std::cerr << "create error" << std::endl;
          }
        }
      } else {
        std::cerr << "create error" << std::endl;
      }
    }
  }
}
#ifdef __MINGW32__
#undef mkdir
#endif

std::string append_date_suffix(std::string read_img_name,
                               std::string date_suffix) {
  std::string slash = "/", dot = ".";
  std::string::size_type last_slash, last_dot;
  std::string appended_filename;

  last_slash = read_img_name.find_last_of(slash);
  if (last_slash == std::string::npos) {
    last_slash = 0;
  }
  std::string filename = read_img_name.substr(last_slash);
  last_dot = filename.find_last_of(dot);
  if (last_dot == std::string::npos) {
    appended_filename = read_img_name + date_suffix + ".jpg";
    return appended_filename;
  }
  appended_filename =
      read_img_name.substr(
          0, std::max(last_slash + last_dot - 1, (std::string::size_type)0)) +
      date_suffix + filename.substr(last_dot);
  return appended_filename;
}

std::string get_date_suffix() {
  time_t timer;
  // timer = time(NULL);
  time(&timer);
  struct tm *timeinfo;
  // struct tm timeinfo;
  timeinfo = localtime(&timer);
  // localtime_s(&timeinfo, &timer);
  char sbuf[256];
  // printf("%s", ctime(&timer));
  // std::strftime(sbuf, 256, "_%F_%H%M", timeinfo);
  // strftime(sbuf, 256, "_%F_%H%M", &timeinfo);
  std::strftime(sbuf, 256, "_%Y-%m-%d_%H%M", timeinfo);
  // std::cerr << "year:\t" << timeinfo.tm_year << std::endl;
  // std::cerr << "mon:\t" << timeinfo.tm_mon << std::endl;
  // std::cerr << "mday:\t" << timeinfo.tm_mday << std::endl;
  // std::cerr << "hour:\t" << timeinfo.tm_hour << std::endl;
  // std::cerr << "min:\t" << timeinfo.tm_min << std::endl;
  // std::cerr << "sec:\t" << timeinfo.tm_sec << std::endl;
  // std::cerr << "date suffix: " << sbuf << std::endl;
  return std::string(sbuf);
}

std::string get_nth_img_name(std::string img_name, int n) {
  std::stringstream sst;
  std::string filename = img_name;
  if (n > 0) {
    sst << "_" << n << ".jpg";
    filename += sst.str();
  }
  return filename;
}

std::string get_exe_path_from_argv0(char *argv0) {
  std::string path = argv0;
  std::string slash_backslash = "/\\";
  std::string::size_type last_slash;
  last_slash = path.find_last_of(slash_backslash);
  if (last_slash == std::string::npos) {
    // last_slash = 0;
    return std::string("./");
  }
  return path.substr(0, last_slash + 1);
}
