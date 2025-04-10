#include "server.h"
#include "../typed_series/typed_series.h"
#include "../utils.h"
#include <asm-generic/socket.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

TcpServer::TcpServer(
    std::shared_ptr<Logger> logger, std::shared_ptr<Database> database,
    std::shared_ptr<QueryParser> query_parser,
    std::unordered_map<DataType, std::shared_ptr<Encoder>> response_encoders,
    int port, size_t buffer_size)
    : port(port), buffer_size(buffer_size), logger(logger), database(database),
      query_parser(query_parser), response_encoders(response_encoders) {};

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

      auto trimmed_buf = trim(buffer);

      if (data_read == 0 || trimmed_buf == nullptr) {
        this->logger->info("Connection was closed by client");
        timed_out = true;
      }

      try {
        Query query = this->query_parser->to_query(trimmed_buf);
        this->execute_query(query, new_socket);
      } catch (std::invalid_argument err) {
        this->logger->error(
            std::format("failed to parse the command: {}", err.what()));
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

void TcpServer::execute_query(Query &query, int socket) {
  switch (query.operation) {
  case Operation::READ:
    return this->handle_read(query, socket);
  case Operation::INSERT:
    return this->handle_insert(query, socket);
  case Operation::QUIT:
    return this->handle_quit(query, socket);
  case Operation::INVALID:
    return;
  }
}

void TcpServer::handle_read(Query &query, int socket) {

  this->logger->info(
      std::format("got read request over tcp: {}", query.to_string()));

  auto read_points = this->database->read(
      query.series, query.read_params.range_start, query.read_params.range_end);

  auto encoded_response = this->response_encoders[query.data_type]->encode_many(
      std::make_shared<std::vector<DataPoint>>(read_points));

  write(socket, encoded_response.data(), encoded_response.size());
}

void TcpServer::handle_insert(Query &query, int socket) {

  this->logger->info(
      std::format("got insert request over tcp: {}", query.to_string()));

  database->insert(query.series, query.insert_params.key,
                   query.insert_params.value);
}

void TcpServer::handle_quit(Query &query, int socket) {

  close(socket);

  this->exited = true;
}
