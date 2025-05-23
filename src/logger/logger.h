#pragma once

#include "../encoding/encodable.h"
#include <string>

class Logger {
public:
  virtual void info(std::string msg) = 0;
  virtual void error(std::string msg) = 0;
};

class StdLogger : public Logger {
public:
  void info(std::string msg) override;
  void error(std::string msg) override;
};

class DummyLogger : public Logger {
public:
  void info(std::string msg) override;
  void error(std::string msg) override;
};
