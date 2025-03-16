#pragma once

#include <string>
#include <variant>

struct Error {
  std::string message;
};

template <typename T> using Result = std::variant<T, Error>;
