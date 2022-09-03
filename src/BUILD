
cc_library(
  name = "http",
  srcs = ["http.cc"],
  hdrs = ["http.h"],
  deps = [
    "@com_google_absl//absl/strings",
  ]
)

cc_library(
  name = "socket",
  srcs = ["socket.cc"],
  hdrs = ["socket.h"],
  deps = [
    "@com_google_absl//absl/status:status",
    "@com_google_absl//absl/status:statusor",
  ],
)

cc_library(
  name = "server",
  srcs = ["server.cc"],
  hdrs = ["server.h"],
  deps = [
    "@com_google_absl//absl/log",
    "@com_google_absl//absl/status:status",
    "@com_google_absl//absl/synchronization",
    ":http",
    ":socket",
  ],
)

cc_binary(
  name = "main",
  srcs = ["main.cc"],
  deps = [
    "@com_google_absl//absl/log:initialize",
    ":server",
  ],
)
