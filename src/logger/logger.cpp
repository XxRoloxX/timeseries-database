#include "logger.h"
#include <iostream>
#include <string>

void StdLogger::info(std::string msg) {
  std::cout << "INFO: " << msg << std::endl;
}
void StdLogger::error(std::string msg) {
  std::cout << "ERROR: " << msg << std::endl;
}
