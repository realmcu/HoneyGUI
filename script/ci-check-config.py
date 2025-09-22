ci_check_config = {
    "commit_labels":{
        'GUI': {
            "check_handler": "default_handler", # will call default_handler to check this commit, not used currently
            "build_handler": None
        },
        'DOC': {
            "check_handler": "default_handler", # will call default_handler to check this commit, not used currently
            "build_handler": None
        },
        'Script-CI': {
            "check_handler": "default_handler", # will call default_handler to check this commit, not used currently
            "build_handler": None
        },
        'Script-Nightly': {
            "check_handler": "default_handler", # will call default_handler to check this commit, not used currently
            "build_handler": None
        },
        'Script-Release': {
            "check_handler": "default_handler", # will call default_handler to check this commit, not used currently
            "build_handler": None
        },
        'Script-Other': {
            "check_handler": "default_handler", # will call default_handler to check this commit, not used currently
            "build_handler": None
        }
    },
    "ignore_file_encoding":[
        r"win32_sim/RTE/",
        r"realgui/3rd/",
        r"lvgl_v8/",
        r"lvgl_v9/",
        r"example/application/screen_454_454",
        r"example/application/screen_320_385",
        r"doc/cn",
        r"doc/en/_templates/",
        r"menu_config_win.py",
        r"README.md",
        r"README_zh.md",
        r"example/widget/text/test_font_vector_rendering_benchmark.c",
        r"example/widget/text/test_font_bitmap_rendering_benchmark.c",
        r"example/widget/text/test_multi_language.c",
        r"realgui/engine/draw_font.c",
        r"doxygen_graphviz/readme.md",
        r"tool/D2XML/",
    ],
    "ignore_cpp_astyle_check":[
        r"win32_sim/RTE/",
        r"realgui/3rd/",
        r"lvgl_v8/",
        r"lvgl_v9/",
    ],
    "commit_version_check": {
        "ANCESTOR_ACCEPT_LENGTH": 5, # the 5th commit is the parent, ok // first priority
        "REJECT_COMMIT_DELTA": 24,  # the parent should be in 24 hours  // second priority
        "log_file_key_words": "combination"  # combination or branch   // configured by catherine_guo, do not modify !!!!!!
    },
    # all function must return True or False
    "steps":{
        "before_build": {
            #"Code-Review": {
            #    "function": None, output: res: {result: True, message: "abc"}
            #    "gerrit-label": True , will add value in gerrit
            #    "label-value": False,
            #    "fail-fast": True,  if function return false, fail all the steps

            #},
            "Code-Review": {
                "function": None,
                "label-value": '-1',
                "gerrit-message": "",
                "fail-fast": True,
                "gerrit-label": True
            },
            "Commit-Check": {
                "function": "do_commit_check",
                "fail-fast": True,
                "gerrit-label": True,
                "fail-label-value": "-1", # to set this value if fail，-1 by default
                "pass-label-value": "+1", # to set this value if pass, +1 by default
            },
            "Static-Check": {
                "function": "check_commit_files",
                "fail-fast": True,
                "gerrit-label": True
            },
            "License-Scan": {
                "function": "do_license_check",
                "fail-fast": True,
                "gerrit-label": True
            },
            #"repo-custom-check": {
            #    "function": None, # customer function to call, will not add label value in gerrit
            #    "fail-fast": True,
            #    "gerrit-label": False
            #}
        },
        "after_build": {
            "Code-Review": {
                "function": None,
                "label-value": '+1',
                "fail-fast": True,
                "gerrit-label": True
            },
            "Test-Request": {
                "function": None,
                "label-value": True,
                "fail-fast": True,
                "gerrit-label": True,
                "ignore-if-set": True # if this label is already set, then ignore this step, only take effect when funciton is None
            },
            "Reviewer-Check": {
                "function": None,
                "label-value": '+1',
                "fail-fast": True,
                "gerrit-label": True
            },
            "SDK-API-Review": {
                "function": None,
                "label-value": '+2',
                "fail-fast": True,
                "gerrit-label": True
            },
            #"repo-custom-check": {
            #    "function": None,
            #    "fail-fast": True
            #}
        }
    }
}
