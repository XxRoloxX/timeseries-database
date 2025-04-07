#include "./datapoints/data_point.h"
#include <string>
#include <vector>

std::vector<std::string> split(std::string input, std::string delimiter) {

  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;

  while ((pos = input.find(delimiter)) != std::string::npos) {
    token = input.substr(0, pos);
    tokens.push_back(token);
    input.erase(0, pos + 1);
  }

  tokens.push_back(input);

  return tokens;
}

std::vector<DataPoint> merge_points(std::vector<DataPoint> *a,
                                    std::vector<DataPoint> *b) {

  std::vector<DataPoint> result;

  if (a->size() == 0) {
    return *b;
  }

  if (b->size() == 0) {
    return *a;
  }

  size_t a_index = 0;
  size_t b_index = 0;

  while (a_index != a->size() || b_index != b->size()) {

    if (a_index == a->size()) {
      result.push_back(b->at(b_index));
      b_index++;
      continue;
    }

    if (b_index == b->size()) {
      result.push_back(a->at(a_index));
      a_index++;
      continue;
    }

    if (a->at(a_index).get_key() <= b->at(b_index).get_key()) {
      result.push_back(a->at(a_index));
      a_index++;
      continue;
    }

    if (a->at(a_index).get_key() > b->at(b_index).get_key()) {
      result.push_back(b->at(b_index));
      b_index++;
      continue;
    }
  }

  return std::move(result);
}
