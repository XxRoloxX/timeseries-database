#include <gtest/gtest.h>
#include <vector>

#include "../src/encoding/binary_encoder.h"
#include "../src/indexing/indexed_ss_table_reader.h"
#include "../src/indexing/indexed_ss_table_writer.h"
#include <memory>

std::vector<DataPoint> test_read_index(DataPointKey start_key,
                                       DataPointKey end_key) {

  std::shared_ptr<Decoder> decoder = std::make_shared<DataPointDecoder>();
  std::shared_ptr<Encoder> encoder = std::make_shared<DataPointEncoder>();
  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();

  std::vector<DataPoint> data = {
      DataPoint(12, std::vector<char>{13}),
      DataPoint(13, std::vector<char>{14}),
      DataPoint(13, std::vector<char>{14}),
      DataPoint(13, std::vector<char>{14}),
      DataPoint(13, std::vector<char>{14}),
      DataPoint(17, std::vector<char>{14}),

  };

  auto ss_table_writer =
      std::make_shared<IndexedSSTableWriter>(logger, decoder, encoder);

  ss_table_writer->save("table1",
                        std::make_shared<std::vector<DataPoint>>(data));

  std::shared_ptr<SSTableStorage> raw_table =
      std::make_shared<SSTableStorage>(logger, "table1");

  raw_table->initialize();

  auto indexed_table = std::make_shared<IndexedSSTableReader>(logger, raw_table,
                                                              decoder, encoder);
  indexed_table->initialize();

  auto datapoints = indexed_table->read_range(12, 17);

  return *(indexed_table->read_range(start_key, end_key));
}

TEST(IndexReadTest, FullRange) {
  EXPECT_EQ(12, test_read_index(12, 17)[0].get_key());
  EXPECT_EQ(13, test_read_index(12, 17)[0].get_value()[0]);
  EXPECT_EQ(13, test_read_index(12, 17)[1].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[1].get_value()[0]);
  EXPECT_EQ(13, test_read_index(12, 17)[2].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[2].get_value()[0]);
  EXPECT_EQ(17, test_read_index(12, 17)[5].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[5].get_value()[0]);
}

TEST(IndexReadTest, NotFullRange) {
  EXPECT_EQ(13, test_read_index(13, 14)[0].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[0].get_value()[0]);
  EXPECT_EQ(13, test_read_index(13, 14)[1].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[1].get_value()[0]);
  EXPECT_EQ(13, test_read_index(13, 14)[2].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[2].get_value()[0]);
  EXPECT_EQ(13, test_read_index(13, 14)[3].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[3].get_value()[0]);
}

TEST(IndexReadTest, OverRange) {
  EXPECT_EQ(12, test_read_index(10, 20)[0].get_key());
  EXPECT_EQ(13, test_read_index(10, 20)[0].get_value()[0]);
  EXPECT_EQ(13, test_read_index(10, 20)[1].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[1].get_value()[0]);
  EXPECT_EQ(13, test_read_index(10, 20)[2].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[2].get_value()[0]);
  EXPECT_EQ(17, test_read_index(10, 20)[5].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[5].get_value()[0]);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
