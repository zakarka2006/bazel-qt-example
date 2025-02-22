load("@rules_qt//:qt.bzl", "qt_cc_binary")

qt_cc_binary(
    name = "app",
    srcs = ["main.cpp"],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_widgets",
    ],
)

load("@rules_pkg//pkg:tar.bzl", "pkg_tar")

pkg_tar(
    name = "release",
    srcs = [
        ":app",
    ],
    include_runfiles = True,
    package_dir = "app",
)
