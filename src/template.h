// Copyright 2022 Daniel Liu

#ifndef _CPPSERVER_TEMPLATE_H_
#define _CPPSERVER_TEMPLATE_H_

#include <string>
#include <unordered_map>
#include <variant>

#include "absl/status/statusor.h"

namespace cppserver {

namespace templates {

#define TEMPLATE_TYPE std::variant<std::string, int, double>
#define TEMPLATE_OBJECT_TYPE \
  std::variant<std::string, int, double, \
  std::unordered_map<std::string, TEMPLATE_TYPE>>

absl::StatusOr<std::string> EvaluateExpression(
    const std::string& expression,
    const std::unordered_map<std::string, TEMPLATE_TYPE>& context);

absl::StatusOr<std::string> RenderTemplate(
    const std::string& template_str,
    const std::unordered_map<std::string, TEMPLATE_TYPE>& context);

}  // namespace templates

}  // namespace cppserver

#endif
