#include "cache/write_back_cache.h"
#include "datapoints/data_point.h"
#include <gtest/gtest.h>
#include <vector>

using namespace std;

int random_test() {

  std::map<char, int> mp;
  mp['a'] = 2;

  // auto res = mp['b'];
  auto res2 = mp.at('a');
  return res2;
}

std::vector<char> serialize_datapoint() {
  DataPoint<int> *point = new DataPoint(100, 12);
  return point->encode();
}

TEST(DataPointTest, Serialization) {
  EXPECT_EQ(100, serialize_datapoint()[0]);
  EXPECT_EQ(12, serialize_datapoint()[1]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
