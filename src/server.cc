// Copyright 2022 Daniel Liu

#include "src/server.h"

#include <sys/types.h>

#include <optional>
#include <queue>
#include <thread>
#include <utility>

#include "absl/log/log.h"
#include "absl/synchronization/mutex.h"

#include "http.h"
#include "socket.h"

namespace cppserver {

const size_t kMAX_CONCURRENCY = 8;

Server::Server(in_port_t port_in) 
    : port_{port_in}, 
      socket_ {cppserver::SocketDomain::kIPV4,
          cppserver::SocketType::kTCP} {
  listening_thread_ = std::thread(&Server::ListenForConnections, this);
}

void Server::AddEndpointHandler(
    std::string endpoint, 
    std::function<std::string(
        HTTPRequest, std::unordered_map<std::string, std::string>)> handler) {
  endpoint_handlers_mutex_.WriterLock();
  std::vector<std::string> components;
  // Get each component (within <>).
  std::regex component_regex("<(.*)>");
  std::sregex_iterator component_it(endpoint.begin(), endpoint.end(), 
      component_regex);
  std::sregex_iterator component_end;
  while (component_it != component_end) {
    std::smatch match = *component_it;
    components.push_back(match[1]);
    ++component_it;
  }
  // Preprocess the string, replacing `<.*>` with `(.*)`.
  std::regex endpoint_regex {std::regex_replace(endpoint, 
      std::regex("<.*>"), "(.*)")};
  
  LOG(INFO) << "Adding endpoint handler for endpoint " << endpoint;

  endpoint_handlers_.push_back({
      std::regex(endpoint_regex), components, handler});
  endpoint_handlers_mutex_.WriterUnlock();
}

Server::~Server() {
  listening_thread_.join();
}

void Server::ListenForConnections() {
  socket_.Bind(cppserver::SocketSockAddr("127.0.0.1", port_));
  socket_.Listen(kMAX_CONCURRENCY);

  if (!socket_) {
    LOG(ERROR) << "Failed to bind/listen";
    LOG(ERROR) << "errno = " << errno;
    LOG(FATAL) << "Failed to set up server!";
    return;
  }

  LOG(INFO) << "Server listening on port " << port_;

  std::queue<std::thread> threads;

  while (true) {
    while (size(threads) > kMAX_CONCURRENCY) {
      threads.front().join();
      threads.pop();
    }

    auto accept = socket_.Accept();
    if (!socket_) {
      LOG(ERROR) << "Failed to accept connection!";
      continue;
    }

    auto client = std::move(accept.value().first);
    std::thread client_thread {&Server::HandleMessage, this, 
        std::move(client)};
    threads.emplace(std::move(client_thread));
  }
}

void Server::HandleMessage(Socket client) {
  std::string msg = "";
  bool receiving = true;
  std::optional<int> remaining_bytes = {};

  do {
    int bytes_to_receive = 256;
    if (remaining_bytes) {
      bytes_to_receive = remaining_bytes.value();
    }

    auto recvd = client.Receive<char>(bytes_to_receive);

    if (!recvd) {
      LOG(ERROR) << "Failed to receive message!";
      LOG(ERROR) << "Error message: " << client.Status().message();
      LOG(ERROR) << errno;
      continue;
    }

    if (recvd->second > 0) {
      msg += std::string(recvd->first.get(), recvd->second);

      if (remaining_bytes) {
        LOG(INFO) << "Need to read in " << *remaining_bytes << " bytes";
        remaining_bytes = remaining_bytes.value() - recvd->second;
      } else {
        auto remaining = cppserver::DetermineRemainingHTTPContentLength(msg);
        if (remaining) {
          remaining_bytes = remaining.value();
          LOG(INFO) << "Can calculate end of message... " << *remaining_bytes 
              << " bytes remaining";
        }
      }
    } else {
      receiving = false;
    }      
  } while (receiving && (!remaining_bytes || remaining_bytes.value() > 0));

  auto parsed = ParseHTTPRequest(msg);
  if (!parsed) {
    LOG(ERROR) << "Failed to parse HTTP request!" << std::endl;
    return;
  }
  HTTPRequest request = parsed.value();

  LOG(INFO) << request.method << " " << request.target;
  LOG(INFO) << "Found " << size(request.headers) << " headers";
  LOG(INFO) << "Found body = " << request.body << ";";

  // Look through endpoints, find the first one that matches the request.

  endpoint_handlers_mutex_.ReaderLock();
  for (auto [path, component_names, handler] : endpoint_handlers_) {
    std::smatch match;
    if (std::regex_match(request.target, match, path)) {
      endpoint_handlers_mutex_.ReaderUnlock();
      LOG(INFO) << "Matched endpoint " << request.target;

      std::unordered_map<std::string, std::string> url_components;
      for (size_t i = 0; i < size(component_names); ++i) {
        LOG(INFO) << component_names[i] << " = " << match[i + 1];
        url_components[component_names[i]] = match[i + 1];
      }

      std::string response = handler(request, std::move(url_components));
      auto sent = client.Send(response.c_str(), response.size());
      LOG(INFO) << "Sent " << sent << " response bytes";
      return;
    }
  }
  endpoint_handlers_mutex_.ReaderUnlock();

  LOG(INFO) << "Could not match endpoint " << request.target;

  HTTPResponse response(404);

  response.AddHeader("Content-Type", "text/html; charset=utf-8");
  response.SetBody("<h1>404 Page Not Found</h1>");

  std::string response_str = response.ToString();

  auto sent = client.Send(response_str.c_str(), response_str.size());
  LOG(INFO) << "Sent " << sent << " response bytes";
}

}  // namespace cppserver
