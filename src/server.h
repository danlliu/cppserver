// Copyright 2022 Daniel Liu

#ifndef _CPPSERVER_SERVER_H_
#define _CPPSERVER_SERVER_H_

#include <sys/types.h>

#include <functional>
#include <optional>
#include <regex>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "absl/synchronization/mutex.h"

#include "http.h"
#include "url.h"
#include "socket.h"

namespace cppserver {

class Server {
 public:
  // Starts the server running on the specified port.
  Server(in_port_t port_in);

  // Wait for the listening thread to terminate.
  ~Server();

  // Add an endpoint handler.
  // Endpoints are matched in the order they are added.
  // The first endpoint that matches the request will be used.
  // To add parameters into endpoints, use the syntax `<param_name>`.
  //
  // Examples:
  // 
  //    server.AddEndpointHandler("/", IndexHandler);
  //    server.AddEndpointHandler("/posts/<post_id>/", PostHandler);
  void AddEndpointHandler(
      std::string endpoint, 
      std::function<std::string(
          HTTPRequest, std::unordered_map<std::string, std::string>)> handler);

 private:
  void ListenForConnections();

  void HandleMessage(Socket socket);

  // Server port.
  in_port_t port_;

  // Server socket.
  Socket socket_;

  // Listening thread.
  std::thread listening_thread_;

  // Endpoint handlers.
  std::vector<std::tuple<std::regex, std::vector<std::string>, 
      std::function<std::string(HTTPRequest, 
                                std::unordered_map<std::string, std::string>)>>>
      endpoint_handlers_;

  // Mutex for endpoint handlers.
  absl::Mutex endpoint_handlers_mutex_;
};

}  // namespace cppserver

#endif
