#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include "../src/database.h"
#include "../src/encoding/binary_encoder.h"
#include "../src/indexing/indexed_ss_table_writer.h"
#include "../src/storage/storage_manager.h"

std::vector<DataPoint> test_read_index(DataPointKey start_key,
                                       DataPointKey end_key) {

  // clear the test directory for the test test
  if (std::filesystem::is_directory("./test_storage")) {
    std::filesystem::remove_all("./test_storage");
  }

  std::shared_ptr<Decoder> decoder = std::make_shared<DataPointDecoder>();
  std::shared_ptr<Encoder> encoder = std::make_shared<DataPointEncoder>();
  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();

  std::vector<DataPoint> data = {
      DataPoint(12, std::vector<char>{13}),
      DataPoint(13, std::vector<char>{14}),
      DataPoint(14, std::vector<char>{14}),
      DataPoint(15, std::vector<char>{14}),
      DataPoint(16, std::vector<char>{14}),
      DataPoint(17, std::vector<char>{14}),

  };

  auto ss_table_writer =
      std::make_shared<IndexedSSTableWriter>(logger, decoder, encoder);

  std::shared_ptr<WriteBackCache<DataPointKey, DataPoint>> cache =
      std::make_shared<MemTable<DataPointKey, DataPoint>>(logger);

  std::shared_ptr<StorageManager> storage_manager =
      std::make_shared<StorageManager>(logger, decoder, encoder, cache,
                                       ss_table_writer, "test_storage");

  Database database =
      Database(logger, decoder, encoder, 3, ss_table_writer, storage_manager);

  for (auto point : data) {
    database.insert("test_series", point.get_key(), point.get_value());
  }

  auto results = database.read("test_series", start_key, end_key);

  return results;
}

TEST(IndexReadTest, FullRange) {
  EXPECT_EQ(12, test_read_index(12, 17)[0].get_key());
  EXPECT_EQ(13, test_read_index(12, 17)[0].get_value()[0]);
  EXPECT_EQ(13, test_read_index(12, 17)[1].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[1].get_value()[0]);
  EXPECT_EQ(14, test_read_index(12, 17)[2].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[2].get_value()[0]);
  EXPECT_EQ(17, test_read_index(12, 17)[5].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[5].get_value()[0]);
}

TEST(IndexReadTest, NotFullRange) {
  EXPECT_EQ(13, test_read_index(13, 14)[0].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[0].get_value()[0]);
  EXPECT_EQ(14, test_read_index(13, 14)[1].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[1].get_value()[0]);
}

TEST(IndexReadTest, OverRange) {
  EXPECT_EQ(12, test_read_index(10, 20)[0].get_key());
  EXPECT_EQ(13, test_read_index(10, 20)[0].get_value()[0]);
  EXPECT_EQ(13, test_read_index(10, 20)[1].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[1].get_value()[0]);
  EXPECT_EQ(14, test_read_index(10, 20)[2].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[2].get_value()[0]);
  EXPECT_EQ(17, test_read_index(10, 20)[5].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[5].get_value()[0]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
