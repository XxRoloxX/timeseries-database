#include "../datapoints/data_point.h"
#include "../logger/logger.h"
#include <string>
#include <vector>

#pragma once

enum Operation { READ, INSERT, QUIT, INVALID };

std::string operation_to_string(Operation operation);

enum DataType { INT, FLOAT, BINARY };

DataType datatype_from_string();

struct ReadParams {
  DataPointKey range_start;
  DataPointKey range_end;
};

struct InsertParams {
  DataPointKey key;
  DataPointValue value;
};

class Query {
public:
  std::string series;
  Operation operation;
  ReadParams read_params;
  InsertParams insert_params;
  DataType data_type;

  Query *set_series(std::string series);
  Query *set_operation(Operation &operation);
  Query *set_read_params(ReadParams &params);
  Query *set_insert_params(InsertParams &params);
  Query *set_data_type(DataType data_type);
  std::string to_string();
};

class QueryParser {
public:
  virtual Query to_query(char *buffer) = 0;
  virtual ~QueryParser() = default;
};

class PlaintextQueryParser : public QueryParser {
public:
  PlaintextQueryParser(std::shared_ptr<Logger> logger);
  Query to_query(char *buffer) override;

private:
  std::shared_ptr<Logger> logger;
  Query parse_read_query(std::vector<std::string> tokens);
  Query parse_insert_query(std::vector<std::string> tokens);
  Query parse_quit_query(std::vector<std::string> tokens);
  std::vector<std::string> split_tokens(char *buffer);
};
