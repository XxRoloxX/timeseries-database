#include "server.h"
#include "../typed_series/typed_series.h"
#include "../utils.h"
#include <asm-generic/socket.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

TcpServer::TcpServer(std::shared_ptr<Logger> logger,
                     std::shared_ptr<Database> database, int port,
                     size_t buffer_size)
    : port(port), buffer_size(buffer_size), logger(logger),
      database(database) {};

void TcpServer::listen() {

  int server_fd, new_socket;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  char *buffer = new char[this->buffer_size];

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    this->logger->error("Failed to create a socket");
    exit(EXIT_FAILURE);
  }

  setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  address.sin_port = htons(this->port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    this->logger->error("Failed to bind the socket");
    exit(EXIT_FAILURE);
  }

  if (::listen(server_fd, 3) < 0) {
    this->logger->error("Failed to listen on the socket");
    exit(EXIT_FAILURE);
  }

  TypedSeries<int> int_series("perf_test", database);

  while (true) {

    new_socket =
        accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

    if (new_socket < 0) {
      this->logger->error("Failed to accept a connection on the socket");
      exit(EXIT_FAILURE);
    }

    buffer = new char[this->buffer_size];
    std::memset(buffer, 0, this->buffer_size); // Fill the buffer with zeros
    // while (true) {

    int data_read = read(new_socket, buffer, this->buffer_size);

    if (data_read == 0) {

      this->logger->info("exiting");
      break;
    }

    // for (std::string line; std::getline(std::cin, line);) {
    std::string line(buffer);

    this->logger->info("Got " + line);
    auto tokens = split(line, " ");

    if (tokens.size() == 0) {
      this->logger->error("Invalid command: " + line);
      break;
    }

    auto operation = tokens[0];

    if (operation == "r") {

      if (tokens.size() != 3) {
        this->logger->error("Invalid command: " + line);
        break;
      }

      auto key = stoi(tokens[1], 0, 10);
      auto value = stoi(tokens[2], 0, 10);
      auto read_points = int_series.read(key, value);
      std::string response;

      for (auto &point : read_points) {
        response += point.to_string() + "\n";
      }

      char *response_buff = new char[response.size()];

      std::strcpy(response_buff, response.c_str());

      write(new_socket, response_buff, response.size());

    } else if (operation == "i") {

      if (tokens.size() != 3) {
        this->logger->error("Invalid command: " + line);
        return;
      }

      auto key = stoi(tokens[1], 0, 10);
      int value = stoi(tokens[2], 0, 10);

      int_series.insert(key, value);
      std::cout << (std::format("inserted: {}, {}", key, value)) << std::endl;
      ;
    } else {
      this->logger->error("Invalid command: " + line);
      break;
      ;
    }

    close(new_socket);
  }

  close(server_fd);
}
//}
