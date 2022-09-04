// Copyright 2022 Daniel Liu

#include "http.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "absl/log/log.h"
#include "absl/strings/str_split.h"

#include "template.h"

namespace cppserver {

std::optional<int> DetermineRemainingHTTPContentLength(const std::string& msg) {
  std::vector<std::string> lines = absl::StrSplit(msg, "\n");
  for (int i = 0; i < size(lines); ++i) {
    auto line = lines[i];

    if (line == "\r") {
      // No content!
      return 0;
    }

    if (line.find("Content-Length") != std::string::npos) {
      std::vector<std::string> parts = absl::StrSplit(line, ":");
      if (parts.size() != 2) {
        return {};
      }

      std::string content_length_str = parts[1];
      content_length_str.erase(0, 1);
      int total_length = std::stoi(content_length_str);

      // Find length of content
      int content_length = 0;

      // Sum up remaining lines (from first empty line to end)
      bool past_headers = false;
      for (int j = i + 1; j < size(lines); ++j) {
        if (!past_headers) {
          if (lines[j] == "\r") {
            past_headers = true;
          }
          continue;
        }
        content_length += size(lines[j]);
      }

      return total_length - content_length;
    }
  }
  return {};
}

std::optional<HTTPRequest> ParseHTTPRequest(const std::string& msg) {
  std::vector<std::string> lines = absl::StrSplit(msg, "\n");

  if (size(lines) < 1) {
    return {};
  }

  std::vector<std::string> request_line = absl::StrSplit(lines[0], " ");
  if (size(request_line) != 3) {
    return {};
  }

  HTTPRequest request;
  request.method = request_line[0];
  request.target = request_line[1];
  request.version = request_line[2];

  // Parse headers
  bool past_headers = false;
  for (int i = 1; i < size(lines); ++i) {
    auto line = lines[i];

    if (line == "\r") {
      past_headers = true;
      continue;
    }

    if (!past_headers) {
      std::vector<std::string> parts = absl::StrSplit(line, ": ");
      if (parts.size() != 2) {
        return {};
      }

      request.headers.emplace_back(parts[0], parts[1]);
    } else {
      request.body += line;
      if (i != size(lines) - 1) {
        request.body += "\n";
      }
    }
  }

  return request;
}

HTTPResponse::HTTPResponse(int status_code)
    : version_{"HTTP/1.1"},
      status_code_{status_code} {
  switch (status_code) {
    case 200:
      status_message_ = "OK";
      break;
    case 201:
      status_message_ = "Created";
      break;
    case 202:
      status_message_ = "Accepted";
      break;
    case 204:
      status_message_ = "No Content";
      break;
    case 400:
      status_message_ = "Bad Request";
      break;
    case 401:
      status_message_ = "Unauthorized";
      break;
    case 403:
      status_message_ = "Forbidden";
      break;
    case 404:
      status_message_ = "Not Found";
      break;
    case 418:
      status_message_ = "I'm a teapot";
      break;
    case 451:
      status_message_ = "Unavailable For Legal Reasons";
      break;
    case 500:
      status_message_ = "Internal Server Error";
      break;
    default:
      status_message_ = "Unknown";
      break;
  }
}

void HTTPResponse::LoadBodyFromFile(const std::string& path) {
  std::filesystem::path file_path = HTTPResponse::kserver_path_ + path;
  LOG(INFO) << "Loading file " << file_path;

  if (!std::filesystem::exists(file_path)) {
    LOG(WARNING) << "File not found!";
    status_code_ = 404;
    status_message_ = "Not Found";
  }

  std::ifstream file(file_path);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  body_ = content;
}

void HTTPResponse::RenderTemplateFile(
    const std::string& path, 
    const std::unordered_map<std::string, templates::TEMPLATE_OBJECT_ANY>& context) {
  std::filesystem::path file_path = HTTPResponse::kserver_path_ + path;
  LOG(INFO) << "Loading template file " << file_path;

  if (!std::filesystem::exists(file_path)) {
    LOG(WARNING) << "File not found!";
    status_code_ = 404;
    status_message_ = "Not Found";
  }

  std::ifstream file(file_path);
  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  auto rendered = templates::RenderTemplate(content, context);
  if (!rendered.ok()) {
    LOG(WARNING) << "Failed to render template!";
    status_code_ = 500;
    status_message_ = "Internal Server Error";
  }
  body_ = rendered.value();
}

std::string HTTPResponse::ToString() const {
  std::string status_line = version_ + " " + std::to_string(status_code_) + " " 
    + status_message_ + "\r\n";
  std::string headers = "";
  for (auto header : headers_) {
    headers += header.first + ": " + header.second + "\r\n";
  }
  headers += "Content-Length: " + std::to_string(size(body_)) + "\r\n";
  headers += "\r\n";
  return status_line + headers + body_;
}

const std::string HTTPResponse::kserver_path_ = "src/server/";

}  // namespace cppserver
