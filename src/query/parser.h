#include "../database.h"
#include <memory>
#include <string>
#include <vector>

enum Operation { READ, INSERT, QUIT, INVALID };

enum DataType { INT, FLOAT, BINARY };

class IQuery {
public:
  virtual void execute(std::shared_ptr<Database> database) = 0;
};

class RawReadQuery : IQuery {
private:
  std::string series;
  DataPointKey from;
  DataPointKey to;

public:
  void execute(std::shared_ptr<Database> database);
};

template <typename T> class TypedReadQuery : IQuery {
private:
  std::string series;
  DataPointKey from;
  DataPointKey to;

public:
  void execute(std::shared_ptr<Database> database);
};

class RawInsertQuery : IQuery {
private:
  std::string series;
  DataPointKey key;
  DataPointValue value;

public:
  void execute(std::shared_ptr<Database> database);
};

class QueryParser {};
