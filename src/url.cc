// Copyright 2022 Daniel Liu

#include "url.h"

#include <iterator>
#include <regex>
#include <string>

#include "absl/strings/str_format.h"

namespace cppserver {

std::string QuoteUrl(const std::string& str_in) {
  static const std::string UNRESERVED_CHARACTERS = 
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~";

  std::string str_out;
  for (char c : str_in) {
    if (UNRESERVED_CHARACTERS.find(c) != std::string::npos) {
      str_out += c;
    } else {
      str_out += "%" + absl::StrFormat("%02X", c);
    }
  }
  return str_out;
}

std::string UnquoteUrl(const std::string& str_in) {
  std::string str_out;
  auto it = str_in.begin();
  auto end = str_in.end();
  while (it != end) {
    if (*it == '%') {
      if (std::distance(it, end) < 3) {
        return {};
      }
      std::string hex_str = std::string(it + 1, it + 3);
      int hex_val = std::stoi(hex_str, nullptr, 16);
      str_out += static_cast<char>(hex_val);
      it += 3;
    } else {
      str_out += *it;
      it++;
    }
  }
  return str_out;
}

}
