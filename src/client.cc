// Copyright 2022 Daniel Liu

#include <iostream>

#include "src/socket.h"

int main() {
  cppserver::Socket socket(
      cppserver::SocketDomain::kIPV4, cppserver::SocketType::kTCP);

  socket.Connect(cppserver::SocketSockAddr("127.0.0.1", 8000));
  if (!socket) {
    return 1;
  }

  std::cout << "Connected to port 8000" << std::endl;

  std::string message = "Hello, world!";
  auto sent = socket.Send(message.c_str(), message.size());

  if (!sent) {
    std::cout << "Send failed!" << std::endl;
    std::cout << "Error message: " << socket.Status().message() << std::endl;
    return 1;
  }

  return 0;
}
