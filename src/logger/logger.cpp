#include "logger.h"
#include <format>
#include <fstream>
#include <iostream>
#include <string>

const std::string LOG_PATH = "tsdb.log";

void StdLogger::info(std::string msg) {
  std::ofstream log_file;

  time_t timestamp;
  time(&timestamp);

  log_file.open(LOG_PATH, std::ios_base::app);

  struct tm *p = localtime(&timestamp);

  char s[1000];

  strftime(s, sizeof s, "%B %d %Y %H\:%M", p);

  auto log = std::format("\033[1;33m[{}] INFO\033[0m {}", s, msg);

  log_file << log << std::endl;
  log_file.close();
}

void StdLogger::error(std::string msg) {
  std::ofstream log_file;

  time_t timestamp;
  time(&timestamp);

  log_file.open(LOG_PATH, std::ios_base::app);

  struct tm *p = localtime(&timestamp);

  char s[1000];

  strftime(s, sizeof s, "%B %d %Y %H\:%M", p);

  auto log = std::format("\033[1;32m[{}] ERROR\033[0m {}", s, msg);

  log_file << log << std::endl;
  log_file.close();
}
