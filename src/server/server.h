#include "../database.h"
#include "../logger/logger.h"
#include "../query/parser.h"
#include <cstddef>
#include <memory>
#include <netinet/in.h>
#include <string>
#include <unordered_map>

class TcpServer {
private:
  int port;
  size_t buffer_size;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<Database> database;
  void handle_read(Query &query, int socket);
  void handle_insert(Query &query, int socket);
  void handle_quit(Query &query, int socket);
  bool exited;
  int server_fd;
  struct sockaddr_in address;
  void trim_buffer(std::string &buffer);
  std::shared_ptr<QueryParser> query_parser;
  std::unordered_map<DataType, std::shared_ptr<Encoder>> response_encoders;
  void execute_query(Query &query, int socket);

public:
  void listen();
  void initialize();
  TcpServer(
      std::shared_ptr<Logger> logger, std::shared_ptr<Database> database,
      std::shared_ptr<QueryParser> query_parser,
      std::unordered_map<DataType, std::shared_ptr<Encoder>> response_encoders,
      int port, size_t buffer_size);
  ~TcpServer();
};
