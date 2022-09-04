// Copyright 2022 Daniel Liu

#include <signal.h>

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "absl/log/initialize.h"
#include "absl/log/log.h"
#include "absl/log/log_sink.h"
#include "absl/log/log_sink_registry.h"

#include "http.h"
#include "template.h"
#include "server.h"

std::string IndexHandler(
    cppserver::HTTPRequest request, 
    [[maybe_unused]] std::unordered_map<std::string, std::string> params) {
  cppserver::HTTPResponse response(200);
  response.AddHeader("Content-Type", "text/html; encoding=utf-8");
  response.LoadBodyFromFile("static/index.html");
  return response.ToString();
}

std::string PathHandler(
    cppserver::HTTPRequest request, 
    std::unordered_map<std::string, std::string> params) {
  cppserver::HTTPResponse response(200);
  response.AddHeader("Content-Type", "text/html; encoding=utf-8");
  response.LoadBodyFromFile("templates/path.html");
  LOG(INFO) << "path = " << params["path"];
  response.RenderTemplateFile(
      "templates/path.html", 
      {{"path", params["path"]}});
  return response.ToString();
}

int main() {
  // Set up logging.
  struct LogSink : public absl::LogSink {
    virtual void Send(const absl::LogEntry& entry) override {
      std::cout << "[" << entry.log_severity() << "] ";
      std::cout << entry.text_message_with_newline();
    }
  };

  auto sink = std::make_unique<LogSink>();
  absl::AddLogSink(sink.get());

  absl::InitializeLog();

  cppserver::Server server(8000);
  server.AddEndpointHandler("/", IndexHandler);
  server.AddEndpointHandler("/path/<path>/", PathHandler);

  return 0;
}

