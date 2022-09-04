// Copyright 2022 Daniel Liu

#include "template.h"

#include <memory>
#include <regex>
#include <stack>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_split.h"

namespace cppserver {

namespace templates {

namespace ast {

using TEMPLATE_TYPE = templates::TEMPLATE_TYPE;
using CONTEXT_TYPE = templates::CONTEXT_TYPE;

absl::StatusOr<TEMPLATE_TYPE> EvaluateBinaryOperator(
    TEMPLATE_TYPE left, TEMPLATE_TYPE right, const std::string& op) {
  if (op == "+") {
    // string + string = concatenation
    // int + int = int
    // int + double or double + int = double
    // double + double = double
    // anything else = error
    if (std::holds_alternative<std::string>(left) &&
        std::holds_alternative<std::string>(right)) {
      return std::get<std::string>(left) + std::get<std::string>(right);
    } else if (std::holds_alternative<int>(left) &&
                std::holds_alternative<int>(right)) {
      return std::get<int>(left) + std::get<int>(right);
    } else if (std::holds_alternative<int>(left) &&
                std::holds_alternative<double>(right)) {
      return std::get<int>(left) + std::get<double>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<int>(right)) {
      return std::get<double>(left) + std::get<int>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<double>(right)) {
      return std::get<double>(left) + std::get<double>(right);
    } else {
      return absl::InvalidArgumentError("Invalid types for binary operator");
    }
  } else if (op == "-") {
    // int - int = int
    // int - double or double - int = double
    // double - double = double
    // anything else = error
    if (std::holds_alternative<int>(left) &&
        std::holds_alternative<int>(right)) {
      return std::get<int>(left) - std::get<int>(right);
    } else if (std::holds_alternative<int>(left) &&
                std::holds_alternative<double>(right)) {
      return std::get<int>(left) - std::get<double>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<int>(right)) {
      return std::get<double>(left) - std::get<int>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<double>(right)) {
      return std::get<double>(left) - std::get<double>(right);
    } else {
      return absl::InvalidArgumentError("Invalid types for binary operator");
    }
  } else if (op == "*") {
    // same rules as -
    if (std::holds_alternative<int>(left) &&
        std::holds_alternative<int>(right)) {
      return std::get<int>(left) * std::get<int>(right);
    } else if (std::holds_alternative<int>(left) &&
                std::holds_alternative<double>(right)) {
      return std::get<int>(left) * std::get<double>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<int>(right)) {
      return std::get<double>(left) * std::get<int>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<double>(right)) {
      return std::get<double>(left) * std::get<double>(right);
    } else {
      return absl::InvalidArgumentError("Invalid types for binary operator");
    }
  } else if (op == "/") {
    // same rules as -, but return absl::InvalidArgumentError for divide by 0
    if (std::holds_alternative<int>(left) &&
        std::holds_alternative<int>(right)) {
      if (std::get<int>(right) == 0) {
        return absl::InvalidArgumentError("Division by 0");
      }
      return std::get<int>(left) / std::get<int>(right);
    } else if (std::holds_alternative<int>(left) &&
                std::holds_alternative<double>(right)) {
      if (std::get<double>(right) == 0) {
        return absl::InvalidArgumentError("Division by 0");
      }
      return std::get<int>(left) / std::get<double>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<int>(right)) {
      if (std::get<int>(right) == 0) {
        return absl::InvalidArgumentError("Division by 0");
      }
      return std::get<double>(left) / std::get<int>(right);
    } else if (std::holds_alternative<double>(left) &&
                std::holds_alternative<double>(right)) {
      if (std::get<double>(right) == 0) {
        return absl::InvalidArgumentError("Division by 0");
      }
      return std::get<double>(left) / std::get<double>(right);
    } else {
      return absl::InvalidArgumentError("Invalid types for binary operator");
    }
  } else {
    return absl::InvalidArgumentError("Invalid binary operator");
  }
}

class TemplateASTNode {
 public:
  TemplateASTNode() = default;

  virtual ~TemplateASTNode() = default;

  void AddChild(std::unique_ptr<TemplateASTNode> child) {
    children_.push_back(std::move(child));
  }

  virtual absl::StatusOr<TEMPLATE_TYPE> Evaluate(
      const std::unordered_map<std::string, CONTEXT_TYPE>& context) const = 0;

 protected:
  std::vector<std::unique_ptr<TemplateASTNode>> children_;
};

class TemplateASTConstantNode : public TemplateASTNode {
 public:
  TemplateASTConstantNode(TEMPLATE_TYPE value) : value_(value) {}

  absl::StatusOr<TEMPLATE_TYPE> Evaluate(
      [[maybe_unused]] const std::unordered_map<std::string, CONTEXT_TYPE>& 
          context) const {
    return value_;
  }

 private:
  TEMPLATE_TYPE value_;
};

class TemplateASTVariableNode : public TemplateASTNode {
 public:
  TemplateASTVariableNode(const std::string& variable_name)
      : name_(variable_name) {}

  absl::StatusOr<TEMPLATE_TYPE> Evaluate(
      const std::unordered_map<std::string, CONTEXT_TYPE>& context) const {
    std::vector<std::string> components = absl::StrSplit(name_, '.');
    if (size(components) == 0) {
      return absl::InvalidArgumentError(
          "Invalid variable name; variable name cannot be empty.");
    }
    
    if (context.find(components[0]) == context.end()) {
      return absl::InvalidArgumentError("Variable not found in context.");
    }
    const CONTEXT_TYPE* indexing = &context.at(components[0]);

    for (int i = 1; i < size(components); ++i) {
      if (std::holds_alternative<TemplateObject>(*indexing)) {
        auto o = std::reference_wrapper(std::get<TemplateObject>(*indexing));
        if (!o.get().ContainsKey(components[i])) {
          return absl::InvalidArgumentError("Variable not found in context.");
        }
        indexing = &o.get().at(components[i]);
      } else {
        return absl::InvalidArgumentError(
            "Invalid variable access; attempting to lookup key in a non-object.");
      }
    }

    // Type check on `indexing`.
    if (std::holds_alternative<std::string>(*indexing)) {
      return std::get<std::string>(*indexing);
    } else if (std::holds_alternative<int>(*indexing)) {
      return std::get<int>(*indexing);
    } else if (std::holds_alternative<double>(*indexing)) {
      return std::get<double>(*indexing);
    } else {
      return absl::InvalidArgumentError(
          "Invalid variable access; resulting value is not a TEMPLATE_TYPE");
    }
  }

 private:
  std::string name_;
};

class TemplateASTBinaryOperatorNode : public TemplateASTNode {
 public:
  TemplateASTBinaryOperatorNode(const std::string& operator_in)
      : operator_(operator_in) {}

  absl::StatusOr<TEMPLATE_TYPE> Evaluate(
      const std::unordered_map<std::string, CONTEXT_TYPE>& context) const {
    if (children_.size() != 2) {
      return absl::InvalidArgumentError(
          "Binary operator must have 2 children.");
    }

    auto left = children_[0]->Evaluate(context);
    if (!left.ok()) {
      return left.status();
    }
    auto right = children_[1]->Evaluate(context);
    if (!right.ok()) {
      return right.status();
    }

    return EvaluateBinaryOperator(left.value(), right.value(), operator_);
  }
 private:
  std::string operator_;
};

// ----------------------------------------------------------------

// Parsing functionality for strings into TemplateASTs.

std::vector<std::string> TokenizeString(const std::string& expression) {
  std::vector<std::string> tokens;
  std::string token;

  auto it = begin(expression);
  auto end_it = end(expression);
  while (it != end_it) {
    char c = *it;
    std::regex string_literal {"(\".*?\").*"};
    std::regex number_literal {"(-?[0-9]+(\\.[0-9]+)?).*"};

    std::smatch match;
    if (std::regex_match(it, end_it, match, string_literal)) {
      tokens.push_back(match[1]);
      it += match[1].length();
    } else if (std::regex_match(it, end_it, match, number_literal)) {
      tokens.push_back(match[1]);
      it += match[1].length();
    } else {
      if (c == ' ') {
        if (!token.empty()) {
          tokens.push_back(token);
          token.clear();
        }
      } else if (c == '(' || c == ')') {
        if (!token.empty()) {
          tokens.push_back(token);
          token.clear();
        }
        tokens.push_back(std::string(1, c));
      } else if (c == '+' || c == '-' || c == '*' || c == '/') {
        if (!token.empty()) {
          tokens.push_back(token);
          token.clear();
        }
        tokens.push_back(std::string(1, c));
      } else {
        token += c;
      }
      ++it;
    } 
  }

  if (!token.empty()) {
    tokens.push_back(token);
  }

  return tokens;
}

absl::StatusOr<std::unique_ptr<TemplateASTNode>> ParseString(
    const std::string& expression) {
  auto tokens = TokenizeString(expression);
  if (tokens.empty()) {
    return absl::InvalidArgumentError("Empty expression.");
  }

  // Shunting yard algorithm.
  std::vector<std::string> rpn;
  std::stack<std::string> operators;

  for (auto token : tokens) {
    if (token == "(") {
      operators.push(token);
    } else if (token == ")") {
      while (!operators.empty() && operators.top() != "(") {
        rpn.push_back(operators.top());
        operators.pop();
      }
      if (operators.empty()) {
        return absl::InvalidArgumentError("Mismatched parentheses.");
      }
      operators.pop();
    } else if (token == "+" || token == "-") {
      while (!operators.empty() && operators.top() != "(") {
        rpn.push_back(operators.top());
        operators.pop();
      }
      operators.push(token);
    } else if (token == "*" || token == "/") {
      while (!operators.empty() && 
             (operators.top() == "*" || operators.top() == "/")) {
        rpn.push_back(operators.top());
        operators.pop();
      }
      operators.push(token);
    } else {
      rpn.push_back(token);
    }
  }

  while (!operators.empty()) {
    if (operators.top() == "(" || operators.top() == ")") {
      return absl::InvalidArgumentError("Mismatched parentheses.");
    }
    rpn.push_back(operators.top());
    operators.pop();
  }

  // Generate AST from RPN
  std::stack<std::unique_ptr<TemplateASTNode>> ast_stack;

  for (auto token : rpn) {
    if (token == "+" || token == "-" || token == "*" || token == "/") {
      if (ast_stack.size() < 2) {
        return absl::InvalidArgumentError(
            "Invalid expression; not enough operands to binary operation.");
      }
      auto right = std::move(ast_stack.top());
      ast_stack.pop();
      auto left = std::move(ast_stack.top());
      ast_stack.pop();
      auto node = std::make_unique<TemplateASTBinaryOperatorNode>(token);
      node->AddChild(std::move(left));
      node->AddChild(std::move(right));
      ast_stack.push(std::move(node));
    } else if (token[0] == '"') {
      ast_stack.push(std::make_unique<TemplateASTConstantNode>(
          token.substr(1, token.size() - 2)));
    } else if (token[0] == '-' || (token[0] >= '0' && token[0] <= '9')) {
      if (token.find('.') != std::string::npos) {
        ast_stack.push(std::make_unique<TemplateASTConstantNode>(
            std::stod(token)));
      } else {
        ast_stack.push(std::make_unique<TemplateASTConstantNode>(
            std::stoi(token)));
      }
    } else {
      auto node = std::make_unique<TemplateASTVariableNode>(token);
      ast_stack.push(std::move(node));
    }
  }

  if (ast_stack.size() != 1) {
    return absl::InvalidArgumentError(
        "Invalid expression; could not resolve RPN to single root.");
  }
  return std::move(ast_stack.top());
}

}  // namespace ast

absl::StatusOr<std::string> EvaluateExpression(
    const std::string& expression,
    const std::unordered_map<std::string, CONTEXT_TYPE>& context) {
  auto tree = ast::ParseString(expression);
  if (!tree.ok()) {
    return tree.status();
  }
  auto evaluated = tree.value()->Evaluate(context);
  if (!evaluated.ok()) {
    return evaluated.status();
  }
  TEMPLATE_TYPE result = evaluated.value();
  if (std::holds_alternative<std::string>(result)) {
    return std::get<std::string>(result);
  } else if (std::holds_alternative<int>(result)) {
    return std::to_string(std::get<int>(result));
  } else if (std::holds_alternative<double>(result)) {
    return std::to_string(std::get<double>(result));
  } else {
    return absl::InvalidArgumentError(
            "Invalid expression; expression evaluated to unknown type.");
  }
}

struct TemplateInterpolationExpression {
  std::string expression;
};

struct TemplateControlFlowExpression {
  std::string command;
  std::string expression;
};

using TemplateSegment = std::variant<std::string,
                                     TemplateInterpolationExpression,
                                     TemplateControlFlowExpression>;
                                    
std::vector<TemplateSegment> TokenizeTemplate(const std::string& template_str) {
  std::vector<TemplateSegment> segments;

  // Regexes
  std::regex interpolation_regex {"(\\{\\{(.*?)\\}\\}).*"};
  std::regex control_flow_regex {"(\\{\\%\\s*(\\w+)\\s+(.*?)\\s*\\%\\}).*"};

  auto it = begin(template_str);
  auto end_it = end(template_str);

  std::string plain_text = "";

  while (it != end_it) {
    std::smatch match;
    if (std::regex_search(it, end_it, match, interpolation_regex)
        && match.prefix().length() == 0) {
      if (!plain_text.empty()) {
        segments.push_back(plain_text);
        plain_text.clear();
      }
      segments.push_back(TemplateInterpolationExpression{match[2]});
      it += match[1].length();
    } else if (std::regex_search(it, end_it, match, control_flow_regex)
               && match.prefix().length() == 0) {
      if (!plain_text.empty()) {
        segments.push_back(plain_text);
        plain_text.clear();
      }
      segments.push_back(TemplateControlFlowExpression{match[2], match[3]});
      it += match[1].length();
    } else {
      plain_text += *it;
      ++it;
    }
  }

  if (!plain_text.empty()) {
    segments.push_back(plain_text);
  }

  return segments;
}

// absl::StatusOr<std::string> RenderTemplateHelper(
//     const std::string& template_str,
//     const std::unordered_map<std::string, CONTEXT_TYPE>& context,
//     std::optional<TemplateControlFlowExpression> control_flow_context) {

// }

absl::StatusOr<std::string> RenderTemplate(
    const std::string& template_str,
    const std::unordered_map<std::string, CONTEXT_TYPE>& context) {
  auto tokenized = TokenizeTemplate(template_str);
  std::string result = "";
  for (auto segment : tokenized) {
    if (std::holds_alternative<std::string>(segment)) {
      result += std::get<std::string>(segment);
    } else {
      auto expression = std::get<TemplateInterpolationExpression>(segment);
      auto evaluated = EvaluateExpression(expression.expression, context);
      if (!evaluated.ok()) {
        return evaluated.status();
      }
      result += evaluated.value();
    }
  }
  return result;
}

}  // namespace templates

}  // namespace cppserver
