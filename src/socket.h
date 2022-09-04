// Copyright 2022 Daniel Liu

#ifndef _CPPSERVER_SOCKET_H_
#define _CPPSERVER_SOCKET_H_

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace cppserver {

enum SocketDomain {
    kIPV4 = AF_INET,
    kIPV6 = AF_INET6,
};

enum SocketType {
    kTCP  = SOCK_STREAM,
    kUDP  = SOCK_DGRAM,
};

// A more user-friendly wrapper for `struct sockaddr`.
class SocketSockAddr {
 public:
  // Initializes the SockAddr struct with the given address and port.
  SocketSockAddr(std::optional<std::string> address, in_port_t port);

  explicit SocketSockAddr(struct sockaddr addr) : addr_{addr} {}

  const struct sockaddr& rawValue() const { return addr_; }

 private:
  struct sockaddr addr_;
};

// A wrapper for the C websocket interface.
class Socket {
 public:
  // Initializes the socket with the given domain and type.
  // `protocol` can be set to a custom value if you know what you're doing.
  Socket(SocketDomain domain, SocketType type, int protocol = 0);

  // Closes the socket.
  ~Socket() {
    Close();
  }

  Socket(const Socket&) = delete;
  Socket& operator=(const Socket&) = delete;

  Socket(Socket&& other) : fd_{other.fd_}, status_{other.status_} {
    other.fd_ = -1;
  }

  // Checks if the socket is valid.
  operator bool() const { return status_.ok(); }

  // Get the file descriptor of the socket.
  int GetFD() const { return fd_; }

  // Get the status message.
  const absl::Status& Status() const { return status_; }

  // Clear the status message.
  void ClearStatus() { status_ = absl::OkStatus(); }

  // Accept a connection, returning a socket object to the connection as well
  // as the socket's address and port.
  std::optional<std::pair<Socket, SocketSockAddr>> Accept();

  // Bind the socket to the given address and port.
  void Bind(const SocketSockAddr& addr);

  // Close the socket.
  void Close() {
    if (fd_ > 0) {
      close(fd_);
      fd_ = -1;
    }
  }

  // Connect to the given address and port.
  void Connect(const SocketSockAddr& addr);

  // Listen on the socket.
  void Listen(int backlog);

  // Receive a message.
  template <typename T>
  std::optional<std::pair<std::unique_ptr<T[]>, ssize_t>> Receive(
    size_t len,
    int flags = 0);

  // Send a message.
  template <typename T>
  ssize_t Send(const T* data, size_t len, int flags = 0);

 private:
  // Private constructor used to return a socket based on file descriptor.
  explicit Socket(int fd) : fd_{fd}, status_{absl::OkStatus()} {}

  // File descriptor of the associated socket.
  // fd_ is set to -1 if the socket is in an invalid state (usually since it
  // was closed)
  int fd_;

  // Status of the socket; if an error occurs, status_ will be set.
  absl::Status status_;
};

}  // namespace cppserver

#endif
