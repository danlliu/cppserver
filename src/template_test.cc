
#include "absl/status/statusor.h"
#include "gtest/gtest.h"

#include "template.h"

using namespace cppserver::templates;

TEST(TemplateTests, EmptyTemplate) {
  std::string template_str = "";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, TextTemplate) {
  std::string template_str = "Hello, world!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, HTMLTemplate) {
  std::string template_str = "<html><body><h1>Hello, world!</h1></body></html>";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "<html><body><h1>Hello, world!</h1></body></html>";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringExpressionTemplate) {
  std::string template_str = "Hello, {{name}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {
    {"name", "world"}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerExpressionTemplate) {
  std::string template_str = "Hello, {{name}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {
    {"name", 123}
  };
  std::string expected = "Hello, 123!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleExpressionTemplate) {
  std::string template_str = "Hello, {{name}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {
    {"name", 123.456}
  };
  std::string expected = "Hello, 123.456000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringLiteralTemplate) {
  std::string template_str = "Hello, {{\"world\"}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerLiteralTemplate) {
  std::string template_str = "Hello, {{123}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, 123!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleLiteralTemplate) {
  std::string template_str = "Hello, {{123.456}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, 123.456000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringConcatenationTemplate) {
  std::string template_str = "Hello, {{\"world\" + \"!\"}}";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringConcatenationFromContextTemplate) {
  std::string template_str = "Hello, {{name + \"!\"}}";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {
    {"name", "world"}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerAdditionTemplate) {
  std::string template_str = "Hello, {{123 + 456}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, 579!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerAdditionFromContextTemplate) {
  std::string template_str = "Hello, {{123 + num}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {
    {"num", 456}
  };
  std::string expected = "Hello, 579!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleAdditionTemplate) {
  std::string template_str = "Hello, {{123.456 + 789.012}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, 912.468000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleAdditionFromContextTemplate) {
  std::string template_str = "Hello, {{123.456 + num}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {
    {"num", 789.012}
  };
  std::string expected = "Hello, 912.468000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerDoubleAdditionTemplate) {
  std::string template_str = "Hello, {{123 + 456.789}}!";
  std::unordered_map<std::string, TEMPLATE_TYPE> context = {};
  std::string expected = "Hello, 579.789000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}
