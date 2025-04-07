#include "../database.h"
#include "../logger/logger.h"
#include <cstddef>
#include <memory>

class TcpServer {
private:
  int port;
  size_t buffer_size;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<Database> database;

public:
  void listen();
  TcpServer(std::shared_ptr<Logger> logger, std::shared_ptr<Database> database,
            int port, size_t buffer_size);
};
