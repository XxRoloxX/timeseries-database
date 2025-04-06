#include "./datapoints/data_point.h"
#include <string>
#include <vector>

std::vector<std::string> split(std::string input, std::string delimiter);
std::vector<DataPoint> merge_points(std::vector<DataPoint> *a,
                                    std::vector<DataPoint> *b);
