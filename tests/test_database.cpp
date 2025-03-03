
#include "data_point.h"
#include "database.h"
#include <gtest/gtest.h>
#include <memory>
#include <vector>

using namespace std;

int insert_test() {
  auto point = std::make_shared<DataPoint<int>>(DataPoint(100, 12));
  auto *db = new Database(10);
  db->insert("series_1", 100, static_pointer_cast<Encodable>(point));

  auto records = db->read(10, 200);

  return records.size();
}

TEST(DatabaseTest, Insert) { EXPECT_EQ(1, insert_test()); }

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
