
#include "database.h"
#include "datapoints/data_point.h"
#include "storage/ss_table_storage.h"
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

// int write_test(){
//   auto table = new RawSSTableWriter<char>();
//   auto vec = vector<char>();
//   vec.push_back('a');
//   vec.push_back('b');
//   vec.push_back('c');
//   table->write("./test_table", vec);
//   return 1;
// }

TEST(DatabaseTest, Insert) { EXPECT_EQ(1, insert_test()); }

// TEST(RawSSTableWriterTest, Write) { EXPECT_EQ(1, write_test()); }
