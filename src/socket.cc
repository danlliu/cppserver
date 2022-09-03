// Copyright 2022 Daniel Liu

#include "socket.h"

#include <sys/types.h>
#include <sys/socket.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace cppserver {

SocketSockAddr::SocketSockAddr(std::optional<std::string> address,
                               in_port_t port) {
  struct sockaddr_in addr_in;
  if (address) {
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = inet_addr(address.value().c_str());
  } else {
    addr_in.sin_family = AF_INET;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = INADDR_ANY;
  }

  // Look, it's not my fault they decided to design it this way.
  addr_ = absl::bit_cast<sockaddr>(addr_in);
}

Socket::Socket(SocketDomain domain, SocketType type, int protocol) {
  fd_ = socket(domain, type, protocol);
  status_ = absl::OkStatus();

  if (fd_ < 0) {
    status_ = absl::Status(absl::StatusCode::kInternal, "Initialization failed");
  }
}

std::optional<std::pair<Socket, SocketSockAddr>> Socket::Accept() {
  if (!status_.ok()) {
    return {};
  }

  struct sockaddr addr;
  socklen_t addrlen = sizeof(addr);

  int new_fd = accept(this->fd_, &addr, &addrlen);

  if (new_fd < 0) {
    status_ = absl::Status(absl::StatusCode::kInternal, "Accept failed");
    return {};
  }

  Socket new_socket(new_fd);
  return std::make_pair(std::move(new_socket), SocketSockAddr(addr));
}

void Socket::Bind(const SocketSockAddr& addr) {
  if (!status_.ok()) {
    return;
  }

  const struct sockaddr &addr_in = addr.rawValue();

  int result = bind(this->fd_, &addr_in, sizeof(addr_in));
  if (result < 0) {
    status_ = absl::Status(absl::StatusCode::kInternal, "Bind failed");
    return;
  }
}

void Socket::Connect(const SocketSockAddr& addr) {
  if (!status_.ok()) {
    return;
  }

  const struct sockaddr &addr_in = addr.rawValue();

  int result = connect(this->fd_, &addr_in, sizeof(addr_in));
  if (result < 0) {
      status_ = absl::Status(absl::StatusCode::kInternal, "Connect failed");
      return;
  }
}

void Socket::Listen(int backlog) {
  if (!status_.ok()) {
    return;
  }

  int result = listen(this->fd_, backlog);
  if (result < 0) {
    status_ = absl::Status(absl::StatusCode::kInternal, "Listen failed");
    return;
  }
}

template <typename T>
std::optional<std::pair<std::unique_ptr<T[]>, ssize_t>> Socket::Receive(
    size_t len, int flags) {
  if (!status_.ok()) {
    return {};
  }

  // we make do with what we have.
  if (len % sizeof(T) != 0) {
    status_ = absl::Status(absl::StatusCode::kInvalidArgument,
      "Socket receive requires len % sizeof(T) to be 0");
    return {};
  }

  std::unique_ptr<T[]> data = std::make_unique<T[]>(len / sizeof(T));

  ssize_t result = recv(fd_, reinterpret_cast<void*>(data.get()), len, flags);

  if (result < 0) {
    status_ = absl::Status(absl::StatusCode::kInternal, "Receive failed");
    return {{std::move(data), result}};
  }

  return {{std::move(data), result}};
}

template std::optional<std::pair<std::unique_ptr<char[]>, ssize_t>>
  Socket::Receive(size_t len, int flags);

template <typename T>
ssize_t Socket::Send(const T* data, size_t len, int flags) {
  if (!status_.ok()) {
    return -1;
  }

  ssize_t result = send(fd_, reinterpret_cast<const void*>(data), len, flags);
  if (result < 0) {
      status_ = absl::Status(absl::StatusCode::kInternal, "Send failed");
  }
  return result;
}

template ssize_t Socket::Send(const void* data, size_t len, int flags);
template ssize_t Socket::Send(const char* data, size_t len, int flags);

}  // namespace cppserver
