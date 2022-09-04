
#include "absl/status/statusor.h"
#include "gtest/gtest.h"

#include "template.h"

using namespace cppserver::templates;

TEST(TemplateTests, EmptyTemplate) {
  std::string template_str = "";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, TextTemplate) {
  std::string template_str = "Hello, world!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, HTMLTemplate) {
  std::string template_str = "<html><body><h1>Hello, world!</h1></body></html>";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "<html><body><h1>Hello, world!</h1></body></html>";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringExpressionTemplate) {
  std::string template_str = "Hello, {{name}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"name", "world"}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerExpressionTemplate) {
  std::string template_str = "Hello, {{name}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"name", 123}
  };
  std::string expected = "Hello, 123!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleExpressionTemplate) {
  std::string template_str = "Hello, {{name}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"name", 123.456}
  };
  std::string expected = "Hello, 123.456000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringLiteralTemplate) {
  std::string template_str = "Hello, {{\"world\"}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerLiteralTemplate) {
  std::string template_str = "Hello, {{123}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, 123!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleLiteralTemplate) {
  std::string template_str = "Hello, {{123.456}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, 123.456000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringConcatenationTemplate) {
  std::string template_str = "Hello, {{\"world\" + \"!\"}}";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, StringConcatenationFromContextTemplate) {
  std::string template_str = "Hello, {{name + \"!\"}}";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"name", "world"}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerAdditionTemplate) {
  std::string template_str = "Hello, {{123 + 456}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, 579!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerAdditionFromContextTemplate) {
  std::string template_str = "Hello, {{123 + num}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"num", 456}
  };
  std::string expected = "Hello, 579!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleAdditionTemplate) {
  std::string template_str = "Hello, {{123.456 + 789.012}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, 912.468000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, DoubleAdditionFromContextTemplate) {
  std::string template_str = "Hello, {{123.456 + num}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"num", 789.012}
  };
  std::string expected = "Hello, 912.468000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, IntegerDoubleAdditionTemplate) {
  std::string template_str = "Hello, {{123 + 456.789}}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {};
  std::string expected = "Hello, 579.789000!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, NestedObjectTemplate) {
  std::string template_str = "Hello, {{name.first}}{{name.last}}";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"name", TemplateObject{
      {"first", "world"},
      {"last", "!"},
    }}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, TakenIfTemplate) {
  std::string template_str = "Hello{% if a == 1 %}, world{% endif %}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"a", 1}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, NotTakenIfTemplate) {
  std::string template_str = "Hello{% if a == 1 %}, world{% endif %}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"a", 2}
  };
  std::string expected = "Hello!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, TakenIfElseTemplate) {
  std::string template_str = "Hello, {% if a == 1 %}world{% else %}there{% endif %}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"a", 1}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, NotTakenIfElseTemplate) {
  std::string template_str = "Hello, {% if a == 1 %}world{% else %}there{% endif %}!";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"a", 2}
  };
  std::string expected = "Hello, there!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, NestedIfElseTemplate) {
  std::string template_str = 
      "{% if a == 1 %}{% if b == 1 %}a{% else %}b{% endif %}{% else %}{% if b == 1 %}c{% else %}d{% endif %}{% endif %}";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"a", 1},
    {"b", 1}
  };
  std::string expected = "a";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());

  context["b"] = 2;
  expected = "b";
  actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());

  context["a"] = 2;
  context["b"] = 1;
  expected = "c";
  actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());

  context["b"] = 2;
  expected = "d";
  actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, ForLoopTemplate) {
  std::string template_str = "Hello, {% for name in names %}{{name}}{% endfor %}";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"names", TemplateList{
      "world",
      "!",
    }}
  };
  std::string expected = "Hello, world!";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}

TEST(TemplateTests, RepeatedIfTemplate) {
  std::string template_str = 
      "Hello, {% for person in people %}{% if person.shown %}{{person.name}} {% endif %}{% endfor %}";
  std::unordered_map<std::string, TEMPLATE_OBJECT_ANY> context = {
    {"people", TemplateList{
      TemplateObject{
        {"shown", true},
        {"name", "A"}
      },
      TemplateObject{
        {"shown", false},
        {"name", "B"}
      },
      TemplateObject{
        {"shown", false},
        {"name", "C"}
      },
      TemplateObject{
        {"shown", true},
        {"name", "D"}
      }
    }}
  };
  std::string expected = "Hello, A D ";
  auto actual = RenderTemplate(template_str, context);
  EXPECT_TRUE(actual.ok());
  EXPECT_EQ(expected, actual.value());
}
