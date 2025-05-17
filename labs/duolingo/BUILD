load("@rules_qt//:qt.bzl", "qt_cc_binary", "qt_cc_library")

qt_cc_library(
    name = "duolingo_lib",
    hdrs = [
        "include/mainwindow.h",
        "include/translation_exercise.h",
        "include/grammar_exercise.h",
        "include/task_types.h",
        "include/difficulty_dialog.h",
    ],
    srcs = [
        "src/mainwindow.cpp",
        "src/translation_exercise.cpp",
        "src/grammar_exercise.cpp",
        "src/difficulty_dialog.cpp",
    ],
    includes = ["include"],
    data = [
        "resources/home_pic.png",
        "resources/correct.wav",
        "resources/wrong.wav",
    ],
    deps = [
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_multimedia"
    ],
)

qt_cc_binary(
    name = "duolingo",
    srcs = ["src/main.cpp"],
    deps = [
        ":duolingo_lib",
        "@rules_qt//:qt_core",
        "@rules_qt//:qt_gui",
        "@rules_qt//:qt_widgets",
        "@rules_qt//:qt_multimedia"
    ],
)