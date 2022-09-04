// Copyright 2022 Daniel Liu

#ifndef _CPPSERVER_TEMPLATE_H_
#define _CPPSERVER_TEMPLATE_H_

#include <initializer_list>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>

#include "absl/status/statusor.h"

namespace cppserver {

namespace templates {

// Supported types as of now!
using TEMPLATE_TYPE = std::variant<std::string, int, double>;

template <typename K>
class GeneralTemplateMap {
  using V = std::variant<std::string, int, double, GeneralTemplateMap<std::string>, GeneralTemplateMap<int>>;
 public:
  GeneralTemplateMap() {}
  GeneralTemplateMap(std::initializer_list<V> init) {
    int index = 0;
    for (const auto& element : init) {
      mapping_[index] = element;
      index++;
    }
  }
  GeneralTemplateMap(std::initializer_list<std::pair<K, V>> init)  {
    for (const auto& [key, value] : init) {
      mapping_[key] = value;
    }
  }

  V& operator[](const K& key) {
    return mapping_[key];
  }

  bool ContainsKey(const K& key) const {
    return mapping_.find(key) != mapping_.end();
  }

  const V& at(const K& key) const {
    return mapping_.at(key);
  }
 private:
  std::unordered_map<K, V> mapping_;
};

using TemplateObject = GeneralTemplateMap<std::string>;
using TemplateList = GeneralTemplateMap<int>;
using CONTEXT_TYPE = std::variant<std::string, int, double, TemplateObject, TemplateList>;

absl::StatusOr<std::string> EvaluateExpression(
    const std::string& expression,
    const std::unordered_map<std::string, CONTEXT_TYPE>& context);

absl::StatusOr<std::string> RenderTemplate(
    const std::string& template_str,
    const std::unordered_map<std::string, CONTEXT_TYPE>& context);

}  // namespace templates

}  // namespace cppserver

#endif
