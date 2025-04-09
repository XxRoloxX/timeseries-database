#include "server.h"
#include "../typed_series/typed_series.h"
#include "../utils.h"
#include <asm-generic/socket.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <numbers>
#include <stdexcept>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

std::unordered_map<std::string, Operation> stringToOperationMap = {
    {"r", Operation::READ},
    {"i", Operation::INSERT},
    {"q", Operation::QUIT},
};

TcpServer::TcpServer(std::shared_ptr<Logger> logger,
                     std::shared_ptr<Database> database, int port,
                     size_t buffer_size)
    : port(port), buffer_size(buffer_size), logger(logger),
      database(database) {};

void TcpServer::initialize() {

  int opt = 1;

  this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (this->server_fd == 0) {
    this->logger->error("Failed to create a socket");
    exit(EXIT_FAILURE);
  }

  setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  this->address.sin_family = AF_INET;
  this->address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  this->address.sin_port = htons(this->port);

  if (bind(this->server_fd, (struct sockaddr *)&this->address,
           sizeof(this->address)) < 0) {
    this->logger->error("Failed to bind the socket");
    exit(EXIT_FAILURE);
  }

  if (::listen(this->server_fd, 3) < 0) {
    this->logger->error("Failed to listen on the socket");
    exit(EXIT_FAILURE);
  }
}

void TcpServer::trim_buffer(std::string &buffer) {
  buffer.erase(0, buffer.find_first_not_of(" \t\n\r"));

  buffer.erase(buffer.find_last_not_of(" \t\n\r") + 1);
}

void TcpServer::listen() {

  char *buffer = new char[this->buffer_size];

  fd_set read_fds;

  while (!this->exited) {

    auto address_len = sizeof(this->address);

    auto new_socket = accept(server_fd, (struct sockaddr *)&address,
                             (socklen_t *)&address_len);

    if (new_socket < 0) {
      this->logger->error("Failed to accept a connection on the socket");
      exit(EXIT_FAILURE);
    }

    bool timed_out = false;

    while (!timed_out && !this->exited) {
      timed_out = false;

      // Fill the buffer with zeros
      std::memset(buffer, 0, this->buffer_size);

      FD_ZERO(&read_fds);
      FD_SET(new_socket, &read_fds);

      // Block forever, until data is available
      int activity =
          select(new_socket + 1, &read_fds, nullptr, nullptr, nullptr);

      if (!(activity > 0) || !FD_ISSET(new_socket, &read_fds)) {
        this->logger->error("Connection timeout");
        timed_out = true;
      }

      int data_read = read(new_socket, buffer, this->buffer_size);

      if (data_read == 0) {
        this->logger->info("Connection was closed by client");
        timed_out = true;
      }

      std::string line(buffer);

      auto tokens = split(line, " ");
      if (tokens.size() == 0) {
        this->logger->error("Invalid empty command: " + line);
        break;
      }

      Operation operation = Operation::INVALID;
      auto it = stringToOperationMap.find(tokens[0]);
      if (it != stringToOperationMap.end()) {
        operation = it->second;
      }

      switch (operation) {
      case Operation::READ:
        this->handle_read(tokens, new_socket);
        break;
      case Operation::INSERT:
        this->handle_insert(tokens, new_socket);
        break;
      case Operation::QUIT:
        this->handle_quit(tokens, new_socket);
        break;
      case Operation::INVALID:
        this->logger->error(
            std::format("Invalid operation: {}", tokens[0].size()));
        break;
      default:
        break;
      }
    }

    close(new_socket);
  };

  delete[] buffer;
}

TcpServer::~TcpServer() {
  if (server_fd != 0) {
    close(server_fd);
  }
}

void TcpServer::handle_read(std::vector<std::string> &tokens, int socket) {

  if (tokens.size() != 3) {
    this->logger->error(std::format("Invalid command: {}", tokens[0]));
    return;
  }

  auto start = stoi(tokens[1], 0, 10);
  auto end = stoi(tokens[2], 0, 10);

  this->logger->info(std::format("got tcp request: read {} {}", start, end));

  TypedSeries<int> int_series("perf_test", database);
  auto read_points = int_series.read(start, end);

  std::string response;

  for (auto &point : read_points) {
    response += point.to_string() + "\n";
  }

  char *response_buff = new char[response.size()];

  std::strcpy(response_buff, response.c_str());

  write(socket, response_buff, response.size());

  delete[] response_buff;
}

void TcpServer::handle_insert(std::vector<std::string> &tokens, int socket) {

  if (tokens.size() != 3) {
    this->logger->error(std::format("Invalid command: {}", tokens[0]));
    return;
  }

  TypedSeries<int> int_series("perf_test", database);

  if (tokens.size() != 3) {
    this->logger->error("Invalid command: " + tokens[0]);
    return;
  }

  auto key = stoi(tokens[1], 0, 10);
  int value = stoi(tokens[2], 0, 10);

  int_series.insert(key, value);
  this->logger->info(std::format("got tcp request: insert {} {}", key, value));
}

void TcpServer::handle_quit(std::vector<std::string> &tokens, int socket) {

  if (tokens.size() != 1) {
    this->logger->error(std::format("Invalid command: {}", tokens[0]));
    return;
  }

  close(socket);

  this->exited = true;
}
