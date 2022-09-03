// Copyright 2022 Daniel Liu

#ifndef _CPPSERVER_HTTP_PARSER_H_
#define _CPPSERVER_HTTP_PARSER_H_

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace cppserver {

std::optional<int> DetermineRemainingHTTPContentLength(const std::string& msg);

struct HTTPRequest {
  std::string method;
  std::string target;
  std::string version;

  std::vector<std::pair<std::string, std::string>> headers;

  std::string body;
};

std::optional<HTTPRequest> ParseHTTPRequest(const std::string& msg);

class HTTPResponse {
 public:
  HTTPResponse(int status_code);

  // Adds the given header to the HTTPResponse object.
  // Don't use this method for Content-Length; it will be automatically added.
  void AddHeader(const std::string& key, const std::string& value) {
    headers_.push_back({key, value});
  }

  void SetBody(const std::string& body) {
    body_ = body;
  }

  std::string ToString() const;

 private:
  std::string version_;
  int status_code_;
  std::string status_message_;

  // Don't add Content-Length header to this, it is automatically added.
  std::vector<std::pair<std::string, std::string>> headers_;

  std::string body_;
};

}  // namespace cppserver

#endif
