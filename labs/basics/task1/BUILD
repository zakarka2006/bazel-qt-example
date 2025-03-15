load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "task1_lib",
    srcs = [
        "MainWindow.cpp",
    ],
    hdrs = [
        "MainWindow.h"
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "task1",
    srcs = ["main.cpp"],
    deps = [
        ":task1_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

pkg_tar(
    name = "release",
    srcs = [":task1"],
    include_runfiles = True,
    package_dir = "app",
)
