#include "parser.h"
#include "../utils.h"
#include <stdexcept>
#include <unordered_map>

Query *Query::set_operation(Operation &new_operation) {
  this->operation = new_operation;
  return this;
};
Query *Query::set_read_params(ReadParams &params) {
  this->read_params = params;
  return this;
};

Query *Query::set_insert_params(InsertParams &params) {
  this->insert_params = params;
  return this;
}

std::unordered_map<std::string, DataType> stringToDataTypeMap = {
    {"int", DataType::INT},
    {"float", DataType::FLOAT},
    {"binary", DataType::BINARY},
};

std::unordered_map<DataType, std::string> dataTypeToStringMap = {
    {DataType::INT, "int"},
    {DataType::FLOAT, "float"},
    {DataType::BINARY, "binary"},
};

DataType datatype_from_string(std::string raw_datatype) {

  DataType type = DataType::INT;

  auto it = stringToDataTypeMap.find(raw_datatype);
  if (it != stringToDataTypeMap.end()) {
    type = it->second;
  }

  return type;
}

std::string datatype_to_string(DataType datatype) {

  auto it = dataTypeToStringMap.find(datatype);
  if (it != dataTypeToStringMap.end()) {
    return it->second;
  }

  return "INVALID";
}

std::unordered_map<std::string, Operation> stringToOperationMap = {
    {"r", Operation::READ},
    {"i", Operation::INSERT},
    {"q", Operation::QUIT},
};

Operation operation_from_string(std::string operation_str) {

  Operation operation = Operation::INVALID;
  auto it = stringToOperationMap.find(operation_str);
  if (it != stringToOperationMap.end()) {
    operation = it->second;
  }

  return operation;
}

PlaintextQueryParser::PlaintextQueryParser(std::shared_ptr<Logger> logger)
    : logger(logger) {}

Query PlaintextQueryParser::to_query(char *buffer) {

  std::string stringified_buffer(buffer);
  auto tokens = split(stringified_buffer, " ");

  if (tokens.size() <= 1) {
    this->logger->error("Invalid empty command: " + stringified_buffer);
    throw std::invalid_argument("Invalid command");
  }

  std::string series = tokens[0];
  Operation operation = operation_from_string(tokens[1]);

  switch (operation) {
  case Operation::READ:
    return this->parse_read_query(tokens);
  case Operation::INSERT:
    return this->parse_insert_query(tokens);
  case Operation::QUIT:
    return this->parse_quit_query(tokens);
  case Operation::INVALID:
    this->logger->error(std::format("Invalid operation: {}", tokens[1]));
    throw std::invalid_argument("Invalid operation");
  default:
    throw std::invalid_argument("Invalid operation");
  }
}

Query PlaintextQueryParser::parse_read_query(std::vector<std::string> tokens) {

  if (tokens.size() != 5) {
    this->logger->error(std::format("Invalid command: {}", tokens[1]));
    throw std::invalid_argument("Invalid command");
  }

  DataType data_type = datatype_from_string(tokens[2]);

  auto start = stoi(tokens[3], 0, 10);
  auto end = stoi(tokens[4], 0, 10);

  Query query;
  std::string series = tokens[0];
  Operation operation = Operation::READ;
  ReadParams read_params = ReadParams{.range_start = start, .range_end = end};

  query.set_operation(operation)
      ->set_read_params(read_params)
      ->set_series(series)
      ->set_data_type(data_type);

  return query;
}

Query PlaintextQueryParser::parse_insert_query(
    std::vector<std::string> tokens) {

  if (tokens.size() != 5) {
    this->logger->error(std::format("Invalid command: {}", tokens[1]));
    throw std::invalid_argument("Invalid command");
  }

  Query query;
  std::string series = tokens[0];
  DataType data_type = datatype_from_string(tokens[2]);

  DataPointKey key = stoi(tokens[3], 0, 10);
  std::string raw_value_tokens =
      join(std::vector<std::string>(tokens.begin() + 4, tokens.end()));

  EncodedBuffer value;

  switch (data_type) {
  case DataType::INT: {
    int int_value = stoi(raw_value_tokens);
    value.resize(sizeof(int));
    std::memcpy(value.data(), &int_value, sizeof(int));
    break;
  }
  case DataType::FLOAT: {

    float float_value = stof(raw_value_tokens);
    value.resize(sizeof(float));
    std::memcpy(value.data(), &float_value, sizeof(int));
    break;
  }
  case DataType::BINARY: {
    value.resize(0);
    value.insert(value.end(), raw_value_tokens.begin(), raw_value_tokens.end());
    break;
  }
  }

  Operation operation = Operation::INSERT;
  InsertParams params = InsertParams{.key = key, .value = value};

  query.set_operation(operation)
      ->set_insert_params(params)
      ->set_series(series)
      ->set_data_type(data_type);
  return query;
}

Query PlaintextQueryParser::parse_quit_query(std::vector<std::string> tokens) {
  Query query;
  Operation operation = Operation::QUIT;
  query.set_operation(operation);
  return query;
}

Query *Query::set_series(std::string new_series) {
  this->series = new_series;
  return this;
}

Query *Query::set_data_type(DataType new_data_type) {
  this->data_type = new_data_type;
  return this;
}

std::string Query::to_string() {

  switch (this->operation) {
  case Operation::READ:
    return std::format("Query[series, {}, operation: {}, data_type: {}, "
                       "read_params: (start: {}, end: {})]",
                       this->series, operation_to_string(this->operation),
                       datatype_to_string(this->data_type),
                       this->read_params.range_start,
                       this->read_params.range_end);
  case Operation::INSERT:
    return std::format(
        "Query[series: {}, operation: {}, data_type: {}, insert_params: (key: "
        "{}, value:{})]",
        this->series, operation_to_string(this->operation),
        datatype_to_string(this->data_type), this->insert_params.key,
        std::string(this->insert_params.value.data()));
  case Operation::QUIT:
    return "Query[operation: QUIT";
  case Operation::INVALID:
    return "Query[operation: Invalid]";
  }
}
std::string operation_to_string(Operation operation) {
  switch (operation) {
  case Operation::READ:
    return "READ";
  case Operation::INSERT:
    return "INSERT";
  case Operation::QUIT:
    return "QUIT";
  case Operation::INVALID:
    return "INVALID";
  }

  return "";
}
