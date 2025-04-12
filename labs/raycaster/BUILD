load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "raycaster_lib",
    hdrs = glob(["include/*.h"]),
    srcs = glob(["src/*.cpp"]),
    includes = ["include"],  # Указываем директорию с заголовками
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)

qt_cc_binary(
    name = "raycaster",
    srcs = ["src/main.cpp"],
    deps = [
        ":raycaster_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
    ],
)
