#include <gtest/gtest.h>
#include <vector>

#include "../src/encoding/binary_encoder.h"
#include "../src/indexing/indexed_ss_table_reader.h"
#include "../src/indexing/indexed_ss_table_writer.h"
#include <memory>

int random_test() {

  std::map<char, int> mp;
  mp['a'] = 2;

  auto res2 = mp.at('a');
  return res2;
}

std::vector<char> serialize_datapoint() {
  DataPoint<int> *point = new DataPoint(100, 12);
  return point->encode();
}

std::vector<DataPoint<int>> test_read_index(DataPointKey start_key,
                                            DataPointKey end_key) {

  std::shared_ptr<Decoder<int>> decoder =
      std::make_shared<DataPointDecoder<int>>();
  std::shared_ptr<Encoder<int>> encoder =
      std::make_shared<DataPointEncoder<int>>();

  std::shared_ptr<Logger> logger = std::make_shared<StdLogger>();

  std::vector<DataPoint<int>> data = {
      DataPoint<int>(12, 12), DataPoint<int>(13, 14), DataPoint<int>(13, 14),
      DataPoint<int>(13, 14), DataPoint<int>(13, 14), DataPoint<int>(17, 14),
  };

  auto ss_table_writer =
      std::make_shared<IndexedSSTableWriter<int>>(logger, decoder, encoder);

  ss_table_writer->save("table1",
                        std::make_shared<std::vector<DataPoint<int>>>(data));

  std::shared_ptr<SSTableStorage> raw_table =
      std::make_shared<SSTableStorage>(logger, "table1");

  raw_table->initialize();

  auto indexed_table = std::make_shared<IndexedSSTableReader<int>>(
      logger, raw_table, decoder, encoder);

  indexed_table->initialize();

  return *(indexed_table->read_range(start_key, end_key));
}

TEST(IndexReadTest, FullRange) {
  EXPECT_EQ(12, test_read_index(12, 17)[0].get_key());
  EXPECT_EQ(12, test_read_index(12, 17)[0].get_value());
  EXPECT_EQ(13, test_read_index(12, 17)[1].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[1].get_value());
  EXPECT_EQ(13, test_read_index(12, 17)[2].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[2].get_value());
  EXPECT_EQ(17, test_read_index(12, 17)[5].get_key());
  EXPECT_EQ(14, test_read_index(12, 17)[5].get_value());
}

TEST(IndexReadTest, NotFullRange) {
  EXPECT_EQ(13, test_read_index(13, 14)[0].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[0].get_value());
  EXPECT_EQ(13, test_read_index(13, 14)[1].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[1].get_value());
  EXPECT_EQ(13, test_read_index(13, 14)[2].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[2].get_value());
  EXPECT_EQ(13, test_read_index(13, 14)[3].get_key());
  EXPECT_EQ(14, test_read_index(13, 14)[3].get_value());
}

TEST(IndexReadTest, OverRange) {
  EXPECT_EQ(12, test_read_index(10, 20)[0].get_key());
  EXPECT_EQ(12, test_read_index(10, 20)[0].get_value());
  EXPECT_EQ(13, test_read_index(10, 20)[1].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[1].get_value());
  EXPECT_EQ(13, test_read_index(10, 20)[2].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[2].get_value());
  EXPECT_EQ(17, test_read_index(10, 20)[5].get_key());
  EXPECT_EQ(14, test_read_index(10, 20)[5].get_value());
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
