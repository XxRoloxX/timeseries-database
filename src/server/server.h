#include "../database.h"
#include "../logger/logger.h"
#include <cstddef>
#include <memory>
#include <netinet/in.h>
#include <string>

class TcpServer {
private:
  int port;
  size_t buffer_size;
  std::shared_ptr<Logger> logger;
  std::shared_ptr<Database> database;
  void handle_read(std::vector<std::string> &tokens, int socket);
  void handle_insert(std::vector<std::string> &tokens, int socket);
  void handle_quit(std::vector<std::string> &tokens, int socket);
  bool exited;
  int server_fd;
  struct sockaddr_in address;
  void trim_buffer(std::string &buffer);

public:
  void listen();
  void initialize();
  TcpServer(std::shared_ptr<Logger> logger, std::shared_ptr<Database> database,
            int port, size_t buffer_size);
  ~TcpServer();
};
